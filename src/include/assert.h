#ifndef __ASSERT_H__
#define __ASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#define	assert(x)	((void)0)
#else
#define assert(x)	((void)((x) || (__assert_fail(#x, __FILE__, __LINE__, __func__), 0)))
#endif

void __assert_fail(const char * expr, const char * file, int line, const char * func);

#ifdef __cplusplus
}
#endif

#endif /* __ASSERT_H__ */
