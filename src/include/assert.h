#ifndef __ASSERT_H__
#define __ASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
#define	assert(x)	((x) ? (void)0 : __assert_fail(__FILE__, __LINE__, __func__, #x))
#else
#define	assert(x)	((void)0)
#endif

void __assert_fail(const char * file, int line, const char * func, const char * expr);

#ifdef __cplusplus
}
#endif

#endif /* __ASSERT_H__ */
