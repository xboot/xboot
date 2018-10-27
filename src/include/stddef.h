#ifndef __STDDEF_H__
#define __STDDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__cplusplus)
#define NULL		(0)
#else
#define NULL		((void *)0)
#endif

#if (__GNUC__ >= 4)
#define offsetof(type, member)	__builtin_offsetof(type, member)
#else
#define offsetof(type, field)	((size_t)(&((type *)0)->field))
#endif

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define likely(expr)	(__builtin_expect(!!(expr), 1))
#define unlikely(expr)	(__builtin_expect(!!(expr), 0))
#else
#define likely(expr)	(!!(expr))
#define unlikely(expr)	(!!(expr))
#endif

enum {
	FALSE	= 0,
	TRUE	= 1,
};

#ifdef __cplusplus
}
#endif

#endif /* __STDDEF_H__ */
