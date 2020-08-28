#ifndef __LSORT_H__
#define __LSORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <list.h>

void lsort(void * priv, struct list_head * head, int (*cmp)(void * priv, struct list_head * a, struct list_head * b));

#ifdef __cplusplus
}
#endif

#endif /* __LSORT_H__ */
