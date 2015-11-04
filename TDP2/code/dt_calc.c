#include <float.h>
#include <math.h>
#include "dt_calc.h"

//#define DEBUG_DT_CALC

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define CLOSE(a,b,e) (fabs(a-b)<e)

double polynomial_solver(double a, double b, double c){
    if(CLOSE(a,0.0,1E-6)){
	return -c/b; //-c and b are always positives
    }  

    double delta = b*b - (4.0*a*c);

    if(delta<=0)//donc P toujours positif
	return DBL_MAX;

    double dt1 = (-b - sqrt(delta)) / 2.0*a;
    double dt2 = (-b + sqrt(delta)) / 2.0*a;
  
    if(dt1 > 0 && dt2 > 0)
	return MIN(dt1,dt2);
  
    if(dt1 <= 0 && dt2 <= 0)
	return DBL_MAX;
  
    return MAX(dt1,dt2);
}
