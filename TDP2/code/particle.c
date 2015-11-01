#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include "particle.h"
#include "dt_calc.h"

#define CLOSE2(a,b,e) (fabs(a-b)<e)

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

void forces(particle_t *p, const int n){
  double * acc = malloc(n*2*sizeof(double));
  int acc_id = 0;
  double min_dt = 100000;

  for (int i = 0; i < n; i++) {
    double f_x = 0.0;
    double f_y = 0.0;
    double min_dist = DBL_MAX;
    if(p[i].m != 0.0){
      for (int j = 0; j < n; j++) {
	if(i!=j && p[j].m!=0.0){
	  double dist = distance(p[i].p[0], p[i].p[1], p[j].p[0], p[j].p[1]);

	  if(!CLOSE2(dist,0.0,2E8)){/*ATENTION*/
	      double intens = intensity(p[i].m, p[j].m, dist);
	      double vec_unit_x = (p[j].p[0] - p[i].p[0]) / dist;
	      double vec_unit_y = (p[j].p[1] - p[i].p[1]) / dist;
	    
	      f_x += (intens * vec_unit_x);
	      f_y += (intens * vec_unit_y);
	    
	      if(dist < min_dist)
		  min_dist = dist;
	  }
	  else{//fusion
	      p[i].m+=p[j].m;
	      p[j].m=0.0;
	  }
	}
      } 
      
      //acc array contains acceleration vector for each particle
      double a_x = f_x / p[i].m;
      double a_y = f_y / p[i].m;
      acc[acc_id] = a_x;
      acc[acc_id + 1] = a_y;
      acc_id+=2;
      
      //Polynomial resolution to compute a fittable dt
      double a = sqrt( (a_x * a_x) + (a_y * a_y) ) / 2.0;
      double b = sqrt( (p[i].v[0] * p[i].v[0]) + (p[i].v[1] * p[i].v[1]) );
      double c = -0.01 * min_dist; 
      double dt = polynomial_solver(a,b,c);

      if(dt<min_dt)
	  min_dt = dt;
    }
  }

  double dt = min_dt;
  
  //printf("%40.40f\n", dt);
  //Mise a jour de p,v,a
  acc_id = 0;
  for (int i = 0; i < n; i++) {
      p[i].v[0] += (acc[acc_id] * dt); 
      p[i].v[1] += (acc[acc_id+1] * dt);

      p[i].p[0] += (p[i].v[0] * dt) + ((acc[acc_id]/2) *dt *dt);
      p[i].p[1] += (p[i].v[1] * dt) + ((acc[acc_id+1]/2) *dt *dt);

      acc_id+=2;
  }
  free(acc);
}

void move(particle_t *p, const int n){
    //print_particle(p[1]);
    forces(p,n);
}

