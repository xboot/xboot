#ifndef __IRQ_H__
#define __IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

typedef void (*interrupt_function_t)(void * data);

struct irq_handler_t {
	void (*func)(void * data);
	void * data;
};

struct irq_t {
	/* The irq name */
	const char * name;

	/* Interrupt number */
	const int irq_no;

	/* Irq handler */
	struct irq_handler_t * handler;

	/* Enable irq or not */
	void (*enable)(struct irq_t * irq, bool_t enable);
};

bool_t irq_register(struct irq_t * irq);
bool_t irq_unregister(struct irq_t * irq);
bool_t request_irq(const char * name, interrupt_function_t func, void * data);
bool_t free_irq(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __IRQ_H__ */
