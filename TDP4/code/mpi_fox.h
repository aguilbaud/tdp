#ifndef _MPI_FOX_H_
#define _MPI_FOX_H_

#define BLOCK_SIZE 2

void fox_compute();
void fox_scatter(int n, MPI_Datatype *mpi_type_bloc);
void fox_gather();

/********Called by all processes********/
void fox();

#endif /* _MPI_FOX_H_ */
