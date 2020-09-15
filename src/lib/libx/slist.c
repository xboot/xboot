/*
 * libx/slist.c
 */

#include <types.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <list.h>
#include <lsort.h>
#include <malloc.h>
#include <slist.h>

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

static char * slist_vasprintf(const char * fmt, va_list ap)
{
	char * p;
	int len;

	len = vasprintf(&p, fmt, ap);
	if(len < 0)
		return NULL;
	return p;
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

static int slist_compare(void * priv, struct list_head * a, struct list_head * b)
{
	char * keya = (char *)list_entry(a, struct slist_t, list)->key;
	char * keyb = (char *)list_entry(b, struct slist_t, list)->key;
	return strcmp(keya, keyb);
}

void slist_sort(struct slist_t * sl)
{
	if(sl)
		lsort(NULL, &sl->list, slist_compare);
}

int slist_empty(struct slist_t * sl)
{
	if(sl && !list_empty_careful(&sl->list))
		return 0;
	return 1;
}
