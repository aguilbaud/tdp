#include <stdio.h>
#include <stdlib.h>

void affiche(int m, int n, double * a, int lda, FILE * f){
  int l,c;
  for(l=0;l<m;l++){
    for(c=0;c<n;c++){
      fprintf(f, "%f ", a[c*lda + l]);
    }
    fprintf(f,"\n");
  }
}


double * alloc_vector(int n){
  return malloc(sizeof(double)*n);//TODO check result
}

void init_vector(int n, double * v){
  int i;
  for(i=0;i<n;i++)
    v[i] = i;
}

double * alloc_matrix(int m, int n){
    return calloc(m*n, sizeof(double));//TODO check result
}

//if F=1; fill a with 0
void init_matrix(int m, int n, double * a, int F){
  int i;

  for(i=0;i<n*m;i++)
    if(!F)
      a[i]=i;
    else
      a[i]=0;
}

  
