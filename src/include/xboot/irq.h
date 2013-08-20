#ifndef __IRQ_H__
#define __IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

typedef void (*irq_handler_t)(void);

struct irq_t {
	/* The irq name */
	const char * name;

	/* Interrupt number */
	const u32_t irq_no;

	/* Irq handler */
	irq_handler_t * handler;

	/* Enable irq or disable */
	void (*enable)(struct irq_t * irq, bool_t enable);
};

bool_t irq_register(struct irq_t * irq);
bool_t irq_unregister(struct irq_t * irq);
bool_t request_irq(const char * name, irq_handler_t handler);
bool_t free_irq(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __IRQ_H__ */
