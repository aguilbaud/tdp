#include <math.h>
#define G 6.67E-11

#ifndef _PARTICLE_H
#define _PARTICLE_H

/*
 * Definition of a particle
 */
typedef struct particle_s{
    double m;
    double p[2];
    double v[2];
}particle_t;

/*
 * Create and return a particle created with the given arguments
 */
particle_t init_particle(double m, double px, double py, double vx, double vy);


inline double distance(double x1, double y1, double x2, double y2){
    return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

inline double intensity(double m1, double m2, double d){
  return G * ((m1 * m2) / (d * d)) ;
}

/* Desc: computes the forces between two particle sets (p1 & p2),
 * store the computed accelerations of the particles within p1 in acc,
 * return the minimal dt value that will avoid particle collision.
 */
void forces(particle_t *p1, const int n1, particle_t *p2, const int n2, double *acc, double *min_dt, int SAME_SET_F);


/* Desc: update the positions and velocities of the particles thanks to acc and dt
 */
void update_pos_vel(particle_t *p, int n, double *acc, int dt);

/* only used in sequential, this function is a wrapper for move() + update_pos_vel()
 */
void move(particle_t *p, const int n, const int def_dt);


#endif
