#include <math.h>
#define G 6.67E-11

#ifndef _PARTICLE_H
#define _PARTICLE_H



typedef struct particle_s{
    double m;
    double p[2];
    double v[2];
}particle_t;

particle_t init_particle(double m, double px, double py, double vx, double vy);

inline double distance(double x1, double y1, double x2, double y2){
    return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

inline double intensity(double m1, double m2, double d){
  return G * ((m1 * m2) / (d * d)) ;
}

void forces2(particle_t *p1, const int n1, particle_t *p2, const int n2, double *acc, double *min_dt, int SAME_SET_F);
void update_pos_vel(particle_t *p, int n, double *acc, int dt);
void forces(particle_t *p, const int n, double *acc, double *min_dt);
void move(particle_t *p, const int n);
#endif
