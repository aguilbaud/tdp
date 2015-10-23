#include <stdlib.h>
#include <math.h>
#include "particle.h"

#define G 6.67E-11

particle_t init_particle(float m, float px, float py, float vx, float vy){
  particle_t par;
  
  par.m = m;
  par.p[0] = px;
  par.p[1] = py;
  par.v[0] = vx;
  par.v[1] = vy;
  
  return par;
}

double distance(double x1, double y1, double x2, double y2)
{
  return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}


