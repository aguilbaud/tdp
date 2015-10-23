#ifndef _UTIL_H
#define _UTIL_H
#include <stdio.h>

void affiche(int m, int n, double * a, int lda, FILE * f);
double * alloc_vector(int n);
void init_vector(int n, double * v);
double * alloc_matrix(int m, int n);
void init_matrix(int m, int n, double * a,int F);

#endif
