#include <float.h>
#include <math.h>
#include <stdio.h>
#include "dt_calc.h"

//#define DEBUG_DT_CALC

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define CLOSE(a,b,e) (fabs(a-b)<e)

double polynomial_solver(double a, double b, double c){
  if(CLOSE(a,0.0,1E-6)){
#ifdef DEBUG_DT_CALC
    printf("[dt_calc]a=0 (%20.20f)\n", a);
#endif // DEBUG_DT_CALC
    
    return -c/b; //-c and b are always positives
  }  
  double delta = b*b - (4.0*a*c);

  if(delta<=0){//donc P touujours positif
#ifdef DEBUG_DT_CALC
    printf("[dt_calc]delta < 0 (%20.20f)\n", delta);
#endif // DEBUG_DT_CALC

    return DBL_MAX;
  }
  
  double dt1 = (-b - sqrt(delta)) / 2.0*a;
  double dt2 = (-b + sqrt(delta)) / 2.0*a;
  
  if(dt1 > 0 && dt2 > 0){
#ifdef DEBUG_DT_CALC
    printf("[dt_calc]dt1 & dt2 > 0 (%20.20f)(%20.20f)\n",dt1,dt2);
#endif // DEBUG_DT_CALC
    return MIN(dt1,dt2);
  }
  
  if(dt1 <= 0 && dt2 <= 0){
#ifdef DEBUG_DT_CALC
    printf("[dt_calc]dt1 & dt2 <= 0 (%20.20f)(%20.20f)\n", dt1, dt2);    
#endif // DEBUG_DT_CALC

    return DBL_MAX;
  }
#ifdef DEBUG_DT_CALC
  printf("[dt_calc]default (%20.20f)(%20.20f)\n", dt1, dt2);  
#endif // DEBUG_DT_CALC
  
  return MAX(dt1,dt2);
}
