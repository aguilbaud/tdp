#ifndef DT_CALC_H
#define DT_CALC_H

/* Desc: Resolve a quadratic equation (ie. a*x*x + bx +c = 0) and return the 
 * minimun positive solution. If there is no positive solution, DBL_MAX is returned.
 */
double polynomial_solver(double a, double b, double c);

#endif /* DT_CALC_H */
