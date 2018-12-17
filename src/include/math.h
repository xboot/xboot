#ifndef __MATH_H__
#define __MATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <float.h>
#include <endian.h>
#include <limits.h>

typedef float	float_t;
typedef double	double_t;

#define FORCE_EVAL(x) do {									\
	if (sizeof(x) == sizeof(float)) {						\
		volatile float __x __attribute__((unused));			\
		__x = (x);											\
	} else if (sizeof(x) == sizeof(double)) {				\
		volatile double __x __attribute__((unused));		\
		__x = (x);											\
	} else {												\
		volatile long double __x __attribute__((unused));	\
		__x = (x);											\
	}														\
} while(0)

/* Get two 32 bit ints from a double */
#define EXTRACT_WORDS(hi, lo, d)							\
do {														\
	union {double f; uint64_t i;} __u;						\
	__u.f = (d);											\
	(hi) = __u.i >> 32;										\
	(lo) = (uint32_t)__u.i;									\
} while (0)

/* Get the more significant 32 bit int from a double */
#define GET_HIGH_WORD(hi, d)								\
do {														\
	union {double f; uint64_t i;} __u;						\
	__u.f = (d);											\
	(hi) = __u.i >> 32;										\
} while (0)

/* Get the less significant 32 bit int from a double */
#define GET_LOW_WORD(lo, d)									\
do {														\
	union {double f; uint64_t i;} __u;						\
	__u.f = (d);											\
	(lo) = (uint32_t)__u.i;									\
} while (0)

/* Set a double from two 32 bit ints */
#define INSERT_WORDS(d, hi, lo)								\
do {														\
	union {double f; uint64_t i;} __u;						\
	__u.i = ((uint64_t)(hi)<<32) | (uint32_t)(lo);			\
	(d) = __u.f;											\
} while (0)

/* Set the more significant 32 bits of a double from an int */
#define SET_HIGH_WORD(d, hi)								\
do {														\
	union {double f; uint64_t i;} __u;						\
	__u.f = (d);											\
	__u.i &= 0xffffffff;									\
	__u.i |= (uint64_t)(hi) << 32;							\
	(d) = __u.f;											\
} while (0)

/* Set the less significant 32 bits of a double from an int */
#define SET_LOW_WORD(d, lo)									\
do {														\
	union {double f; uint64_t i;} __u;						\
	__u.f = (d);											\
	__u.i &= 0xffffffff00000000ull;							\
	__u.i |= (uint32_t)(lo);								\
	(d) = __u.f;											\
} while (0)

/* Get a 32 bit int from a float */
#define GET_FLOAT_WORD(w, d)								\
do {														\
	union {float f; uint32_t i;} __u;						\
	__u.f = (d);											\
	(w) = __u.i;											\
} while (0)

/* Set a float from a 32 bit int */
#define SET_FLOAT_WORD(d, w)								\
do {														\
	union {float f; uint32_t i;} __u;						\
	__u.i = (w);											\
	(d) = __u.f;											\
} while (0)

#define FP_NAN					0
#define FP_INFINITE				1
#define FP_ZERO					2
#define FP_SUBNORMAL			3
#define FP_NORMAL				4

#define NAN						__builtin_nan("")
#define INFINITY				__builtin_inf()
#define	HUGE_VALF				__builtin_huge_valf()
#define	HUGE_VAL				__builtin_huge_val()
#define	HUGE_VALL				__builtin_huge_vall()

#define	isgreater(x, y)			__builtin_isgreater((x), (y))
#define	isgreaterequal(x, y)	__builtin_isgreaterequal((x), (y))
#define	isless(x, y)			__builtin_isless((x), (y))
#define	islessequal(x, y)		__builtin_islessequal((x), (y))
#define	islessgreater(x, y)		__builtin_islessgreater((x), (y))
#define	isunordered(x, y)		__builtin_isunordered((x), (y))

static __inline unsigned __FLOAT_BITS(float __f)
{
	union {float __f; unsigned __i;} __u;
	__u.__f = __f;
	return __u.__i;
}

static __inline unsigned long long __DOUBLE_BITS(double __f)
{
	union {double __f; unsigned long long __i;} __u;
	__u.__f = __f;
	return __u.__i;
}

#define fpclassify(x) ( \
	sizeof(x) == sizeof(float) ? __fpclassifyf(x) : \
	sizeof(x) == sizeof(double) ? __fpclassify(x) : \
	__fpclassify(x) )

#define isinf(x) ( \
	sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) == 0x7f800000 : \
	sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL>>1) == 0x7ffULL<<52 : \
	__fpclassify(x) == FP_INFINITE)

#define isnan(x) ( \
	sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) > 0x7f800000 : \
	sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL>>1) > 0x7ffULL<<52 : \
	__fpclassify(x) == FP_NAN)

#define isnormal(x) ( \
	sizeof(x) == sizeof(float) ? ((__FLOAT_BITS(x)+0x00800000) & 0x7fffffff) >= 0x01000000 : \
	sizeof(x) == sizeof(double) ? ((__DOUBLE_BITS(x)+(1ULL<<52)) & -1ULL>>1) >= 1ULL<<53 : \
	__fpclassify(x) == FP_NORMAL)

#define isfinite(x) ( \
	sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) < 0x7f800000 : \
	sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL>>1) < 0x7ffULL<<52 : \
	__fpclassify(x) > FP_INFINITE)

#define M_E						2.7182818284590452354	/* e */
#define M_LOG2E					1.4426950408889634074	/* log_2 e */
#define M_LOG10E				0.43429448190325182765	/* log_10 e */
#define M_LN2					0.69314718055994530942	/* log_e 2 */
#define M_LN10					2.30258509299404568402	/* log_e 10 */
#define M_PI					3.14159265358979323846	/* pi */
#define M_PI_2					1.57079632679489661923	/* pi/2 */
#define M_PI_4					0.78539816339744830962	/* pi/4 */
#define M_1_PI					0.31830988618379067154	/* 1/pi */
#define M_2_PI					0.63661977236758134308	/* 2/pi */
#define M_2_SQRTPI				1.12837916709551257390	/* 2/sqrt(pi) */
#define M_SQRT2					1.41421356237309504880	/* sqrt(2) */
#define M_SQRT1_2				0.70710678118654752440	/* 1/sqrt(2) */

double	acos(double);
float	acosf(float);
double	acosh(double);
float	acoshf(float);
double	asin(double);
float	asinf(float);
double	asinh(double);
float	asinhf(float);
double	atan(double);
float	atanf(float);
double	atan2(double, double);
float	atan2f(float, float);
double	atanh(double);
float	atanhf(float);
double	cbrt(double);
float	cbrtf(float);
double	ceil(double);
float	ceilf(float);
double	cos(double);
float	cosf(float);
double	cosh(double);
float	coshf(float);
double	exp(double);
float	expf(float);
double	exp2(double);
float	exp2f(float);
double	expm1(double);
float	expm1f(float);
double	fabs(double);
float	fabsf(float);
double	fdim(double, double);
float	fdimf(float, float);
double	floor(double);
float	floorf(float);
double	fmod(double, double);
float	fmodf(float, float);
double	frexp(double, int *);
float	frexpf(float, int *);
double	hypot(double, double);
float	hypotf(float, float);
double	ldexp(double, int);
float	ldexpf(float, int);
double	log(double);
float	logf(float);
double	log10(double);
float	log10f(float);
double	log1p(double);
float	log1pf(float);
double	log2(double);
float	log2f(float);
double	modf(double, double *);
float	modff(float, float *);
double	pow(double, double);
float	powf(float, float);
double	rint(double);
float	rintf(float);
double	round(double);
float	roundf(float);
double	scalbn(double, int);
float	scalbnf(float, int);
double	scalbln(double, long);
float	scalblnf(float, long);
double	sin(double);
float	sinf(float);
double	sinh(double);
float	sinhf(float);
double	sqrt(double);
float	sqrtf(float);
double	tan(double);
float	tanf(float);
double	tanh(double);
float	tanhf(float);
double	trunc(double);
float	truncf(float);

/*
 * libm kernel functions
 */
double	__cos(double, double);
float	__cosdf(double);
double	__expo2(double);
float	__expo2f(float);
int		__fpclassify(double);
int		__fpclassifyf(float);
int		__rem_pio2_large(double *, double *, int, int, int);
int		__rem_pio2(double, double *);
int		__rem_pio2f(float, double *);
double	__sin(double, double, int);
float	__sindf(double);
double	__tan(double, double, int);
float	__tandf(double, int);

#ifdef __cplusplus
}
#endif

#endif /* __MATH_H__ */
