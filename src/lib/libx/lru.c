/*
 * libx/lru.c
 */

#include <types.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <lru.h>

static uint32_t lru_hash(const char * key, const int nkey)
{
	const char * p;
	uint32_t h = 5381;
	int i;

	for(i = 0, p = key; i < nkey; p++, i++)
	{
		h = (h << 5) + h + *p;
	}
	return h;
}

static struct lru_item_t * lru_hash_search(struct lru_t * l, const char * key, int nkey, uint32_t hv)
{
	struct lru_item_t * item = l->table[hv & ((1 << l->hashpower) - 1)];
	struct lru_item_t * ret = NULL;

	while(item)
	{
		if((nkey == item->nkey) && (memcmp(key, item->data, nkey) == 0))
		{
			ret = item;
			break;
		}
		item = item->hnext;
	}
	return ret;
}

static inline int lru_hash_insert(struct lru_t * l, struct lru_item_t * item, uint32_t hv)
{
	item->hnext = l->table[hv & ((1 << l->hashpower) - 1)];
	l->table[hv & ((1 << l->hashpower) - 1)] = item;
	return 1;
}

static inline struct lru_item_t ** lru_hash_before(struct lru_t * l, const char * key, int nkey, uint32_t hv)
{
	struct lru_item_t ** pos;

	pos = &(l->table[hv & ((1 << l->hashpower) - 1)]);
	while(*pos && ((nkey != (*pos)->nkey) || memcmp(key, (*pos)->data, nkey)))
	{
		pos = &(*pos)->hnext;
	}
	return pos;
}

static inline void lru_hash_remove(struct lru_t * l, const char * key, int nkey, uint32_t hv)
{
	struct lru_item_t ** before = lru_hash_before(l, key, nkey, hv);
	struct lru_item_t * next;

	if(*before)
	{
		next = (*before)->hnext;
		(*before)->hnext = 0;
		*before = next;
	}
}

static inline void lru_remove_item_hv(struct lru_t * l, struct lru_item_t * item, uint32_t hv)
{
	lru_hash_remove(l, item->data, item->nkey, hv);
	if(l->head == item)
	{
		l->head = item->next;
	}
	if(l->tail == item)
	{
		l->tail = item->prev;
	}
	if(item->next)
		item->next->prev = item->prev;
	if(item->prev)
		item->prev->next = item->next;
	l->curr_bytes -= item->nbytes;
	free(item);
}

static inline void lru_remove_item(struct lru_t * l, struct lru_item_t * item)
{
	lru_remove_item_hv(l, item, lru_hash(item->data, item->nkey));
}

static inline void * lru_alloc_item(struct lru_t * l, int sz, struct lru_item_t * old)
{
	int delta = old ? old->nbytes : 0;
	struct lru_item_t * item;
	void * m;

	if((l->curr_bytes + sz - delta) <= l->max_bytes)
	{
		m = malloc(sz);
		if(!m)
			return NULL;
	}
	else if(sz > l->max_bytes)
	{
		return NULL;
	}
	else
	{
		item = l->tail;
		while((item != NULL) && (l->curr_bytes + sz - delta) > l->max_bytes)
		{
			if(item != old)
			{
				lru_remove_item(l, item);
				item = l->tail;
			}
			else
			{
				item = l->tail->prev;
			}
		}
		m = malloc(sz);
		if(!m)
			return NULL;
	}
	l->curr_bytes += sz;
	return m;
}

struct lru_t * lru_alloc(size_t maxbytes, unsigned int hashpower)
{
	struct lru_t * l;

	l = malloc(sizeof(struct lru_t));
	if(!l)
		return NULL;

	l->hashpower = (hashpower == 0 || hashpower > 32) ? 16 : hashpower;
	l->max_bytes = (maxbytes <= 0) ? SZ_1M : maxbytes;
	l->curr_bytes = 0;
	l->table = calloc((1 << l->hashpower), sizeof(void *));
	if(!l->table)
	{
		free(l);
		return NULL;
	}
	l->head = NULL;
	l->tail = NULL;

	return l;
}

void lru_free(struct lru_t * l)
{
	struct lru_item_t * item;

	if(l)
	{
		item = l->head;
		while(item)
		{
			lru_remove_item(l, item);
			item = l->head;
		}
		free(l->table);
		free(l);
	}
}

int lru_get(struct lru_t * l, const char * key, int nkey, char * buf, int nbuf)
{
	struct lru_item_t * item = lru_hash_search(l, key, nkey, lru_hash(key, nkey));
	int len;

	if(item)
	{
		len = min((int)(item->nbytes - sizeof(struct lru_item_t) - item->nkey - 1), nbuf);
		if(len > 0)
		{
			memcpy(buf, (char *)(item->data) + item->nkey + 1, len);
			return len;
		}
	}
	return 0;
}

int lru_set(struct lru_t * l, const char * key, int nkey, char * buf, int nbuf)
{
	uint32_t hv = lru_hash(key, nkey);
	struct lru_item_t * old = lru_hash_search(l, key, nkey, hv);
	int isize = sizeof(struct lru_item_t) + nkey + 1 + nbuf;
	struct lru_item_t * item = lru_alloc_item(l, isize, old);

	if(item)
	{
		if(old)
			lru_remove_item_hv(l, old, hv);
		item->nkey = nkey;
		item->nbytes = isize;
		memcpy(item->data, key, nkey);
		memcpy(item->data + item->nkey + 1, buf, nbuf);
		lru_hash_insert(l, item, hv);
		item->prev = 0;
		item->next = l->head;
		if(item->next)
			item->next->prev = item;
		l->head = item;
		if(l->tail == NULL)
			l->tail = item;
		return nbuf;
	}
	return 0;
}

int lru_remove(struct lru_t * l, const char * key, int nkey)
{
	uint32_t hv = lru_hash(key, nkey);
	struct lru_item_t * item = lru_hash_search(l, key, nkey, hv);

	if(item)
	{
		lru_remove_item_hv(l, item, hv);
		return 1;
	}
	return 0;
}
