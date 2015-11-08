#ifndef _DGETRF_H
#define _DGETRF_H
#include <stdio.h>
#include "dgetf2.h"
#include "cblas.h"
#include "dgemm.h"
#include "util.h"

void dgetrf(const int M, const int N,
	    double *A, const int lda,
	    int *ipiv, int info);

#endif
