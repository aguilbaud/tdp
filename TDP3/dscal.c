#include "mycblas.h"
     
void mycblas_dscal(const int N, const double alpha, double *X, const int incX)
{
  for(int i=0; i < N; i++, X+=incX)
    (*X) *= alpha;
} 
