#ifndef __ARM32_LIMITS_H__
#define __ARM32_LIMITS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Number of bits in a 'char' */
#define CHAR_BIT		(8)

/* Minimum and maximum values a 'signed char' can hold */
#define SCHAR_MIN		(-128)
#define SCHAR_MAX		127

/* Minimum and maximum values a 'char' can hold */
#define CHAR_MIN		(-128)
#define CHAR_MAX		127

/* Maximum value an 'unsigned char' can hold (Minimum is 0) */
#define UCHAR_MAX		255

/* Minimum and maximum values a 'signed short int' can hold */
#define SHRT_MIN		(-1 - 0x7fff)
#define SHRT_MAX		0x7fff

/* Maximum value an 'unsigned short int' can hold (Minimum is 0) */
#define USHRT_MAX		0xffff

/* Minimum and maximum values a 'signed int' can hold */
#define INT_MIN			(-1 - 0x7fffffff)
#define INT_MAX			0x7fffffff

/* Maximum value an 'unsigned int' can hold (Minimum is 0) */
#define UINT_MAX		0xffffffffU

/* Minimum and maximum values a 'signed long int' can hold */
#define LONG_MIN		(-LONG_MAX - 1)
#define LONG_MAX		0x7fffffffL

/* Maximum value an 'unsigned long int' can hold (Minimum is 0) */
#define ULONG_MAX		(2UL * LONG_MAX + 1)

/* Minimum and maximum values a 'signed long long int' can hold */
#define LLONG_MIN		(-LLONG_MAX - 1)
#define LLONG_MAX		0x7fffffffffffffffLL

/* Maximum value an 'unsigned long long int' can hold (Minimum is 0) */
#define ULLONG_MAX		(2ULL * LLONG_MAX + 1)

/* Minimum and maximum values a 'max int' can hold */
#define INTMAX_MIN		LLONG_MIN
#define INTMAX_MAX		LLONG_MAX

/* Maximum value an 'max uint' can hold (Minimum is 0) */
#define UINTMAX_MAX		ULLONG_MAX

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_LIMITS_H__ */
