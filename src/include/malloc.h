#ifndef __MALLOC_H__
#define __MALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot/module.h>
#include <types.h>

void * mm_create(void * mem);
void * mm_create_with_pool(void * mem, size_t bytes);
void mm_destroy(void * mm);
void * mm_add_pool(void * mm, void * mem, size_t bytes);
void mm_remove_pool(void * mm, void * pool);
void * mm_get_pool(void * mm);

void * malloc(size_t size);
void * memalign(size_t align, size_t size);
void * realloc(void * ptr, size_t size);
void * calloc(size_t nmemb, size_t size);
void free(void * ptr);

#ifdef __cplusplus
}
#endif

#endif /* __MALLOC_H__ */
