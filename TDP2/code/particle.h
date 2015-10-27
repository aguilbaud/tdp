#include <math.h>
#define G 6.67E-11

#ifndef _PARTICLE_H
#define _PARTICLE_H



typedef struct particle_s{
  float m;
  float p[2];
  float v[2];
}particle_t;

particle_t init_particle(float m, float px, float py, float vx, float vy);

inline double distance(double x1, double y1, double x2, double y2){
  return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

inline double intensity(particle_t p1, particle_t p2){
  double dist = distance(p1.p[0], p1.p[1], p2.p[0], p2.p[1]);
  //  printf("%0.20f, %f, %f,  %f \n",G, p1.m, p2.m, dist);

  //return G * ((p1.m * p2.m) / (dist * dist)) ;
  //Trick to avoid infinite numbers or nan with large masses
  return ((G*p1.m) / (dist * dist))  +  ((G*p2.m) / (dist * dist));
}

void forces(particle_t *p, const int n);
void move(particle_t *p, const int n);
#endif
