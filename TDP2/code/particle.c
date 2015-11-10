#include <stdlib.h>
#include <float.h>
#include "particle.h"
#include "dt_calc.h"

extern inline double distance(double x1, double y1, double x2, double y2);
extern inline double intensity(double m1, double m2, double d);

particle_t init_particle(double m, double px, double py, double vx, double vy){
particle_t par;
  
 par.m = m;
 par.p[0] = px;
 par.p[1] = py;
 par.v[0] = vx;
 par.v[1] = vy;

 return par;
}

void forces(particle_t *p1, const int n1, particle_t *p2, const int n2, double *acc,  double*min_dt, int SAME_SET_F){
    
    int acc_id = 0;
   
    for (int i = 0; i < n1; i++) {
	double f_x = 0.0;
	double f_y = 0.0;
	double min_dist = DBL_MAX;
	for (int j = 0; j < n2; j++) {
	    if((i!=j  || !SAME_SET_F) && p2[j].m!=0.0){
		double dist = distance(p1[i].p[0], p1[i].p[1], p2[j].p[0], p2[j].p[1]);
		double intens = intensity(p1[i].m, p2[j].m, dist);
		double vec_unit_x = (p2[j].p[0] - p1[i].p[0]) / dist;
		double vec_unit_y = (p2[j].p[1] - p1[i].p[1]) / dist;
		    
		f_x += (intens * vec_unit_x);
		f_y += (intens * vec_unit_y);

		if(dist < min_dist)
		    min_dist = dist;
	    }
	}
	 
      
	//acc array contains acceleration vector for each particle
	double a_x = f_x / p1[i].m;
	double a_y = f_y / p1[i].m;
	acc[acc_id] += a_x;
	acc[acc_id + 1] += a_y;
	acc_id+=2;

	//Polynomial resolution to compute a fittable dt
	double a = sqrt( (a_x * a_x) + (a_y * a_y) ) / 2.0;
	double b = sqrt( (p1[i].v[0] * p1[i].v[0]) + (p1[i].v[1] * p1[i].v[1]) );
	double c = -0.01 * min_dist; 
	double dt = polynomial_solver(a,b,c);

	if(dt<*min_dt)
	    *min_dt = dt;
	
    }
}


void update_pos_vel(particle_t *p, int n, double *acc, int dt){
    int acc_id = 0;
	for (int i = 0; i < n; i++) {
	    p[i].v[0] += (acc[acc_id] * dt); 
	    p[i].v[1] += (acc[acc_id+1] * dt);
	
	    p[i].p[0] += (p[i].v[0] * dt) + ((acc[acc_id]/2) *dt *dt);
	    p[i].p[1] += (p[i].v[1] * dt) + ((acc[acc_id+1]/2) *dt *dt);

	    acc_id+=2;
	}
    }


    void move(particle_t *p, const int n, const int def_dt){
	double *acc = calloc(2*n,sizeof(double));
	double dt = def_dt;
	forces(p , n, p, n, acc, &dt, 1);
	update_pos_vel(p , n, acc, dt);	
	free(acc);
    }

