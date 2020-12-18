#ifndef __MATH_H__
#define __MATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <float.h>
#include <endian.h>
#include <limits.h>

typedef float					float_t;
typedef double					double_t;

#define FORCE_EVAL(x) do { \
	if (sizeof(x) == sizeof(float)) { \
		volatile float __x __attribute__((unused)); \
		__x = (x); \
	} else if (sizeof(x) == sizeof(double)) { \
		volatile double __x __attribute__((unused)); \
		__x = (x); \
	} else { \
		volatile long double __x __attribute__((unused)); \
		__x = (x); \
	} \
} while(0)

#define asuint(f)				((union {float _f; uint32_t _i;}){f})._i
#define asfloat(i)				((union {uint32_t _i; float _f;}){i})._f
#define asuint64(f)				((union {double _f; uint64_t _i;}){f})._i
#define asdouble(i)				((union {uint64_t _i; double _f;}){i})._f

#define EXTRACT_WORDS(hi, lo, d) \
do { \
	uint64_t __u = asuint64(d); \
	(hi) = __u >> 32; \
	(lo) = (uint32_t)__u; \
} while(0)

#define GET_HIGH_WORD(hi, d) \
do { \
	(hi) = asuint64(d) >> 32; \
} while(0)

#define GET_LOW_WORD(lo, d) \
do { \
	(lo) = (uint32_t)asuint64(d); \
} while(0)

#define INSERT_WORDS(d, hi, lo) \
do { \
	(d) = asdouble(((uint64_t)(hi) << 32) | (uint32_t)(lo)); \
} while(0)

#define SET_HIGH_WORD(d, hi) \
	INSERT_WORDS(d, hi, (uint32_t)asuint64(d))

#define SET_LOW_WORD(d, lo) \
	INSERT_WORDS(d, asuint64(d) >> 32, lo)

#define GET_FLOAT_WORD(w, d) \
do { \
	(w) = asuint(d); \
} while(0)

#define SET_FLOAT_WORD(d, w) \
do { \
	(d) = asfloat(w); \
} while(0)

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

#define predict_true(x)			__builtin_expect(!!(x), 1)
#define predict_false(x)		__builtin_expect(x, 0)

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
	sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL >> 1) == 0x7ffULL << 52 : \
	__fpclassify(x) == FP_INFINITE)

#define isnan(x) ( \
	sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) > 0x7f800000 : \
	sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL >> 1) > 0x7ffULL << 52 : \
	__fpclassify(x) == FP_NAN)

#define isnormal(x) ( \
	sizeof(x) == sizeof(float) ? ((__FLOAT_BITS(x) + 0x00800000) & 0x7fffffff) >= 0x01000000 : \
	sizeof(x) == sizeof(double) ? ((__DOUBLE_BITS(x) + (1ULL << 52)) & -1ULL >> 1) >= 1ULL << 53 : \
	__fpclassify(x) == FP_NORMAL)

#define isfinite(x) ( \
	sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) < 0x7f800000 : \
	sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL >> 1) < 0x7ffULL << 52 : \
	__fpclassify(x) > FP_INFINITE)

#define signbit(x) ( \
	sizeof(x) == sizeof(float) ? (int)(__FLOAT_BITS(x) >> 31) : \
	sizeof(x) == sizeof(double) ? (int)(__DOUBLE_BITS(x) >> 63) : \
	(int)(__DOUBLE_BITS(x) >> 63))

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
double	erf(double x);
float	erff(float x);
double	erfc(double x);
float	erfcf(float x);
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
double	fmax(double x, double y);
float	fmaxf(float x, float y);
double	fmin(double x, double y);
float	fminf(float x, float y);
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
double	__expo2(double, double);
float	__expo2f(float, float);
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
