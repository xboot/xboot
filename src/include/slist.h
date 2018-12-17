#ifndef __SLIST_H__
#define __SLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <list.h>
#include <stdarg.h>

struct slist_t {
	struct list_head list;
	char * key;
	void * priv;
};

#define slist_for_each_entry(entry, sl) \
	list_for_each_entry(entry, &(sl)->list, list)

struct slist_t * slist_alloc(void);
void slist_free(struct slist_t * sl);
void slist_add(struct slist_t * sl, void * priv, const char * fmt, ...);
void slist_sort(struct slist_t * sl);

#ifdef __cplusplus
}
#endif

#endif /* __SLIST_H__ */
