#ifndef _UTIL_H
#define _UTIL_H
#include "particle.h"


void print_particle(particle_t p);

/*
 * IN f:  a file containing a description of the universe
 * OUT n: n is set with the number of particles
 */
particle_t * load_file(FILE * f, int *n);

/*
 * IN f: a file where the descritpion of each particles wwwill be write
 * IN n: the size of the particle set
 * IN p: the particle set (an array of particle_t)
 */
void write_file(FILE * f, const int n, particle_t * p);

/* 
 * IN n:  the size of the universe to create
 * OUT p: the particle set initialized with random particles (ie. random mass, position & velocity)
 */
particle_t * init_universe(const int n);

/* Desc: create a file which contains a description of the particle set (x_pos y_pos mass)
 * (Useful to create an animation of the simulation
 * IN fp: the file that will contain the plot
 * IN n:  the size of the particle set
 * IN p:  the particle set
 */
void write_plot(FILE * fp, int n, particle_t * p);

/* Note: not used
 * IN path: the path of the picture to create
 * IN n:    the size of the particle set
 * IN p:    the particle set
 */
void write_img_1bpp(char * path, int n, particle_t * p);


#endif
