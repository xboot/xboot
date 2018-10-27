#ifndef __TASK_H__
#define __TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <irqflags.h>
#include <spinlock.h>

typedef void (*task_func_t)(void * data);

struct task_t;
struct scheduler_t;

enum task_status_t {
	TASK_STATUS_DEAD	= 0,
	TASK_STATUS_READY	= 1,
	TASK_STATUS_SUSPEND	= 2,
};

struct task_t {
	struct scheduler_t * sched;
	struct list_head list;
	char * name;
	enum task_status_t status;
	void * fctx;
	void * stack;
	size_t size;
	int weight;
	task_func_t func;
	void * data;
};

struct scheduler_t {
	struct list_head dead;
	struct list_head ready;
	struct list_head suspend;
	struct task_t * running;
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

struct task_t * task_create(struct scheduler_t * sched, const char * name, task_func_t func, void * data, size_t size, int weight);
void task_destory(struct task_t * task);
void task_suspend(struct task_t * task);
void task_resume(struct task_t * task);
void task_yield(void);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_H__ */
