#include <mpi.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "util.h"

#define N 50
#define NB_ITER 20

void usage(){
  
}

int main(int argc, char *argv[])
{
  /*pour chaque itération, accéleration
   *calcul des forces
   *calcul nouveau dt
   *mise a jour en fonction de dt: p,v,a
   */
  int nb_iter = 20;


  int nflag = 0;
  int bflag = 0;
  char *cvalue = NULL;
  int index;
  int c;

  opterr = 0;

  while ((c = getopt (argc, argv, "bn:")) != -1)
    switch (c)
      {
      case 'b':
        bflag = 1;
        break;
      case 'n':
	cvalue = optarg;
	nb_iter = atoi(cvalue);
	printf("%d\n",nb_iter);
	break;
      case '?':
	if (optopt == 'c')
	  fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	else if (isprint (optopt))
	  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	else
	  fprintf (stderr,
		   "Unknown option character `\\x%x'.\n",
		   optopt);
	return 1;
      default:
	return EXIT_FAILURE;
      }
  
  /* particle_t * p = init_universe(N); */
  /* write_file("test.univ", N, p); */


  int n;
  particle_t *p = load_file("simple_orbit.univ", &n);

  for(int i = 0; i< nb_iter; i++){
    char dest[18] ;
    sprintf(dest,"output/test%2.2d.bmp", i);
   
    write_img(dest, n  , p);
    move(p,n);
  }
  
  free(p);
  return 1000;
 }

