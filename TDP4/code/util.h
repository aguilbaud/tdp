#ifndef _UTIL_H
#define _UTIL_H


double * load_file(FILE * fp, int *n);

void write_file(FILE * fp, const int n, double * A);

void generate_rand_matrix(FILE *fp, const int n);

#endif
