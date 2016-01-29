#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

int BS;

#define cell( _i_, _j_ ) board[ ldboard * (_i_) + (_j_) ]
#define cell2( _i_, _j_ ) loc_A_neigh[ (loc_BS + 2) * (_i_) + (_j_) ]
#define ngb( _i_, _j_ )  nbngb[ ldnbngb * ((_i_) - 1) + ((_j_) - 1 ) ]

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define UPPERLEFT 4
#define UPPERRIGHT 5
#define LOWERRIGHT 6
#define LOWERLEFT 7

#define LINE 8
#define COLUMN 9

double mytimer(void)
{
    struct timeval tp;
    gettimeofday( &tp, NULL );
    return tp.tv_sec + 1e-6 * tp.tv_usec;
}

/**
 * This function generates the iniatl board with one row and one
 * column of living cells in the middle of the board
 */
int generate_initial_board(int N, int *board, int ldboard)
{
    int i, j, num_alive = 0;

    for (i = 0; i < N; i++) {
	for (j = 0; j < N; j++) {
	    if (i == N/2 || j == N/2) {
		cell(i, j) = 1;
		num_alive ++;
	    }
	    else {
		cell(i, j) = 0;
	    }
	}
    }

    return num_alive;
}


void init_cart_comm(MPI_Comm *grid, MPI_Comm *line_comm, int n){
    int dims[] = {n, n};
    int periods[] = {1, 1};
    int rem_dims[] = {1, 0};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, grid);
    MPI_Cart_sub(*grid, rem_dims, line_comm);
}

void init_datatype(int dim, int bloc_size, MPI_Datatype *mpi_type_bloc){
    MPI_Datatype mpi_type_bloc1;
    MPI_Type_vector(bloc_size, bloc_size, bloc_size*dim +2, MPI_INT, &mpi_type_bloc1);
    MPI_Type_create_resized(mpi_type_bloc1, 0, sizeof(int), mpi_type_bloc);
    MPI_Type_commit(mpi_type_bloc);
    MPI_Type_free(&mpi_type_bloc1); 
}

void scatter(const int *A, const int *A_loc, int bloc_size, MPI_Datatype *mpi_type_bloc, int dim, MPI_Comm grid){
    int * scounts = malloc(sizeof(int) * dim*dim);
    int * displs = malloc(sizeof(int) * dim*dim);
    int k=0;
    int ldboard = dim*bloc_size + 2;
    for (int j = 0; j < dim; j++) {
	for (int i = 0; i < dim; i++) {
	    scounts[k] = 1;
	    displs[k] = (j*ldboard*bloc_size) + (i*bloc_size);
	    k++;
	}
    }
    MPI_Scatterv(A, scounts, displs, *mpi_type_bloc, A_loc, bloc_size*bloc_size, MPI_INT, 0, grid);

    free(scounts);
    free(displs);
}

void gather(const int *A, const int *A_loc, int bloc_size, MPI_Datatype mpi_type_bloc, int dim, MPI_Comm grid){
    int * rcounts = malloc(sizeof(int) * dim*dim);
    int * displs = malloc(sizeof(int) * dim*dim);
    int k=0;
    int ldboard = dim*bloc_size + 2;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(rank==0){
      for (int j = 0; j < dim; j++) {
	for (int i = 0; i < dim; i++) {
	  rcounts[k] = 1;
	  displs[k] = (j*ldboard*bloc_size) + (i*bloc_size);
	  k++;
	}
      }
    }
    MPI_Gatherv(A_loc, bloc_size*bloc_size, MPI_INT, A, rcounts, displs, mpi_type_bloc, 0, grid);

    free(rcounts);
    free(displs);
}


void create_neighbors(int* neighbors, MPI_Comm grid, int myrank){
  int displ;
  int index;
  int coords[2];

  index = 1;
  displ = 1;
  MPI_Cart_shift(grid, index, displ, &neighbors[LEFT], &neighbors[RIGHT]);
  index = 0;
  MPI_Cart_shift(grid, index, displ, &neighbors[UP], &neighbors[DOWN]);
  MPI_Cart_coords(grid, myrank, 2, coords);
  coords[0]--;
  coords[1]--;
  MPI_Cart_rank(grid, coords, &neighbors[UPPERLEFT]);
  coords[1]+=2;
  MPI_Cart_rank(grid, coords, &neighbors[UPPERRIGHT]);
  coords[0]+=2;
  MPI_Cart_rank(grid, coords, &neighbors[LOWERRIGHT]);
  coords[1]-=2;
  MPI_Cart_rank(grid, coords, &neighbors[LOWERLEFT]);
}

void do_communications(MPI_Request* rq, int* neighbors, int block_size, MPI_Comm grid, int *tab, int ldtab, MPI_Datatype block_column, int comm_rank){
  // Vers le haut
  MPI_Isend(&tab[ldtab+1], block_size, MPI_INT, neighbors[UP], 0, grid, &rq[0]);
  MPI_Irecv(&tab[ldtab*(ldtab-1)+1], block_size, MPI_INT, neighbors[DOWN], 0, grid, &rq[0]);
  // Vers le bas
  MPI_Isend(&tab[ldtab*(ldtab-2)+1], block_size, MPI_INT, neighbors[DOWN], 0, grid, &rq[1]);
  MPI_Irecv(&tab[1], block_size, MPI_INT, neighbors[UP], 0, grid, &rq[1]);
  // Vers la droite
  MPI_Isend(&tab[ldtab-2], 1, block_column,neighbors[RIGHT], 0, grid, &rq[2]);
  MPI_Irecv(&tab[0], 01, block_column, neighbors[LEFT], 0, grid, &rq[2]);
  // Vers la gauche
  MPI_Isend(&tab[1], 1, block_column, neighbors[LEFT], 0, grid, &rq[3]);
  MPI_Irecv(&tab[ldtab-1], 1, block_column, neighbors[RIGHT], 0, grid, &rq[3]);
  // Vers le haut droit
  MPI_Isend(&tab[ldtab*2 - 2], 1, MPI_INT, neighbors[UPPERRIGHT], 0, grid, &rq[4]);
  MPI_Irecv(&tab[ldtab*(ldtab-1)], 1, MPI_INT, neighbors[LOWERLEFT], 0, grid, &rq[4]);
  // Vers le bas droit
  MPI_Isend(&tab[ldtab*(ldtab-1)-2], 1, MPI_INT, neighbors[LOWERRIGHT], 0, grid, &rq[5]);
  MPI_Irecv(&tab[0], 1, MPI_INT, neighbors[UPPERLEFT], 0, grid, &rq[5]);
  // Vers le haut gauche
  MPI_Isend(&tab[ldtab+1], 1, MPI_INT, neighbors[UPPERLEFT], 0, grid, &rq[6]);
  MPI_Irecv(&tab[ldtab*ldtab - 1], 1, MPI_INT, neighbors[LOWERRIGHT], 0, grid, &rq[6]);
  // Vers le bas gauche
  MPI_Isend(&tab[ldtab*(ldtab-2)+1], 1, MPI_INT,neighbors[LOWERLEFT], 0, grid, &rq[7]);
  MPI_Irecv(&tab[ldtab-1], 1, MPI_INT, neighbors[UPPERRIGHT], 0, grid, &rq[7]);
}

void calc_ngb(int k, int type, int block_size, int* loc_A_neigh, int ldboard, int* nbngb, int ldnbngb)
{
  int loc_BS = block_size;
  if (type == LINE)
    {
      int i = k;
      for (int j = 1; j <= block_size; j++) {
	ngb( i, j ) =
	  cell2( i-1, j-1 ) + cell2( i, j-1 ) + cell2( i+1, j-1 ) +
	  cell2( i-1, j ) + cell2( i+1, j ) +
	  cell2( i-1, j+1 ) + cell2( i, j+1 ) + cell2( i+1, j+1 );
      }
    }
  if (type == COLUMN)
    {
      int j = k;
      for (int i = 1; i <= block_size; i++) {
	ngb( i, j ) =
	  cell2( i-1, j-1 ) + cell2( i, j-1 ) + cell2( i+1, j-1 ) +
	  cell2( i-1, j ) + cell2( i+1, j ) +
	  cell2( i-1, j+1 ) + cell2( i, j+1 ) + cell2( i+1, j+1 );
      }
    }
}


int main(int argc, char* argv[]){
    
    int comm_size, comm_rank, dim, loc_BS;
    int loop, maxloop, ldboard, ldnbngb;
    int num_alive;
    int *board, *nbngb;
    MPI_Comm grid_comm, line_comm;
    MPI_Datatype mpi_type_bloc;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);	
    
    if (argc < 3) {
	printf("Usage: %s nb_iterations size\n", argv[0]);
	MPI_Finalize();
	return 0;
    } else {
	maxloop = atoi(argv[1]);
	BS = atoi(argv[2]);
    }

    //La grille peut elle etre un carre ?
    dim = sqrt(comm_size);
    if(dim*dim != comm_size){
	fprintf(stderr, "The number of processus must be a power\n");
	MPI_Finalize();
	return 0;
    }
    
    loc_BS = BS/dim;
    if(loc_BS*dim != BS){
	fprintf(stderr, "%d is not divisible by %d\n", BS, dim);
	MPI_Finalize();
	return 0;
    }
    
    init_cart_comm(&grid_comm, &line_comm, dim);
    init_datatype(dim, loc_BS, &mpi_type_bloc);

    //Un processus initialise la grille
    if(comm_rank == 0){

	ldboard = BS + 2;
	board = malloc( ldboard * ldboard * sizeof(int) );

	num_alive = generate_initial_board( BS, &(cell(1, 1)), ldboard );

	// affichage de la grille initiale
	/*for(int i=1; i<=BS; i++){
	  for(int j=1; j<=BS; j++){
	    printf("%d ", cell(i, j));
	  }
	  printf("\n");
	  }*/
    }
    
    ldnbngb = loc_BS;
    nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );
    int * loc_A = malloc(loc_BS * loc_BS * sizeof(int));

    //Eclatement de la matrice
    scatter(&cell(1, 1), loc_A, loc_BS, &mpi_type_bloc, dim, grid_comm);


    // Permet de stocker les voisins reçus
    int * loc_A_neigh = malloc((loc_BS+2) * (loc_BS+2) * sizeof(int));
    for(int i=0; i<loc_BS; i++){
      for(int j=0; j<loc_BS; j++){
	loc_A_neigh[(i+1)*(loc_BS+2) + j+1] = loc_A[i*loc_BS + j];
      }
    }
    
    // creation de la table des voisins
    int neighbors[8];
    create_neighbors(neighbors, grid_comm, comm_rank);
    
    // Type pour l'échange de colonnes
    MPI_Datatype block_column;
    MPI_Type_vector(loc_BS, 1, loc_BS+2, MPI_INT, &block_column);
    MPI_Type_commit(&block_column);
    
    MPI_Status st;
    MPI_Request rq[8];
    // début de l'exécution des boucles
    for(loop=0; loop<maxloop; loop++){
      // communacations
      do_communications(rq, neighbors, loc_BS, grid_comm, loc_A_neigh, loc_BS+2, block_column, comm_rank);

      //Compte le nombre de voisins de la cellule [i,j] -> cellules interieures
      for (int j = 2; j <= loc_BS-1; j++) {
	for (int i = 2; i <= loc_BS-1; i++) {
	  ngb( i, j ) =
	    cell2( i-1, j-1 ) + cell2( i, j-1 ) + cell2( i+1, j-1 ) +
	    cell2( i-1, j ) + cell2( i+1, j ) +
	    cell2( i-1, j+1 ) + cell2( i, j+1 ) + cell2( i+1, j+1 );
	}
      }
      
      //Compte le nombre de voisins pour les cellules sur les bords
      //Colonne gauche
      MPI_Wait(&rq[2], &st);
      MPI_Wait(&rq[4], &st);
      MPI_Wait(&rq[5], &st);
      calc_ngb(1, COLUMN, loc_BS, loc_A_neigh, loc_BS+2, nbngb, ldnbngb);
      //Ligne du haut
      MPI_Wait(&rq[1], &st);
      MPI_Wait(&rq[7], &st);
      calc_ngb(1, LINE, loc_BS, loc_A_neigh, loc_BS+2, nbngb, ldnbngb);
      //Colonne droite
      MPI_Wait(&rq[3], &st);
      MPI_Wait(&rq[6], &st);
      calc_ngb(loc_BS, COLUMN, loc_BS, loc_A_neigh, loc_BS+2, nbngb, ldnbngb);
      //Ligne du bas
      MPI_Wait(&rq[0], &st);
      calc_ngb(loc_BS, LINE, loc_BS, loc_A_neigh, loc_BS+2, nbngb, ldnbngb);

      //Règle pour la vie des cellules
      num_alive = 0;
      for (int j = 1; j <= loc_BS; j++) {
	for (int i = 1; i <= loc_BS; i++) {
	  if ( (ngb( i, j ) < 2) ||
	       (ngb( i, j ) > 3) ) {
	    cell2(i, j) = 0;
	  }
	  else {
	    if ((ngb( i, j )) == 3)
	      cell2(i, j) = 1;
	  }
	  if (cell2(i, j) == 1) {
	    num_alive ++;
	  }
	}
      }
    }
    
    for(int i=0; i<loc_BS; i++){
      for(int j=0; j<loc_BS; j++){
	 loc_A[i*loc_BS + j] = loc_A_neigh[(i+1)*(loc_BS+2) + j+1];
      }
    }
    /*
    if(comm_rank==3){
      printf("+----loc_A----+\n");
      for(int i=0;i<loc_BS;i++){
	for(int j=0;j<loc_BS;j++){
	  printf("%d ", loc_A[i*loc_BS + j]);
	}
	printf("\n");
      }
      printf("+----Fin loc_A----+\n");
    }
    */
    gather(&cell(1, 1), loc_A, loc_BS, mpi_type_bloc, dim, grid_comm);
    MPI_Allreduce(MPI_IN_PLACE, &num_alive, 1, MPI_INT, MPI_SUM, grid_comm);
    
    if(comm_rank ==0){
      // affichage de la grille finale
      /*printf("+-----------------------+\n");
      for(int i=1; i<=BS; i++){
	  for(int j=1; j<=BS; j++){
	    printf("%d ", cell(i, j));
	  }
	  printf("\n");
	  }
      */
      printf("Final number of living cells : %d\n", num_alive);
    }

    MPI_Type_free(&mpi_type_bloc); 
    MPI_Finalize();

}
