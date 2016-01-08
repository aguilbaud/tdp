#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "mycblas.h"
#include "mkl.h"
#include "util.h"


int main(int argc, char **argv){
  double *A=NULL, *B=NULL, *C=NULL;
  int N;
  double start, elapsed;
  FILE *fp = fopen(argv[1], "r");
  A = load_file(fp, &N);
  fclose(fp);
  
  B = malloc(sizeof(double)*N*N);
  C = malloc(sizeof(double)*N*N);
  memcpy(B, A, N * sizeof(double));
  memcpy(C, A, N * sizeof(double));
  
  start = MPI_Wtime();
  cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, A, N, B, N, 0.0, C, N);
  elapsed = MPI_Wtime() - start;
  
  printf("%f\n", elapsed);

  free(A);
  free(B);
  free(C);
}
