#ifndef	__ALLOCA_H__
#define	__ALLOCA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

#define alloca	__builtin_alloca
void * alloca(size_t);

#ifdef __cplusplus
}
#endif

#endif /* __ALLOCA_H__ */
