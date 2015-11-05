#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void print_particle(particle_t p){
    printf("Mass: %10.f,p.x: %f, p.y %f, p.vx:%f, p.vy:%f\n",p.m, p.p[0], p.p[1], p.v[0], p.v[1]);
}


particle_t * load_file(FILE * fp, int *n){
    double m,px,py,vx,vy;
    
    if(fscanf(fp, "%d", n) != 1){
	fprintf(stderr, "Cannot read input file\n");
	return NULL;
    }
    
    particle_t *p = malloc((*n)*sizeof(particle_t));

    for (int i = 0; i < *n; ++i){
	if(fscanf(fp, "%lf %lf %lf %lf %lf", &m,&px,&py,&vx,&vy) != 5){
	    fprintf(stderr, "Cannot read input file.\n");
	    free(p);
	    return NULL;
	}
	p[i] = init_particle( m,  px,  py,  vx,  vy);
    }

    return p;
}


void write_file(FILE * fp, const int n, particle_t * p){
    fprintf(fp, "%d\n", n);
    
	for (int i = 0; i<n; ++i)
	    fprintf(fp, "%lf %lf %lf %lf %lf\n", p[i].m, p[i].p[0], p[i].p[1], p[i].v[0], p[i].v[1]);
    }

    particle_t * init_universe(const int n){
	particle_t * p = malloc(n*sizeof(particle_t));
    
	for (int i = 0; i < n; i++) 
	    p[i] = init_particle(( rand()/(double)RAND_MAX ) * (MAX_MASS-MIN_MASS) + MIN_MASS,
				 ( rand()/(double)RAND_MAX ) * (MAX_XY-MIN_XY) + MIN_XY,
				 ( rand()/(double)RAND_MAX ) * (MAX_XY-MIN_XY) + MIN_XY,
				 ( rand()/(double)RAND_MAX ) * (MAX_V-MIN_V) + MIN_V,
				 ( rand()/(double)RAND_MAX ) * (MAX_V-MIN_V) + MIN_V);
	return p;
    }


    void write_plot(FILE *fp, int n, particle_t * p){
	for (int i = 0; i < n; i++) {
	    if(p[i].m!=0.0 )
		fprintf(fp, "%lf %lf %lf\n", p[i].p[0], p[i].p[1], p[i].m); 
	    else{
		printf("%f\n",p[i].m);
		fprintf(fp, "\n");
	    }
	}
	fprintf(fp, "\n\n");
    }


