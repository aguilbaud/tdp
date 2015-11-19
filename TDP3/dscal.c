#include "mycblas.h"

void mycblas_dscal(const int N, const double alpha, double *X, const int incX)
{

    for(int i=0; i < N; i++, X+=incX){
	/* printf("-==== DSCAL ====-\n"); */
	/* printf("%3.3f = %3.3f / %3.3f", (*X), (*X), alpha);  */
	(*X) *= alpha;
	/* printf(" = %3.3f\n", (*X)); */
	/* printf("-==== FIN DSCAL ====-\n"); */
    }
    
} 
