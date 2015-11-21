#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#define MAX 10.0
#define MIN -10.0

double * load_file(FILE * fp, int *n){
    
  if(fscanf(fp, "%d", n) != 1){
    fprintf(stderr, "Cannot read input file\n");
    return NULL;
  }
  
  
  double * values = malloc ((*n)*sizeof(double));
  double * A = malloc((*n)*(*n)*sizeof(double));
  
  for (int i = 0; i < (*n); ++i){
    //if(fscanf(fp, "%lf %lf %lf %lf %lf", &m,&px,&py,&vx,&vy) != 5){
    if(fread(values, sizeof(double), (*n), fp)!=(*n)){
      fprintf(stderr, "Cannot read input file.\n");
      return NULL;
    }
    
    memcpy(A + (i*(*n)), values, (*n)*sizeof(double));
  }

  return A;
}

void write_file(FILE * fp, const int n, double * A){
  
  fprintf(fp, "%d\n", n);
  double * values = malloc(n*sizeof(double));
  for (int i = 0; i<n; ++i){
    for (int j = 0; i<n; ++j){
      values[i]=A[i*n+j];
    }
    fwrite(values,sizeof(double),n,fp);
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
