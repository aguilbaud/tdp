#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#define MAX_V 3E-4
#define MIN_V 3E-5
#define MAX_MASS 1E30
#define MIN_MASS 1E10
#define MAX_XY 12E3
#define MIN_XY 0
#define IMG_SIZE 800 //Must be a multiple of 4


void print_particle(particle_t p){
  printf("Mass: %10.f,p.x: %f, p.y %f, p.vx:%f, p.vy:%f\n",p.m, p.p[0], p.p[1], p.v[0], p.v[1]);
}

particle_t * load_file(FILE * fp, int *n){
  float m,px,py,vx,vy;
  fscanf(fp, "%d", n);
  particle_t *p = malloc((*n)*sizeof(particle_t));
  
  for (int i = 0; i <*n; ++i){
    fscanf(fp, "%f %f %f %f %f", &m,&px,&py,&vx,&vy);
    p[i] = init_particle( m,  px,  py,  vx,  vy);
  }
  return p;
}


void write_file(FILE * fp, const int n, particle_t * p){
  fprintf(fp, "%d\n", n);
  
  for (int i = 0; i<n; ++i){
    fprintf(fp, "%f %f %f %f %f\n", p[i].m, p[i].p[0], p[i].p[1], p[i].v[0], p[i].v[1]);
  }
}


particle_t * init_universe(const int n){
  particle_t * p = malloc(n*sizeof(particle_t));
  
  for (int i = 0; i < n; i++) {
    p[i] = init_particle(( rand()/(double)RAND_MAX ) * (MAX_MASS-MIN_MASS) + MIN_MASS,
			 ( rand()/(double)RAND_MAX ) * (MAX_XY-MIN_XY) + MIN_XY,
			 ( rand()/(double)RAND_MAX ) * (MAX_XY-MIN_XY) + MIN_XY,
			 /* ( rand()/(double)RAND_MAX ) * (MAX_V-MIN_V) + MIN_V, */
			 /* ( rand()/(double)RAND_MAX ) * (MAX_V-MIN_V) + MIN_V); */
			 0, 0);
   
  }
  return p;
}


/********************************************************************/

void write_img(char * path, int n, particle_t * p){
  FILE *fp = fopen(path, "wb");
  int offset = 14 + 40  ;
  int header_size = 40;
  int img_width = IMG_SIZE;
  int img_height = IMG_SIZE;
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
    double ratio = IMG_SIZE / MAX_XY;
    int x = p[i].p[0];
    int y = p[i].p[1];
    if( (x >= 0 && x <MAX_XY) && (y >= 0 && y < MAX_XY)){
      x *= ratio;
      y *= ratio;
     
      int pos =offset +  ((y*3) * img_width +(x*3));
    
      fseek(fp, pos, SEEK_SET);
      fwrite(&blanc, 1, 1, fp);
      fwrite(&blanc, 1, 1, fp);
      fwrite(&blanc, 1, 1, fp);
    }
    else{
      printf("Particle is outside of picture.\n");
    }
  }
  fclose(fp);
}

/* TODO: Faire tout ce bouzin dans un buffer */
void write_img_1bpp(char * path, int n, particle_t * p){

  
  int offset = 14 + 40 + 8;
  int header_size = 40;
  int img_width = IMG_SIZE;
  int img_height = IMG_SIZE;
  int nb_planes = 1;
  int bpp = 1;
  int compression = 0;
  int img_size = img_height * img_width / 8;
  int size = offset + img_size;
  char blanc = 0x00;
  char noir = 0xff;


  char * img_array = calloc(size,1);
  int array_cpt = 2;
  /*HEADER*/
  img_array[0]='B';
  img_array[1]='M';
  memcpy(img_array+array_cpt, &size, 4);array_cpt+=4;
  memcpy(img_array+array_cpt, "\0\0\0\0", 4);array_cpt+=4;
  memcpy(img_array+array_cpt, &offset, 4);array_cpt+=4;

  /* /\*INFO HEADER*\/ */
  memcpy(img_array+array_cpt, &header_size, 4);array_cpt+=4;
  memcpy(img_array+array_cpt, &img_width, 4);array_cpt+=4;
  memcpy(img_array+array_cpt, &img_height, 4);array_cpt+=4;
  memcpy(img_array+array_cpt, &nb_planes, 2);array_cpt+=2;
  memcpy(img_array+array_cpt, &bpp, 2);array_cpt+=2;
  memcpy(img_array+array_cpt, &compression, 4);array_cpt+=4;
  memcpy(img_array+array_cpt, "\0\0\0\0", 4);array_cpt+=4;
  memcpy(img_array+array_cpt, "\0\0\0\0", 4);array_cpt+=4;
  memcpy(img_array+array_cpt, "\0\0\0\0", 4);array_cpt+=4;
  memcpy(img_array+array_cpt, "\0\0\0\0", 4);array_cpt+=4;
  memcpy(img_array+array_cpt, "\0\0\0\0", 4);array_cpt+=4;

  /* /\*PALETTE*\/ */
  memcpy(img_array+array_cpt, &blanc, 1);array_cpt+=1;
  memcpy(img_array+array_cpt, &blanc, 1);array_cpt+=1;
  memcpy(img_array+array_cpt, &blanc, 1);array_cpt+=1;
  memcpy(img_array+array_cpt, &noir, 1);array_cpt+=1;

  memcpy(img_array+array_cpt, &noir, 1);array_cpt+=1;
  memcpy(img_array+array_cpt, &noir, 1);array_cpt+=1;
  memcpy(img_array+array_cpt, &noir, 1);array_cpt+=1;
  memcpy(img_array+array_cpt, &noir, 1);array_cpt+=1;

  /* /\*DATA*\/ */
      
  for (int i = 0; i < n; i++) {
    double ratio = IMG_SIZE  / MAX_XY ;
    
    int x = p[i].p[0];
    int y = p[i].p[1];
    if( (x >= 0 && x <MAX_XY) && (y >= 0 && y < MAX_XY)){
      x *= ratio;
	y *= ratio;
     
	int octet = offset +  ((y) * img_width +(x)) / 8;
	int bit = ((y) * img_width +(x)) % 8;
	printf("px :%d, py: %d, octet:%d, bit:%d\n",x, y, octet, bit);
	char mask = 0x01 << (7 - bit);
	img_array[octet] |= mask ;
      }
      else{
	printf("Particle is outside of picture.\n");
      }
    }
  
    FILE *fp = fopen(path, "w+");
    fwrite(img_array, 1, size,fp);
    free(img_array);
    fclose(fp);
  }
