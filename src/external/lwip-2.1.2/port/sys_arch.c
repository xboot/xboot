#include <xboot.h>
#include <lwip/debug.h>
#include <lwip/def.h>
#include <lwip/sys.h>
#include <lwip/mem.h>
#include <lwip/stats.h>
#include <lwip/arch.h>

struct sys_sem {
	struct mutex_t m;
};

err_t sys_mutex_new(sys_mutex_t * mutex)
{
	if(mutex)
	{
		mutex->m = malloc(sizeof(struct mutex_t));
		if(mutex->m)
		{
			mutex_init(mutex->m);
			return ERR_OK;
		}
		else
			return ERR_MEM;

	}
	return ERR_VAL;
}

void sys_mutex_lock(sys_mutex_t * mutex)
{
	if(mutex)
		mutex_lock(mutex->m);
}

void sys_mutex_unlock(sys_mutex_t * mutex)
{
	if(mutex)
		mutex_unlock(mutex->m);
}

void sys_mutex_free(sys_mutex_t * mutex)
{
	if(mutex && mutex->m)
		free(mutex->m);
}

err_t sys_mbox_new(sys_mbox_t * mbox, int size)
{
	if(mbox)
	{
		mbox->ch = channel_alloc(size * sizeof(void *));
		if(mbox->ch)
			return ERR_OK;
		else
			return ERR_MEM;
	}
	return ERR_VAL;
}

void sys_mbox_post(sys_mbox_t * mbox, void * msg)
{
	if(mbox)
		channel_send(mbox->ch, msg, sizeof(void *));
}

err_t sys_mbox_trypost(sys_mbox_t * mbox, void * msg)
{
	sys_mbox_post(mbox, msg);
	return ERR_OK;
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t * mbox, void * msg)
{
	sys_mbox_post(mbox, msg);
	return ERR_OK;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t * mbox, void ** msg, u32_t timeout)
{
	channel_recv(mbox->ch, *msg, sizeof(void *));
	return 0;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t * mbox, void ** msg)
{
	return sys_arch_mbox_fetch(mbox, msg, 0);
}

void sys_mbox_free(sys_mbox_t * mbox)
{
	if(mbox && mbox->ch)
		channel_free(mbox->ch);
}

struct thread_data_t {
	lwip_thread_fn thread;
	void * arg;
};

static void sys_thread_task(struct task_t * task, void * data)
{
	struct thread_data_t * td = (struct thread_data_t *)data;

	if(td)
	{
		td->thread(td->arg);
		free(td);
	}
}

sys_thread_t sys_thread_new(const char * name, lwip_thread_fn thread, void * arg, int stacksize, int prio)
{
	sys_thread_t st;
	struct thread_data_t * td;

	td = (struct thread_data_t *)malloc(sizeof(struct thread_data_t));
	td->thread = thread;
	td->arg = arg;
	st.task = task_create(scheduler_self(), name, NULL, NULL, sys_thread_task, td, stacksize, 0);

	return st;
}

void sys_init(void)
{
}

u32_t sys_now(void)
{
	task_yield();
	return (u32_t)ktime_to_ms(ktime_get());
}

u32_t sys_jiffies(void)
{
	task_yield();
	return (u32_t)ktime_to_ns(ktime_get());
}
