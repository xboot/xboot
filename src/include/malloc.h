#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <xboot.h>
#include <types.h>

void * malloc(u32_t size);
void free(void * ptr);
void * realloc(void * ptr, u32_t size);
void * calloc(u32_t num, u32_t size);

#endif /* __MALLOC_H__ */
