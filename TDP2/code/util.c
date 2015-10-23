#include <stdio.h>
#include <stdlib.h>
#include "util.h"


particle_t * load_file(char *path, int *n){
  FILE *fp = fopen(path, "r");
  float m,px,py,vx,vy;

  fscanf(fp, "%d", n);
  particle_t *p = malloc((*n)*sizeof(particle_t));
  
  for (int i = 0; i <*n; ++i){
    fscanf(fp, "%f %f %f %f %f", &m,&px,&py,&vx,&vy);
    p[i] = init_particle( m,  px,  py,  vx,  vy);
  }
  fclose(fp);
  return p;
}


void write_file(char *path, const int n, particle_t * p){
  FILE *fp = fopen(path, "w");

  fprintf(fp, "%d\n", n);
  
  for (int i = 0; i<n; ++i){
    fprintf(fp, "%f %f %f %f %f\n", p[i].m, p[i].p[0], p[i].p[1], p[i].v[0], p[i].v[1]);
  }
  fclose(fp);
}

void init_universe(const int n, particle_t *p){
  /* TODO */
}
