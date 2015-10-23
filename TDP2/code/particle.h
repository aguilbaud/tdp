#ifndef _PARTICLE_H
#define _PARTICLE_H

typedef struct particle_s{
  float m;
  float p[2];
  float v[2];
}particle_t;

particle_t init_particle(float m, float px, float py, float vx, float vy);

double distance(double x1, double y1, double x2, double y2);

#endif
