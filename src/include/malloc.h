#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <configs.h>
#include <default.h>

void * malloc(x_u32 size);
void free(void * ptr);
void * realloc(void * ptr, x_u32 size);
void * calloc(x_u32 num, x_u32 size);

#endif /* __MALLOC_H__ */
