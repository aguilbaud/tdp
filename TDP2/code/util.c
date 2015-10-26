#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define MAX_MASS 1E4
#define MIN_MASS 1E3
#define MAX_XY 1024 //Must be a multiple of 4
#define MIN_XY 0


void print_particle(particle_t p){
  printf("Mass: %10.f,p.x: %f, p.y %f, p.vx:%f, p.vy:%f\n",p.m, p.p[0], p.p[1], p.v[0], p.v[1]);
}

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


particle_t * init_universe(const int n){
  particle_t * p = malloc(n*sizeof(particle_t));
  
  for (int i = 0; i < n; i++) {
    p[i] = init_particle(( rand()/(double)RAND_MAX ) * (MAX_MASS-MIN_MASS) + MIN_MASS,
			 ( rand()/(double)RAND_MAX ) * (MAX_XY-MIN_XY) + MIN_XY,
			 ( rand()/(double)RAND_MAX ) * (MAX_XY-MIN_XY) + MIN_XY,
			 0, 0);
    
    
  }
  return p;
}


/********************************************************************/
void write_img(char * path, int n, particle_t * p){
  FILE *fp = fopen(path, "wb");
  int offset = 14 + 40  ;
  int header_size = 40;
  int img_width = MAX_XY;
  int img_height = MAX_XY;
  int nb_planes = 1;
  int bpp = 24;
  int compression = 0;
  int img_size = img_height * img_width;
  int size = offset + img_size;
 
  /*HEADER*/
  fwrite("BM", 1, 2, fp);
  fwrite(&size, 1, 4, fp);
  fwrite("\0\0\0\0", 1, 4, fp);
  fwrite(&offset,1,4,fp);//offset

  /*INFO HEADER*/
  fwrite(&header_size, 1, 4, fp); 
  fwrite(&img_width, 1, 4, fp);
  fwrite(&img_height, 1, 4, fp);
  fwrite(&nb_planes, 1, 2, fp);
  fwrite(&bpp, 1, 2, fp);
  fwrite(&compression, 1, 4, fp);
  fwrite("\0\0\0\0", 1, 4, fp);
  fwrite("\0\0\0\0\0\0\0\0", 1, 8, fp);
  fwrite ("\0\0\0\0", 1, 4, fp);
  fwrite ("\0\0\0\0", 1, 4, fp);

  /*DATA*/
  char blanc = 0xff;
  char noir = 0x00;
  for (int i = 0; i < img_size; i++) {
    fwrite(&noir,1,1,fp);
    fwrite(&noir,1,1,fp);
    fwrite(&noir,1,1,fp);
  }
  
  for (int i = 0; i < n; i++) {
    int x = p[i].p[0] ;//* (img_width / MAX_XY); 
    int y = p[i].p[1] ;//* (img_height / MAX_XY);
    int pos =offset +  ((y*3) * img_width +(x*3));
    
    fseek(fp, pos, SEEK_SET);
    fwrite(&blanc, 1, 1, fp);
    fwrite(&blanc, 1, 1, fp);
    fwrite(&blanc, 1, 1, fp);
  }
  
  fclose(fp);
}
