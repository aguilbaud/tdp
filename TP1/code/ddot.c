#include "cblas.h"

double cblas_ddot(const int N, const double *X, const int incX,
                  const double *Y, const int incY){
  if(N<=0)
    return 0.;

  double r = 0.;
  int i;
  for(i=0;i<N;i++, X += incX, Y += incY)
    r+= *(X) * *(Y);
  
  return r;
}
