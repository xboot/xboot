#ifndef __TASK_H__
#define __TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <irqflags.h>
#include <spinlock.h>
#include <smp.h>
#include <rbtree.h>
#include <rbtree_augmented.h>

struct task_t;
struct scheduler_t;

typedef void (*task_func_t)(struct task_t * task, void * data);

enum task_status_t {
	TASK_STATUS_SUSPEND	= 0,
	TASK_STATUS_READY	= 1,
	TASK_STATUS_DEAD	= 2,
};

struct task_t {
	struct rb_node node;
	struct list_head list;
	struct scheduler_t * sched;
	enum task_status_t status;
	uint64_t vruntime;
	uint64_t start;
	char * path;
	void * fctx;
	void * stack;
	size_t stksz;
	int weight;
	task_func_t func;
	void * data;

	int __errno;
	void * __xfs_ctx;
};

struct scheduler_t {
	struct rb_root root;
	struct list_head head;
	struct task_t * running;
	struct task_t * next;
	uint64_t min_vruntime;
	spinlock_t lock;
};

extern struct scheduler_t * __sched[CONFIG_MAX_CPUS];

static inline struct scheduler_t * scheduler_self(void)
{
	return __sched[smp_processor_id()];
}
static inline struct task_t * task_self(void)
{
	return __sched[smp_processor_id()]->running;
}
void scheduler_loop(void);

struct task_t * task_create(struct scheduler_t * sched, const char * path, task_func_t func, void * data, size_t stksz, int weight);
void task_destroy(struct task_t * task);
void task_suspend(struct task_t * task);
void task_resume(struct task_t * task);
void task_yield(void);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_H__ */
