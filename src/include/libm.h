#ifndef __LIBM_H__
#define __LIBM_H__

#include <stdint.h>
#include <float.h>
#include <math.h>
#include <endian.h>

#if 0
/* fdlibm kernel functions */

int    __rem_pio2_large(double*,double*,int,int,int);

int    __rem_pio2(double,double*);
double __sin(double,double,int);
double __cos(double,double);
double __tan(double,double,int);
double __expo2(double);

int    __rem_pio2f(float,double*);
float  __sindf(double);
float  __cosdf(double);
float  __tandf(double,int);
float  __expo2f(float);

int __rem_pio2l(long double, long double *);
long double __sinl(long double, long double, int);
long double __cosl(long double, long double);
long double __tanl(long double, long double, int);

/* polynomial evaluation */
long double __polevll(long double, const long double *, int);
long double __p1evll(long double, const long double *, int);
#endif


#endif /* __LIBM_H__ */
