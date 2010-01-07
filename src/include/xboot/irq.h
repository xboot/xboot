#ifndef __IRQ_H__
#define __IRQ_H__


#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>


/* the irq handler type */
typedef void (*irq_handler)(void);

/*
 * the struct of interrupt.
 */
struct irq {
	/* the irq name */
	const char * name;

	/* interrupt number */
	const x_u32 irq_no;

	/* irq handler */
	irq_handler * handler;

	/* enable irq or disable */
	void (*enable)(struct irq * irq, x_bool enable);
};

/*
 * the list of irq
 */
struct irq_list
{
	struct irq * irq;
	x_bool busy;
	struct hlist_node node;
};


x_bool irq_register(struct irq * irq);
x_bool irq_unregister(struct irq * irq);
x_bool request_irq(const char *name, irq_handler handler);
x_bool free_irq(const char *name);


#endif /* __IRQ_H__ */
