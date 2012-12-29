#ifndef __IRQ_H__
#define __IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
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
	const u32_t irq_no;

	/* irq handler */
	irq_handler * handler;

	/* enable irq or disable */
	void (*enable)(struct irq * irq, bool_t enable);
};

/*
 * the list of irq
 */
struct irq_list
{
	struct irq * irq;
	bool_t busy;
	struct hlist_node node;
};


bool_t irq_register(struct irq * irq);
bool_t irq_unregister(struct irq * irq);
bool_t request_irq(const char *name, irq_handler handler);
bool_t free_irq(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* __IRQ_H__ */
