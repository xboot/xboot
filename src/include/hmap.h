#ifndef __HMAP_H__
#define __HMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <list.h>
#include <spinlock.h>

struct hmap_entry_t {
	struct hlist_node node;
	struct list_head head;
	char * key;
	void * value;
};

struct hmap_t {
	struct hlist_head * hash;
	struct list_head list;
	unsigned int size;
	unsigned int n;
	spinlock_t lock;
	void (*callback)(struct hmap_t * m, struct hmap_entry_t * e);
};

#define hmap_for_each_entry(entry, m) \
	list_for_each_entry(entry, &(m)->list, head)

#define hmap_for_each_entry_reverse(entry, m) \
	list_for_each_entry_reverse(entry, &(m)->list, head)

struct hmap_t * hmap_alloc(int size, void (*cb)(struct hmap_t *, struct hmap_entry_t *));
void hmap_free(struct hmap_t * m);
void hmap_clear(struct hmap_t * m);
void hmap_add(struct hmap_t * m, const char * key, void * value);
void hmap_remove(struct hmap_t * m, const char * key);
void hmap_sort(struct hmap_t * m);
void * hmap_search(struct hmap_t * m, const char * key);

#ifdef __cplusplus
}
#endif

#endif /* __HMAP_H__ */
