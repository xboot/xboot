/*
 * libx/hmap.c
 */

#include <types.h>
#include <stddef.h>
#include <malloc.h>
#include <shash.h>
#include <log2.h>
#include <hmap.h>

struct hmap_node_t {
	struct hlist_node node;
	struct list_head head;
	char * key;
	void * value;
};

struct hmap_t * hmap_alloc(unsigned int size)
{
	struct hmap_t * m;
	int i;

	if(size < 16)
		size = 16;
	if(size & (size - 1))
		size = roundup_pow_of_two(size);

	m = malloc(sizeof(struct hmap_t));
	if(!m)
		return NULL;

	m->hash = malloc(sizeof(struct hlist_head) * size);
	if(!m->hash)
	{
		free(m);
		return NULL;
	}
	for(i = 0; i < size; i++)
		init_hlist_head(&m->hash[i]);
	init_list_head(&m->list);
	spin_lock_init(&m->lock);
	m->size = size;
	m->n = 0;

	return m;
}

void hmap_free(struct hmap_t * m)
{
	if(m)
	{
		hmap_clear(m);
		free(m->hash);
		free(m);
	}
}

void hmap_clear(struct hmap_t * m)
{
	struct hmap_node_t * pos, * n;
	irq_flags_t flags;

	if(m)
	{
		list_for_each_entry_safe(pos, n, &m->list, head)
		{
			spin_lock_irqsave(&m->lock, flags);
			hlist_del(&pos->node);
			list_del(&pos->head);
			m->n--;
			spin_unlock_irqrestore(&m->lock, flags);
			free(pos->key);
			free(pos);
		}
	}
}

static void hmap_resize(struct hmap_t * m, unsigned int size)
{
	struct hmap_node_t * pos, * n;
	struct hlist_head * hash;
	irq_flags_t flags;
	int i;

	if(!m)
		return;

	if(size < 16)
		size = 16;
	if(size & (size - 1))
		size = roundup_pow_of_two(size);

	hash = malloc(sizeof(struct hlist_head) * size);
	if(!hash)
		return;
	for(i = 0; i < size; i++)
		init_hlist_head(&hash[i]);

	list_for_each_entry_safe(pos, n, &m->list, head)
	{
		spin_lock_irqsave(&m->lock, flags);
		hlist_del(&pos->node);
		spin_unlock_irqrestore(&m->lock, flags);
	}
	free(m->hash);

	spin_lock_irqsave(&m->lock, flags);
	m->hash = hash;
	m->size = size;
	list_for_each_entry_safe(pos, n, &m->list, head)
	{
		hlist_add_head(&pos->node, &m->hash[shash(pos->key) & (m->size - 1)]);
	}
	spin_unlock_irqrestore(&m->lock, flags);
}

void hmap_add(struct hmap_t * m, const char * key, void * value)
{
	struct hmap_node_t * pos;
	struct hlist_node * n;
	irq_flags_t flags;

	if(!m || !key)
		return;

	hlist_for_each_entry_safe(pos, n, &m->hash[shash(key) & (m->size - 1)], node)
	{
		if(strcmp(pos->key, key) == 0)
		{
			pos->value = value;
			return;
		}
	}

	if(m->n > (m->size >> 1))
		hmap_resize(m, m->size << 1);

	pos = malloc(sizeof(struct hmap_node_t));
	if(!pos)
		return;

	pos->key = strdup(key);
	pos->value = value;
	spin_lock_irqsave(&m->lock, flags);
	init_hlist_node(&pos->node);
	hlist_add_head(&pos->node, &m->hash[shash(pos->key) & (m->size - 1)]);
	init_list_head(&pos->head);
	list_add_tail(&pos->head, &m->list);
	m->n++;
	spin_unlock_irqrestore(&m->lock, flags);
}

void hmap_remove(struct hmap_t * m, const char * key)
{
	struct hmap_node_t * pos;
	struct hlist_node * n;
	irq_flags_t flags;

	if(!m || !key)
		return;

	if((m->size > 16) && (m->n < (m->size >> 1)))
		hmap_resize(m, m->size >> 1);

	hlist_for_each_entry_safe(pos, n, &m->hash[shash(key) & (m->size - 1)], node)
	{
		if(strcmp(pos->key, key) == 0)
		{
			spin_lock_irqsave(&m->lock, flags);
			hlist_del(&pos->node);
			list_del(&pos->head);
			m->n--;
			spin_unlock_irqrestore(&m->lock, flags);
			free(pos->key);
			free(pos);
			return;
		}
	}
}

void hmap_walk(struct hmap_t * m, void (*cb)(const char * key, void * value))
{
	struct hmap_node_t * pos, * n;

	if(m && cb)
	{
		list_for_each_entry_safe(pos, n, &m->list, head)
		{
			cb(pos->key, pos->value);
		}
	}
}

void * hmap_search(struct hmap_t * m, const char * key)
{
	struct hmap_node_t * pos;
	struct hlist_node * n;

	if(!m || !key)
		return NULL;

	hlist_for_each_entry_safe(pos, n, &m->hash[shash(key) & (m->size - 1)], node)
	{
		if(strcmp(pos->key, key) == 0)
			return pos->value;
	}
	return NULL;
}
