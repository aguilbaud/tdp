#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

int BS;

#define cell( _i_, _j_ ) board[ ldboard * (_j_) + (_i_) ]
#define ngb( _i_, _j_ )  nbngb[ ldnbngb * ((_j_) - 1) + ((_i_) - 1 ) ]

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
    MPI_Type_vector(bloc_size, bloc_size, bloc_size*dim, MPI_DOUBLE, &mpi_type_bloc1);
    MPI_Type_create_resized(mpi_type_bloc1, 0, bloc_size*sizeof(int), mpi_type_bloc);
    MPI_Type_commit(mpi_type_bloc);
    MPI_Type_free(&mpi_type_bloc1); 
}

void scatter(const int *A, const *A_loc, int bloc_size, MPI_Datatype *mpi_type_bloc, int dim){
    int * scounts = malloc(sizeof(int) * dim*dim);
    int * displs = malloc(sizeof(int) * dim*dim);
    int k=0;

    for (int j = 0; j < dim; j++) {
	for (int i = 0; i < dim; i++) {
	    scounts[k] = 1;
	    displs[k] = (j*dim*bloc_size) + i;
	    k++;
	}
    }
    
    MPI_Scatterv(A, scounts, displs, *mpi_type_bloc, A_loc, bloc_size*bloc_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    free(scounts);
    free(displs);
}


int main(int argc, char* argv[]){
    
    int comm_size, comm_rank, dim, loc_BS;
    int maxloop, ldboard, ldnbngb;
    int* board, nbngb;
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
    
    //
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
	num_alive = 0;

	/* Leading dimension of the board array */
	ldboard = BS + 2;
	/* Leading dimension of the neigbour counters array */
	ldnbngb = BS;

	board = malloc( ldboard * ldboard * sizeof(int) );
	nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );

	num_alive = generate_initial_board( BS, &(cell(1, 1)), ldboard );
    }

    //Eclatement de la matrice
    fox_scatter(board, loc_A, loc_N, &mpi_type_bloc, dim);


    MPI_Type_free(&mpi_type_bloc); 
    MPI_Finalize();

}
