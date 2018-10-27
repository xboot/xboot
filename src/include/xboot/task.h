#ifndef __TASK_H__
#define __TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <irqflags.h>
#include <spinlock.h>

struct task_t;
struct scheduler_t;
typedef void (*task_func_t)(struct scheduler_t * s, void * data);

enum task_status_t {
	TASK_STATUS_DEAD	= 0,
	TASK_STATUS_READY	= 1,
	TASK_STATUS_SUSPEND	= 2,
};

struct task_t {
	struct scheduler_t * s;
	struct list_head list;
	char * name;
	enum task_status_t status;
	void * fctx;
	void * stack;
	size_t size;
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

struct scheduler_t * scheduler_alloc(void);
void scheduler_free(struct scheduler_t * s);
void scheduler_loop(struct scheduler_t * s);

struct task_t * task_create(struct scheduler_t * s, const char * name, task_func_t func, void * data, size_t size);
void task_destory(struct scheduler_t * s, struct task_t * task);
void task_suspend(struct scheduler_t * s, struct task_t * task);
void task_resume(struct scheduler_t * s, struct task_t * task);
void task_yield(struct scheduler_t * s);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_H__ */
