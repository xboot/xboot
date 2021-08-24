#ifndef __TASK_H__
#define __TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xconfigs.h>
#include <types.h>
#include <stdint.h>
#include <list.h>
#include <irqflags.h>
#include <spinlock.h>
#include <smp.h>
#include <rbtree_augmented.h>

struct task_t;
struct scheduler_t;
typedef void (*task_func_t)(struct task_t * task, void * data);

struct task_data_t {
	const char * fb;
	const char * input;
	void * data;
};

struct task_t {
	struct rb_node node;
	struct scheduler_t * sched;
	uint64_t start;
	uint64_t vtime;
	char * name;
	void * fctx;
	void * stack;
	size_t stksz;
	int nice;
	int dynice;
	task_func_t func;
	void * data;
	int __errno;
};

struct scheduler_t {
	struct rb_root_cached ready;
	struct task_t * running;
	uint64_t min_vtime;
	uint64_t weight;
	spinlock_t lock;
};

extern struct scheduler_t __sched[CONFIG_MAX_SMP_CPUS];

static inline struct scheduler_t * scheduler_self(void)
{
	return &__sched[smp_processor_id()];
}

static inline struct task_t * task_self(void)
{
	return __sched[smp_processor_id()].running;
}

struct task_t * task_create(struct scheduler_t * sched, const char * name, task_func_t func, void * data, size_t stksz, int nice);
void task_destroy(struct task_t * task);
void task_nice(struct task_t * task, int nice);
void task_yield(void);

struct task_data_t * task_data_alloc(const char * fb, const char * input, void * data);
void task_data_free(struct task_data_t * td);

void do_idle_task(void);
void do_init_sched(void);
void scheduler_loop(void);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_H__ */
