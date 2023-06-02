#include <xboot.h>
#include "usb_osal.h"
#include "usb_errno.h"

struct osal_thread_data_t {
	usb_thread_entry_t entry;
	void * arg;
};

static void osal_thread_task(struct task_t * task, void * data)
{
	struct osal_thread_data_t * otd = (struct osal_thread_data_t *)data;

	if(otd)
	{
		otd->entry(otd->arg);
		free(otd);
	}
}

usb_osal_thread_t usb_osal_thread_create(const char * name, uint32_t stack_size, uint32_t prio, usb_thread_entry_t entry, void * args)
{
	struct osal_thread_data_t * otd;

	otd = (struct osal_thread_data_t *)malloc(sizeof(struct osal_thread_data_t));
	otd->entry = entry;
	otd->arg = args;
	return (usb_osal_thread_t)task_create(scheduler_self(), name, NULL, NULL, osal_thread_task, otd, stack_size, 0);
}

usb_osal_sem_t usb_osal_sem_create(uint32_t initial_count)
{
	struct waiter_t * w = malloc(sizeof(struct waiter_t));

	if(w)
	{
		waiter_init(w);
		waiter_add(w, initial_count > 0 ? initial_count : 1);
	}
	return (usb_osal_sem_t)w;
}

void usb_osal_sem_delete(usb_osal_sem_t sem)
{
	struct waiter_t * w = (struct waiter_t *)sem;

	if(w)
		free(w);
}

int usb_osal_sem_take(usb_osal_sem_t sem, uint32_t timeout)
{
	struct waiter_t * w = (struct waiter_t *)sem;

	if(w)
	{
		waiter_wait(w);
		waiter_add(w, 1);
	}
	return 0;
}

int usb_osal_sem_give(usb_osal_sem_t sem)
{
	struct waiter_t * w = (struct waiter_t *)sem;

	if(w)
		waiter_sub(w, 1);
	return 0;
}

usb_osal_mutex_t usb_osal_mutex_create(void)
{
	struct mutex_t * m = malloc(sizeof(struct mutex_t));

	if(m)
		mutex_init(m);
	return (usb_osal_mutex_t)m;
}

void usb_osal_mutex_delete(usb_osal_mutex_t mutex)
{
	struct mutex_t * m = (struct mutex_t *)mutex;

	if(m)
		free(m);
}

int usb_osal_mutex_take(usb_osal_mutex_t mutex)
{
	struct mutex_t * m = (struct mutex_t *)mutex;

	if(m)
		mutex_lock(m);
	return 0;
}

int usb_osal_mutex_give(usb_osal_mutex_t mutex)
{
	struct mutex_t * m = (struct mutex_t *)mutex;

	if(m)
		mutex_unlock(m);
	return 0;
}

usb_osal_mq_t usb_osal_mq_create(uint32_t max_msgs)
{
	struct channel_t * ch = channel_alloc(max_msgs * sizeof(uintptr_t));

	return (usb_osal_mq_t)ch;
}

int usb_osal_mq_send(usb_osal_mq_t mq, uintptr_t addr)
{
	struct channel_t * ch = (struct channel_t *)mq;

	channel_send(ch, (unsigned char *)&addr, sizeof(uintptr_t));
	return 0;
}

int usb_osal_mq_recv(usb_osal_mq_t mq, uintptr_t * addr, uint32_t timeout)
{
	struct channel_t * ch = (struct channel_t *)mq;

	channel_recv(ch, (unsigned char *)addr, sizeof(uintptr_t));
	return 0;
}

static spinlock_t __osal_lock = SPIN_LOCK_INIT();

size_t usb_osal_enter_critical_section(void)
{
	irq_flags_t flags;

	spin_lock_irqsave(&__osal_lock, flags);
	return (size_t)flags;
}

void usb_osal_leave_critical_section(size_t flag)
{
	irq_flags_t flags = (irq_flags_t)flag;

	spin_unlock_irqrestore(&__osal_lock, flags);
}

void usb_osal_msleep(uint32_t delay)
{
	msleep(delay);
}
