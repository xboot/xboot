#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum irq_type_t {
	IRQ_TYPE_NONE			= 0,
	IRQ_TYPE_LEVEL_LOW		= 1,
	IRQ_TYPE_LEVEL_HIGH		= 2,
	IRQ_TYPE_EDGE_FALLING	= 3,
	IRQ_TYPE_EDGE_RISING	= 4,
	IRQ_TYPE_EDGE_BOTH		= 5,
};

struct irq_handler_t {
	void (*func)(void * data);
	void * data;
};

struct irq_t {
	struct kobj_t * kobj;
	const char * name;
	const int no;
	struct irq_handler_t * handler;

	void (*enable)(struct irq_t * irq);
	void (*disable)(struct irq_t * irq);
	void (*set_type)(struct irq_t * irq, enum irq_type_t type);
};

bool_t irq_register(struct irq_t * irq);
bool_t irq_unregister(struct irq_t * irq);
bool_t request_irq(const char * name, void (*func)(void *), enum irq_type_t type, void * data);
bool_t free_irq(const char * name);
void enable_irq(const char * name);
void disable_irq(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __INTERRUPT_H__ */
