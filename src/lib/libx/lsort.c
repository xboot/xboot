/*
 * libx/lsort.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>
#include <list.h>
#include <sizes.h>
#include <lsort.h>

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
	unsigned char count = 0;

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

void lsort(void * priv, struct list_head * head, int (*cmp)(void * priv, struct list_head * a, struct list_head * b))
{
	struct list_head * part[20 + 1];
	struct list_head * list;
	int maxlev = 0;
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
		if(lev > maxlev)
		{
			if(unlikely(lev >= ARRAY_SIZE(part) - 1))
				lev--;
			maxlev = lev;
		}
		part[lev] = cur;
	}
	for(lev = 0; lev < maxlev; lev++)
	{
		if(part[lev])
			list = merge(priv, cmp, part[lev], list);
	}
	merge_and_restore_back_links(priv, cmp, head, part[maxlev], list);
}
