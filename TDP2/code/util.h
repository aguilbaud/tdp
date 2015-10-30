#ifndef _UTIL_H
#define _UTIL_H
#include "particle.h"


void print_particle(particle_t p);
/*
 * out: n
 */
particle_t * load_file(FILE * f, int *n);
void write_file(FILE * f, const int n, particle_t * p);

/* Génére n particules aléatoires 
 * in: n
 * out: p
 */
particle_t * init_universe(const int n);


void write_img(char * path, int n, particle_t * p);
void write_img_1bpp(char * path, int n, particle_t * p);
void write_plot(char * path, int n, particle_t * p);
#endif
