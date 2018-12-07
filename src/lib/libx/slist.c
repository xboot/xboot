/*
 * libx/slist.c
 */

#include <types.h>
#include <sizes.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <slist.h>

#define MAX_LIST_LENGTH_BITS	(20)

static struct list_head * merge(void * priv, int (*cmp)(void * priv, struct list_head * a, struct list_head * b), struct list_head * a, struct list_head * b)
{
	struct list_head head, * tail = &head;

	while(a && b)
	{
		if((*cmp)(priv, a, b) <= 0)
		{
			tail->next = a;
			a = a->next;
		}
		else
		{
			tail->next = b;
			b = b->next;
		}
		tail = tail->next;
	}
	tail->next = a ? a : b;
	return head.next;
}

static void merge_and_restore_back_links(void * priv, int (*cmp)(void * priv, struct list_head * a, struct list_head * b), struct list_head * head, struct list_head * a, struct list_head * b)
{
	struct list_head * tail = head;
	u8_t count = 0;

	while(a && b)
	{
		if((*cmp)(priv, a, b) <= 0)
		{
			tail->next = a;
			a->prev = tail;
			a = a->next;
		}
		else
		{
			tail->next = b;
			b->prev = tail;
			b = b->next;
		}
		tail = tail->next;
	}
	tail->next = a ? a : b;

	do {
		if(unlikely(!(++count)))
			(*cmp)(priv, tail->next, tail->next);

		tail->next->prev = tail;
		tail = tail->next;
	} while(tail->next);

	tail->next = head;
	head->prev = tail;
}

static void list_sort(void * priv, struct list_head * head, int (*cmp)(void *priv, struct list_head * a, struct list_head * b))
{
	struct list_head * part[MAX_LIST_LENGTH_BITS + 1];
	struct list_head * list;
	int max_lev = 0;
	int lev;

	if(list_empty(head))
		return;

	memset(part, 0, sizeof(part));

	head->prev->next = NULL;
	list = head->next;

	while(list)
	{
		struct list_head * cur = list;
		list = list->next;
		cur->next = NULL;

		for(lev = 0; part[lev]; lev++)
		{
			cur = merge(priv, cmp, part[lev], cur);
			part[lev] = NULL;
		}
		if(lev > max_lev)
		{
			if(unlikely(lev >= ARRAY_SIZE(part) - 1))
				lev--;
			max_lev = lev;
		}
		part[lev] = cur;
	}

	for(lev = 0; lev < max_lev; lev++)
	{
		if(part[lev])
			list = merge(priv, cmp, part[lev], list);
	}
	merge_and_restore_back_links(priv, cmp, head, part[max_lev], list);
}

static char * slist_vasprintf(const char * fmt, va_list ap)
{
	char * p;
	int len;

	len = vasprintf(&p, fmt, ap);
	if(len < 0)
		return NULL;
	return p;
}

static int slist_compare(void * priv, struct list_head * a, struct list_head * b)
{
	char * keya = (char *)list_entry(a, struct slist_t, list)->key;
	char * keyb = (char *)list_entry(b, struct slist_t, list)->key;
	return strcmp(keya, keyb);
}

struct slist_t * slist_alloc(void)
{
	struct slist_t * sl;

	sl = malloc(sizeof(struct slist_t));
	if(!sl)
		return NULL;

	sl->key = NULL;
	init_list_head(&sl->list);
	return sl;
}

void slist_free(struct slist_t * sl)
{
	struct slist_t * pos, * n;

	if(sl)
	{
		list_for_each_entry_safe(pos, n, &sl->list, list)
		{
			if(pos->key)
				free(pos->key);
			free(pos);
		}
		if(sl->key)
			free(sl->key);
		free(sl);
	}
}

void slist_add(struct slist_t * sl, void * priv, const char * fmt, ...)
{
	struct slist_t * n;
	va_list ap;

	if(!sl)
		return;

	n = malloc(sizeof(struct slist_t));
	if(!n)
		return;

	va_start(ap, fmt);
	n->key = slist_vasprintf(fmt, ap);
	va_end(ap);
	n->priv = priv;
	if(n->key)
		list_add_tail(&n->list, &sl->list);
	else
		free(n);
}

void slist_sort(struct slist_t * sl)
{
	if(sl)
		list_sort(NULL, &sl->list, slist_compare);
}
