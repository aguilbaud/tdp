#include "mycblas.h"

void mycblas_dgetf2_nopiv(const int M, const int N, double * A, const int lda){
    for(int i = 0; i < N; i++){
	double *col = A + (i*lda + (i+1));
	double *line = A + ((i+1)*lda + i);
	double *ssmat = A + ((i+1)*lda + (i+1));

	mycblas_dscal(M-(i+1), (1/A[i *lda + i]), col, 1);
	mycblas_dger(CblasRowMajor, M-(i+1), N-(i+1), -1.0, col, 1.0, line, lda, ssmat, N);
    }
}
