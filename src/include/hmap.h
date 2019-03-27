#ifndef __HMAP_H__
#define __HMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <list.h>
#include <spinlock.h>

struct hmap_t {
	struct hlist_head * hash;
	struct list_head list;
	unsigned int size;
	unsigned int n;
	spinlock_t lock;
};

struct hmap_t * hmap_alloc(unsigned int size);
void hmap_free(struct hmap_t * m);
void hmap_clear(struct hmap_t * m);
void hmap_add(struct hmap_t * m, const char * key, void * value);
void hmap_remove(struct hmap_t * m, const char * key);
void * hmap_search(struct hmap_t * m, const char * key);

#ifdef __cplusplus
}
#endif

#endif /* __HMAP_H__ */
