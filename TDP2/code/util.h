#ifndef _UTIL_H
#define _UTIL_H
#include "particle.h"

/*
 * out: n
 */
particle_t * load_file(char *path, int *n);
void write_file(char *path, const int n, particle_t * p);

/* Génére n particules aléatoires 
 * in: n
 * out: p
 */
particle_t * init_universe(const int n);


void write_img(char * path, int n, particle_t * p);
#endif
