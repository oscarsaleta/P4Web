#ifndef MATH_NUMERICS_H
#define MATH_NUMERICS_H


void rk78( void (*deriv)( double *, double * ), double y[2], double * hh,
            double hmi, double hma, double e1);
double find_root( double (*f)(double), double (*df)(double), double * value );



#endif // MATH_NUMERICS_H
