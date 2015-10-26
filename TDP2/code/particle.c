#include <stdlib.h>
#include <stdio.h>
#include "particle.h"

extern inline double distance(double x1, double y1, double x2, double y2);
extern inline double intensity(particle_t p1, particle_t p2);

particle_t init_particle(float m, float px, float py, float vx, float vy){
  particle_t par;
  
  par.m = m;
  par.p[0] = px;
  par.p[1] = py;
  par.v[0] = vx;
  par.v[1] = vy;
  
  return par;
}

void forces(particle_t *p, const int n){
  double * acc = malloc(n*2*sizeof(double));
  int acc_id = 0;

  for (int i = 0; i < n; i++) {
    double f_x = 0.0;
    double f_y = 0.0;
    for (int j = 0; j < n; j++) {
      if(i!=j){
	double intens = intensity(p[i], p[j]);
	double norme = distance(p[i].p[0], p[i].p[1], p[j].p[0], p[j].p[1]);
	
	f_x += intens *  (p[j].p[0] - p[i].p[0]) / norme;
	f_y += intens *  (p[j].p[1] - p[i].p[1]) / norme;	
      }

    }
    double a_x = f_x / p[i].m;
    double a_y = f_y / p[i].m;
    acc[acc_id] = a_x;
    acc[acc_id + 1] = a_y;
    acc_id+=2;
  }

  //Calcul dt;
  double dt = 2000;
  
  acc_id = 0;
  for (int i = 0; i < n; i++) {
    p[i].v[0] += acc[acc_id] * dt; 
    p[i].v[1] += acc[acc_id+1] * dt;

    p[i].p[0] = p[i].p[0] + p[i].v[0] * dt + (acc[acc_id]/2) *dt *dt;
    p[i].p[1] = p[i].p[1] + p[i].v[1] * dt + (acc[acc_id+1]/2) *dt *dt;

    print_particle(p[i]);
    
    acc_id+=2;
    
  }
  free(acc);
}

void move(particle_t *p, const int n){
  
  forces(p,n);
  /* for (int i = 0; i < n; i++) { */
  /*   p[i].p[0] += 0.5; */
  /*   p[i].p[1] += 0.5; */
  /* } */
}

