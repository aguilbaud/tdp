#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "mycblas.h"
#include "mpi_fox.h"
#include "util.h"

void init_cart_comm(MPI_Comm *grid, MPI_Comm *line_comm, int n){
    int dims[] = {n, n};
    int periods[] = {0, 1};
    int rem_dims[] = {1, 0};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, grid);
    MPI_Cart_sub(*grid, rem_dims, line_comm);
}

void init_datatype(int dim, int bloc_size, MPI_Datatype *mpi_type_bloc){
    MPI_Datatype mpi_type_bloc1;
    MPI_Type_vector(bloc_size, bloc_size, bloc_size*dim, MPI_DOUBLE, &mpi_type_bloc1);

    MPI_Aint a;
    
    MPI_Type_extent(mpi_type_bloc1, &a); printf ("bef; %d\n",a/sizeof(double));
        
    MPI_Type_create_resized(mpi_type_bloc1, 0,(bloc_size==1)?sizeof(double):bloc_size*sizeof(double), mpi_type_bloc);
    MPI_Type_extent(*mpi_type_bloc, &a); printf ("aft; %d\n",a/sizeof(double));
    MPI_Type_commit(mpi_type_bloc);
    MPI_Type_free(&mpi_type_bloc1); 
}

void fox_scatter(const double *A, double *A_loc, int bloc_size, MPI_Datatype *mpi_type_bloc, int dim){
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

    void fox_gather(double *A, double *A_loc, int bloc_size, MPI_Datatype mpi_type_bloc, int dim){
      int * rcounts = malloc(sizeof(int) * dim*dim);
      int * displs = malloc(sizeof(int) * dim*dim);
      int k=0;

      int rank;
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      if(rank==0){
	for (int j = 0; j < dim; j++) {
	  for (int i = 0; i < dim; i++) {
	    rcounts[k] = 1;
	    displs[k] = (j*dim*bloc_size) + i;
	    k++;
	  }
	}
      }
      MPI_Gatherv(A_loc, bloc_size*bloc_size, MPI_DOUBLE, A, rcounts, displs, mpi_type_bloc, 0, MPI_COMM_WORLD);

      free(rcounts);
      free(displs);
    }

    void fox_compute(const int loc_N, const int dim, 
		     const double *loc_A, const double *loc_B, double *loc_C, 
		     const int rank, MPI_Comm grid_comm, MPI_Comm line_comm){

      int my_coords[2];
      int line_rank, num_line, line_root;
      MPI_Request req_send, req_recv;
      double *buf_A = malloc(sizeof(double) * loc_N * loc_N);
      double *buf_B = malloc(sizeof(double) * loc_N * loc_N);
      memcpy(buf_B, loc_B, sizeof(double) * loc_N * loc_N);

      /*Getting the rank of a each proc in its line*/ 
      MPI_Cart_coords(grid_comm, rank, dim, my_coords);
      num_line = my_coords[1];    
      my_coords[1] = 0;
      MPI_Cart_rank(line_comm, my_coords, &line_rank);
    
      for (int k = 0; k < dim; k++) {
	/*The sender for this step copy its local submatrix A in a buffer*/
	line_root =  (num_line+k)%dim;		
	if(line_root==line_rank)
	  memcpy(buf_A, loc_A, sizeof(double) * loc_N * loc_N);

	MPI_Bcast(buf_A, loc_N*loc_N, MPI_DOUBLE, line_root, line_comm);

	/*Classic dgemm between C, received A and B*/
	mycblas_dgemm_scalaire(CblasColMajor, CblasNoTrans, CblasNoTrans, loc_N, loc_N, loc_N, 
			       1.0, buf_A, loc_N, buf_B, loc_N, 0.0, loc_C, loc_N);

	/*B shift*/
	int shift_src, shift_dst;
	MPI_Cart_shift(grid_comm, 1, -1, &shift_src, &shift_dst);
	
	MPI_Isend(buf_B, loc_N*loc_N, MPI_DOUBLE, shift_dst, 0, grid_comm, &req_send);
	MPI_Irecv(buf_B, loc_N*loc_N, MPI_DOUBLE, shift_src, 0, grid_comm, &req_recv);
	MPI_Wait(&req_send, MPI_STATUS_IGNORE);
	MPI_Wait(&req_recv, MPI_STATUS_IGNORE);
	
	//MPI_Sendrecv_replace(buf_B, loc_N*loc_N, MPI_DOUBLE, shift_dst, 0, shift_src, 0, grid_comm, MPI_STATUS_IGNORE);
      }
      free(buf_A);
      free(buf_B);
    }


    void fox(const double *A, const double *B, double *C, int loc_N, int dim){
      int comm_size, comm_rank;
      double *loc_A, *loc_B, *loc_C;
      double start, time_scatter, time_gather, time_compute;
      MPI_Comm grid_comm, line_comm;
      MPI_Datatype mpi_type_bloc;
    
      MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
      MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);	
        
      /*Broadcast of the size*/
      MPI_Bcast(&loc_N, 1, MPI_INT, 0, MPI_COMM_WORLD);
      loc_A = malloc(sizeof(double) * loc_N * loc_N);
      loc_B = malloc(sizeof(double) * loc_N * loc_N);
      loc_C = malloc(sizeof(double) * loc_N * loc_N);

      /*Grid, communicator & datatype creation*/
      init_cart_comm(&grid_comm, &line_comm, dim);
      init_datatype(dim, loc_N, &mpi_type_bloc);
    
      /*Scatter matrices*/
      MPI_Barrier(MPI_COMM_WORLD);
      start = MPI_Wtime();
      fox_scatter(A ,loc_A, loc_N, &mpi_type_bloc, dim);

      char dest[8];
      sprintf(dest, "%02d.mat", comm_rank);
      FILE *fp = fopen(dest, "w+");
      affiche(loc_N, loc_N, loc_A, loc_N, fp);
      fclose(fp);
    
      fox_scatter(B ,loc_B, loc_N, &mpi_type_bloc, dim);
      fox_scatter(C ,loc_C, loc_N, &mpi_type_bloc, dim);
      time_scatter = MPI_Wtime() - start;
    
      /*Fox algorithm*/
      start = MPI_Wtime();
      fox_compute(loc_N, dim, loc_A, loc_B, loc_C, comm_rank, grid_comm, line_comm);
      time_compute = MPI_Wtime() - start;

      /*Gather result*/
      start = MPI_Wtime();
      fox_gather(C ,loc_C, loc_N, mpi_type_bloc, dim);
      time_gather = MPI_Wtime() - start;

      printf("%d %f %f %f\n", comm_rank, time_scatter, time_compute, time_gather);
      if(comm_rank == 0){
	//affiche(N,N,C,N,stdout);
	
      }
	
      free(loc_A); free(loc_B); free(loc_C);
    }
