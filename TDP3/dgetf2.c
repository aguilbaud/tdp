#include "cblas.h"

void cblas_dscal(const int N, const double alpha, double *X, const int incX){
  for(int i=0; i < N; i+= incX)
    X[i] *= alpha;
}

void dgetf_2_nopiv(const int M, const int N, double * A, const int lda){
  for(int i = 1; i < M; i++){
    dscal();
    dger();
      
  }
}
