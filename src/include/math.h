#ifndef __MATH_H__
#define __MATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <endian.h>

#if (BYTE_ORDER == BIG_ENDIAN)
typedef union {
	double value;
	struct {
		u32_t msw;
		u32_t lsw;
	} parts;
} ieee_double_shape_type;
#else
typedef union {
	double value;
	struct {
		u32_t lsw;
		u32_t msw;
	} parts;
} ieee_double_shape_type;
#endif

/*
 * Get two 32 bit ints from a double
 */
#define EXTRACT_WORDS(ix0,ix1,d)			\
do {										\
	ieee_double_shape_type ew_u;			\
	ew_u.value = (d);						\
	(ix0) = ew_u.parts.msw;					\
	(ix1) = ew_u.parts.lsw;					\
} while (0)

/*
 * Get the more significant 32 bit int from a double
 */
#define GET_HIGH_WORD(i,d)					\
do {										\
	ieee_double_shape_type gh_u;			\
	gh_u.value = (d);						\
	(i) = gh_u.parts.msw;					\
} while (0)

/*
 * Get the less significant 32 bit int from a double
 */
#define GET_LOW_WORD(i,d)					\
do {										\
	ieee_double_shape_type gl_u;			\
	gl_u.value = (d);						\
	(i) = gl_u.parts.lsw;					\
} while (0)

/*
 * Set a double from two 32 bit ints
 */
#define INSERT_WORDS(d,ix0,ix1)				\
do {										\
	ieee_double_shape_type iw_u;			\
	iw_u.parts.msw = (ix0);					\
	iw_u.parts.lsw = (ix1);					\
	(d) = iw_u.value;						\
} while (0)

/*
 * Set the more significant 32 bits of a double from an int
 */
#define SET_HIGH_WORD(d,v)					\
do {										\
	ieee_double_shape_type sh_u;			\
	sh_u.value = (d);						\
	sh_u.parts.msw = (v);					\
	(d) = sh_u.value;						\
} while (0)

/*
 * Set the less significant 32 bits of a double from an int
 */
#define SET_LOW_WORD(d,v)					\
do {										\
	ieee_double_shape_type sl_u;			\
	sl_u.value = (d);						\
	sl_u.parts.lsw = (v);					\
	(d) = sl_u.value;						\
} while (0)

/*
 * A union which permits us to convert between a float and a 32 bit int
 */
typedef union {
	float value;
	u32_t word;
} ieee_float_shape_type;

/*
 * Get a 32 bit int from a float
 */
#define GET_FLOAT_WORD(i,d)					\
do {										\
	ieee_float_shape_type gf_u;				\
	gf_u.value = (d);						\
	(i) = gf_u.word;						\
} while (0)

/*
 * Set a float from a 32 bit int
 */
#define SET_FLOAT_WORD(d,i)					\
do {										\
	ieee_float_shape_type sf_u;				\
	sf_u.word = (i);						\
	(d) = sf_u.value;						\
} while (0)

#define	M_E			2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* log 2e */
#define	M_LOG10E	0.43429448190325182765	/* log 10e */
#define	M_LN2		0.69314718055994530942	/* log e2 */
#define	M_LN10		2.30258509299404568402	/* log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

#define	HUGE_VAL	__builtin_huge_val()

/*
 * fdlibm kernel function
 */
s32_t __ieee754_rem_pio2(double x, double *y);
int __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec, const s32_t *ipio2);
double __kernel_sin(double x, double y, int iy);
double __kernel_cos(double x, double y);
double __kernel_tan(double x, double y, int iy);

/*
 * float versions of fdlibm kernel functions
 */
s32_t __ieee754_rem_pio2f(float x, float *y);
int __kernel_rem_pio2f(float *x, float *y, int e0, int nx, int prec, const s32_t *ipio2);
float __kernel_sindf(double x);
float __kernel_cosdf(double x);
float __kernel_tandf(double x, int iy);


double copysign(double x, double y);
float copysignf(float x, float y);

double scalbn(double x, int n);
double ldexp(double x, int n);

float scalbnf(float x, int n);
float ldexpf(float x, int n);

double expm1(double x);
float expm1f(float x);


double fabs(double x);
float fabsf(float x);
double ceil(double x);
float ceilf(float x);
double floor(double x);
float floorf(float x);
double modf(double x, double *iptr);
float modff(float x, float *iptr);
double frexp(double x, int *eptr);
float frexpf(float x, int *eptr);

double sqrt(double x);
float sqrtf(float x);
double exp(double x);
float expf(float x);
double fmod(double x, double y);
float fmodf(float x, float y);
double pow(double x, double y);
float powf(float x, float y);
double log(double x);
float logf(float x);
double log10(double x);
float log10f(float x);
double hypot(double x, double y);
float hypotf(float x, float y);

double sin(double x);
float sinf(float x);
double cos(double x);
float cosf(float x);
double tan(double x);
float tanf(float x);

double sinh(double x);
float sinhf(float x);
double cosh(double x);
float coshf(float x);
double tanh(double x);
float tanhf(float x);

double asin(double x);
float asinf(float x);
double acos(double x);
float acosf(float x);
double atan(double x);
float atanf(float x);
double atan2(double y, double x);
float atan2f(float y, float x);

#ifdef __cplusplus
}
#endif

#endif /* __MATH_H__ */
