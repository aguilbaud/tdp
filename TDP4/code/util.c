#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#define MAX 10.0
#define MIN -10.0

void affiche(int m, int n, double * a, int lda, FILE * f){
    int l,c;
    for(l=0;l<m;l++){
	for(c=0;c<n;c++){
	    fprintf(f, "%03.3f ", a[c*lda + l]);
	}
	fprintf(f,"\n");
    }
}


double * load_file(FILE * fp, int *n){
    
    if(fscanf(fp, "%d", n) != 1){
	fprintf(stderr, "Cannot read input file\n");
	return NULL;
    }  

    double * A = malloc((*n)*(*n)*sizeof(double));

    
    for (int i = 0; i < (*n); ++i){
	for (int j = 0; j < (*n); ++j){
	    if(fscanf(fp, "%lf", &A[j*(*n)+i]) != 1){
		fprintf(stderr, "Cannot read input file.\n");
		free(A);
		return NULL;
	    }    
	}
    }
    return A;
}

void write_file(FILE * fp, const int n, double * A){
  
    fprintf(fp, "%d\n", n);
	for (int i = 0; i<n; ++i){
	    for (int j = 0; i<n; ++j){
		fprintf(fp,"%f ", A[j*n+i]);
	    }
	    fprintf(fp, "\n");
	}

    }

    void generate_rand_matrix(FILE *fp, const int n){
	fprintf(fp, "%d\n", n);
	for (int i = 0; i<n; ++i){
	    for (int j = 0; j<n; ++j){
		fprintf(fp, "%lf ", (rand()/(double)RAND_MAX) * (MAX-MIN) + MIN);
	    }
	    fprintf(fp, "\n");
	}
    }
