#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define N 500
#define NB_ITER 99 

int main(int argc, char *argv[])
{
  /*pour chaque itération, accéleration
   *calcul des forces
   *calcul nouveau dt
   *mise a jour en fonction de dt: p,v,a
   */
  
  /* particle_t * p = init_universe(N); */
  /* write_file("test.univ", N, p); */

  int n;
  particle_t *p = load_file("test.univ", &n);

  for(int i = 0; i< NB_ITER; i++){
    char dest[18] ;
    sprintf(dest,"output/test%2.2d.bmp", i);
    printf("%s\n",dest);
    write_img(dest, n  , p);
    move(p,n);
  }

  free(p);
  return 1000;
}

