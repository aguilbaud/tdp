#include <string.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include "mycblas.h"
#include "util.h"


#define BLOCK_SIZE 1 //Number of colums per threads



void create_column_block_type(MPI_Datatype * dt, int col_size){
    MPI_Type_contiguous(BLOCK_SIZE* col_size, MPI_DOUBLE, dt);
    MPI_Type_commit(dt);
}

int is_local_bloc(int rank, int k, int nbthreads){
    int p=k;
    
    if( (k/nbthreads) %2 )
	p = (nbthreads-1) - (k%nbthreads);
    else
	p = k%nbthreads;

    return (p==rank);
}

int get_local_bloc_owner(int k, int nbthreads){
    int p=k;
    
    if( (k/nbthreads) %2 )
	p = (nbthreads-1) - (k%nbthreads);
    else
	p = k%nbthreads;
    
    return p;
}


void dgetrf_mpi(int K, int loc_nb_bloc_colums, MPI_Datatype BLOCK_COLUMN_TYPE,double *A_loc, double *A_buf, const int N, const int lda){
    int comm_rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    //Parcours des blocs colonne
    int k;
    int loc_k=0;
    for (k = 0; k < K; k++) {
	if(is_local_bloc(comm_rank, k, comm_size)){
	    
	    int M_dgetf2 = N - (k*BLOCK_SIZE);
	    int N_dgetf2 = BLOCK_SIZE;
	    double * A_dgetf2 = A_loc + (loc_k*BLOCK_SIZE*lda) + (k * BLOCK_SIZE);
	    // printf("dgetf2: M:%d, N:%d, A:%d\n", M_dgetf2, N_dgetf2, (loc_k*BLOCK_SIZE*lda) + (k * BLOCK_SIZE));
	    mycblas_dgetf2_nopiv(M_dgetf2, N_dgetf2, A_dgetf2, lda);
	    
	    if(loc_k + 1 < loc_nb_bloc_colums){//peut etres pas bonne condition (voir avec loc_k)
		int M_dtrsm = BLOCK_SIZE;
		int N_dtrsm = (loc_nb_bloc_colums - loc_k - 1)*BLOCK_SIZE;
		double *A_dtrsm = A_loc + (loc_k*BLOCK_SIZE*lda) + (k * BLOCK_SIZE);
		double *B_dtrsm = A_loc + ((loc_k+1)*BLOCK_SIZE*lda) + (k * BLOCK_SIZE);

		//printf("rank: %d, M: %d, N: %d A: %d, B: %d\n", comm_rank, BLOCK_SIZE, (loc_nb_bloc_colums - loc_k - 1)*BLOCK_SIZE, (loc_k*BLOCK_SIZE*lda) + (k * BLOCK_SIZE), ((loc_k+1)*BLOCK_SIZE*lda) + (k * BLOCK_SIZE));
		mycblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit, M_dtrsm, N_dtrsm, 1.0, A_dtrsm, lda, B_dtrsm, lda);
		

		int M_dgemm = N - ((k+1) * BLOCK_SIZE);
		int N_dgemm = N_dtrsm;
		double *A_dgemm = A_loc + (loc_k*BLOCK_SIZE*lda) + ((k + 1) *BLOCK_SIZE);
		double *B_dgemm = B_dtrsm;
		double *C_dgemm = A_loc + ((loc_k+1)*BLOCK_SIZE*lda) + ((k + 1) *BLOCK_SIZE);
		//printf("dgemm => rank: %d, k: %d M:%d, N: %d, K: %d, A: %d, B: %d, C%d\n", 
		//	comm_rank, k, M_dgemm, N_dgemm, BLOCK_SIZE,
		//	(loc_k*BLOCK_SIZE*lda) + ((k + 1) *BLOCK_SIZE),
		//	((loc_k+1)*BLOCK_SIZE*lda) + (k * BLOCK_SIZE),
		//	((loc_k+1)*BLOCK_SIZE*lda) + ((k + 1) *BLOCK_SIZE));
		mycblas_dgemm_scalaire(CblasColMajor, CblasNoTrans, CblasNoTrans,
				       M_dgemm, N_dgemm, BLOCK_SIZE, -1.0,
				       A_dgemm, lda, B_dgemm, lda, 1.0, C_dgemm, lda);

	    }
	    //Pour tous les processeurs qui possédent une colonne à droite
	    //printf("Bcast send=> rank: %d, k: %d, cond: %d\n", comm_rank, k,k < K-1 );
	    //if(k < K-1)
	    MPI_Bcast( A_loc  + (loc_k*BLOCK_SIZE*lda), 1, BLOCK_COLUMN_TYPE, comm_rank, MPI_COMM_WORLD);
	    loc_k++;
	    /* TODO: Envoyer num de ligne pivot */	    
	}
	else{
	    //Recevoir seulement si on est a droite du sender
	    //printf("Bcast recv=> rank: %d, k: %d, cond: %d\n", comm_rank, k,loc_k != loc_nb_bloc_colums);
	    
	    MPI_Bcast( A_buf, 1, BLOCK_COLUMN_TYPE, get_local_bloc_owner(k, comm_size), MPI_COMM_WORLD);
	    /* TODO: Recevoir num de ligne pivot */
	    /* TODO: swap lignes sur colonne local */
		
	    if(loc_k != loc_nb_bloc_colums) {
		int M_dtrsm = BLOCK_SIZE;
		int N_dtrsm = (loc_nb_bloc_colums - loc_k) * BLOCK_SIZE;
		double *A_dtrsm = A_buf + (k*BLOCK_SIZE);
		double *B_dtrsm = A_loc + (loc_k * BLOCK_SIZE * lda) + (k*BLOCK_SIZE);
		//printf("dtrsm recv=> rank: %d, k: %d, M: %d, N: %d, A: %d, B:%d\n", 
		//comm_rank, k, M_dtrsm, N_dtrsm, (k*BLOCK_SIZE), (loc_k * BLOCK_SIZE * lda) + (k*BLOCK_SIZE)
		//);
		mycblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit,
			      M_dtrsm, N_dtrsm, 1.0, A_dtrsm, lda, B_dtrsm, lda);

		
		int M_dgemm = N - (k+1) * BLOCK_SIZE;
		int N_dgemm =  N_dtrsm;
		double *A_dgemm = A_buf + ((k+1) * BLOCK_SIZE);		
		double *B_dgemm = A_loc + (loc_k * BLOCK_SIZE * lda) + (k*BLOCK_SIZE);
		double *C_dgemm = A_loc + (loc_k * BLOCK_SIZE * lda) + ((k+1) * BLOCK_SIZE);
		//printf("dgemm recv=> rank: %d, k: %d, M: %d, N: %d, A: %d, B: %d, C: %d\n", 
		//comm_rank, k,M_dgemm, N_dgemm, ((k+1) * BLOCK_SIZE), (loc_k * BLOCK_SIZE * lda) + (k*BLOCK_SIZE), (loc_k * BLOCK_SIZE * lda) + ((k+1) * BLOCK_SIZE));
		mycblas_dgemm_scalaire(CblasColMajor, CblasNoTrans, CblasNoTrans, M_dgemm, N_dgemm, BLOCK_SIZE, -1, A_dgemm, lda, B_dgemm, lda, 1.0, C_dgemm, lda);	    
	    }
	}

    }
}



void lu_mpi(const int M, const int N, double *A, const int lda, int *ipiv, int *info)
{
    int comm_size;
    int comm_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    MPI_Datatype BLOCK_COLUMN_TYPE;
    create_column_block_type(&BLOCK_COLUMN_TYPE, N);
        
    int i, loc_nb_bloc_colums=0;
    int K = N/BLOCK_SIZE;//Nombre de blocs-colonne
    int max_col_per_thread = ceil((double)K/comm_size);
        
    //Buffer local, contient les bloc-colonnes du processus
    double *A_loc = malloc(sizeof(double) * N * BLOCK_SIZE * max_col_per_thread);
    //Buffer contenant les blocs colonnes qu'on recoit
    double *A_buf = malloc(sizeof(double) * N * BLOCK_SIZE);

    //Copie les bloc-colonnes dans le buffer local
    for (i = 0; i < K; i++) {
	if(is_local_bloc(comm_rank, i, comm_size)){
	    memcpy(A_loc + (loc_nb_bloc_colums*N*BLOCK_SIZE), 
		   A +(i*N*BLOCK_SIZE), 
		   sizeof(double)*N*BLOCK_SIZE);
	    loc_nb_bloc_colums++;
	}
    }

    //Facto lu
    dgetrf_mpi(K, loc_nb_bloc_colums, BLOCK_COLUMN_TYPE, A_loc, A_buf, N, lda);
    
    
    //On recolle les morceaux
    MPI_Gather(A_loc, loc_nb_bloc_colums, BLOCK_COLUMN_TYPE, A, loc_nb_bloc_colums, BLOCK_COLUMN_TYPE, 0, MPI_COMM_WORLD);

    MPI_Type_free(&BLOCK_COLUMN_TYPE);
}
