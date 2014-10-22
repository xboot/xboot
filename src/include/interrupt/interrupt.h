#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct irq_handler_t {
	void (*func)(void * data);
	void * data;
};

struct irq_t {
	struct kobj_t * kobj;
	const char * name;
	const int irq_no;
	struct irq_handler_t * handler;
	void (*enable)(struct irq_t * irq, bool_t enable);
};

bool_t irq_register(struct irq_t * irq);
bool_t irq_unregister(struct irq_t * irq);
bool_t request_irq(const char * name, void (*func)(void *), void * data);
bool_t free_irq(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __INTERRUPT_H__ */
