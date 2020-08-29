#ifndef __QUEUE_H__
#define __QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <list.h>
#include <spinlock.h>

struct queue_node_t {
	struct list_head entry;
	void * data;
};

struct queue_t
{
	struct queue_node_t node;
	spinlock_t lock;
	int available;
};

struct queue_t * queue_alloc(void);
void queue_free(struct queue_t * q, void (*cb)(struct queue_node_t *));
void queue_clear(struct queue_t * q, void (*cb)(struct queue_node_t *));
int queue_avail(struct queue_t * q);
void queue_push(struct queue_t * q, void * data);
void * queue_pop(struct queue_t * q);
void * queue_peek(struct queue_t * q);

#ifdef __cplusplus
}
#endif

#endif /* __QUEUE_H__ */
