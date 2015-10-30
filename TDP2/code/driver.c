#include <mpi.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "util.h"

#define N 50
#define NB_ITER 20

void usage(){
  printf("Usage:\n\
-i <x>\t\tNombre d'itérations\n\
-n <x>\t\tNombre de particules\n		\
-g <file>\tGenerate a random pattern\n		\
-s <file>(opt)\tSequential execution\n\n	\
");
}

int main(int argc, char *argv[])
{
  /*pour chaque itération, accéleration
   *calcul des forces
   *calcul nouveau dt
   *mise a jour en fonction de dt: p,v,a
   */
  int nb_iter = 20;
  int nb_particle = 50;
  int n;
  FILE *fp = NULL;

  
  int n_flag = 0;
  int i_flag = 0;
  int g_flag = 0;
  int s_flag = 0;
  char *generate_path = NULL;
  char *exec_path = NULL;

  char *cvalue = NULL;
  int c;
  
  opterr = 0;

  while ((c = getopt (argc, argv, "i:n:g:s:")) != -1)
    switch (c) {
    case 'n':
      cvalue = optarg;
      nb_particle = atoi(cvalue);
      n_flag = 1;
      break;
    case 'i':
      cvalue = optarg;
      nb_iter = atoi(cvalue);
      i_flag = 1;
      break;
    case 'g':
      cvalue = optarg;
      generate_path = cvalue;
      g_flag = 1;
      break;
    case 's':
      cvalue = optarg;
      exec_path = cvalue;
      s_flag = 1;
      break;
    case '?':
      printf("%d\n",optopt);
      if (optopt == 'n' || optopt == 's')
	fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint (optopt))
	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
	fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
      usage();
      return 1;
    default:
      return EXIT_FAILURE;
      break;
    }

  if(g_flag){
    if(n_flag == 0)
      printf("Number of particles not set, using default value.\n");
    
    fp = fopen(generate_path,"w");
    if(fp == NULL){
      printf("Problem opening file:%s\n",generate_path);
      return EXIT_FAILURE;
    }
    srand(time(NULL));
    particle_t * p = init_universe(nb_particle); 
    write_file(fp, nb_particle, p);
    fclose(fp);
    free(p);

  }
  if(s_flag){
    if(i_flag == 0)
      printf("Number of iterations not set, using default value.\n");
      
    fp = fopen(exec_path,"r");
    if(fp == NULL){
      printf("Problem opening file:%s\n",exec_path);
      return EXIT_FAILURE;
    }
    
    particle_t *p = load_file(fp, &n);
    int rate = (nb_iter<100)?nb_iter:nb_iter/1000;
    
    for(int i = 0; i< nb_iter; i++){
      printf("\rIter: %d/%d", i+1, nb_iter);
      if((i%rate) == 1){
	//char dest[25] ;
	//write_img_1bpp(dest, n  , p);
	write_plot("output/test.dat", n, p);
      }
      move(p,n);
    }
    printf("\n");
    fclose(fp);
    free(p);
    system("./plot.gp;eog foobar.gif");
  }
  return 1000;
}

