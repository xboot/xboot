/*
 * libx/queue.c
 */

#include <types.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include <spinlock.h>
#include <queue.h>

struct queue_t * queue_alloc(void)
{
	struct queue_t * q;

	q = malloc(sizeof(struct queue_t));
	if(!q)
		return NULL;

	init_list_head(&q->node.entry);
	spin_lock_init(&q->lock);
	return q;
}
EXPORT_SYMBOL(queue_alloc);

void queue_free(struct queue_t * q)
{
	if(q)
	{
		queue_clear(q);
		free(q);
	}
}
EXPORT_SYMBOL(queue_free);

void queue_clear(struct queue_t * q)
{
	struct queue_node_t * pos, * n;
	irq_flags_t flags;

	if(!q)
		return;

	spin_lock_irqsave(&q->lock, flags);
	list_for_each_entry_safe(pos, n, &(q->node.entry), entry)
	{
		list_del(&(pos->entry));
		free(pos);
	}
	spin_unlock_irqrestore(&q->lock, flags);
}
EXPORT_SYMBOL(queue_clear);

void queue_push(struct queue_t * q, void * data)
{
	struct queue_node_t * node;
	irq_flags_t flags;

	if(!q || !data)
		return;

	node = malloc(sizeof(struct queue_node_t));
	if(!node)
		return;

	node->data = data;
	spin_lock_irqsave(&q->lock, flags);
	list_add_tail(&(node->entry), &(q->node.entry));
	spin_unlock_irqrestore(&q->lock, flags);
}
EXPORT_SYMBOL(queue_push);

void * queue_pop(struct queue_t * q)
{
	irq_flags_t flags;
	void * data = NULL;

	if(!q)
		return NULL;

	spin_lock_irqsave(&q->lock, flags);
	if(!list_empty(&(q->node.entry)))
	{
		struct list_head * pos = (&q->node.entry)->next;
		struct queue_node_t * node = list_entry(pos, struct queue_node_t, entry);
		data = node->data;
		list_del(pos);
		free(node);
	}
	spin_unlock_irqrestore(&q->lock, flags);

	return data;
}
EXPORT_SYMBOL(queue_pop);

void * queue_peek(struct queue_t * q)
{
	irq_flags_t flags;
	void * data = NULL;

	if(!q)
		return NULL;

	spin_lock_irqsave(&q->lock, flags);
	if(!list_empty(&(q->node.entry)))
	{
		struct list_head * pos = (&q->node.entry)->next;
		struct queue_node_t * node = list_entry(pos, struct queue_node_t, entry);
		data = node->data;
	}
	spin_unlock_irqrestore(&q->lock, flags);

	return data;
}
EXPORT_SYMBOL(queue_peek);
