#ifndef _MPI_FOX_H_
#define _MPI_FOX_H_

#define BLOCK_SIZE 2

void fox_compute();
void fox_scatter(const double *A, double *A_loc, int n, MPI_Datatype *mpi_type_bloc, int gsize);
void fox_gather(double *A, double *A_loc, int bloc_size, MPI_Datatype mpi_type_bloc, int gsize);

/********Called by all processes********/
void fox();

#endif /* _MPI_FOX_H_ */
