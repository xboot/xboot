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

struct irqchip_t
{
	struct kobj_t * kobj;
	const char * name;
	int base;
	int nirq;
	struct irq_handler_t * handler;
	void (*enable)(struct irqchip_t * chip, int offset);
	void (*disable)(struct irqchip_t * chip, int offset);
	void (*settype)(struct irqchip_t * chip, int offset, enum irq_type_t type);
	void (*process)(struct irqchip_t * chip);
	void * priv;
};

bool_t register_irqchip(struct irqchip_t * chip);
bool_t unregister_irqchip(struct irqchip_t * chip);
bool_t register_sub_irqchip(int parent, struct irqchip_t * chip);
bool_t unregister_sub_irqchip(int parent, struct irqchip_t * chip);
bool_t request_irq(int irq, void (*func)(void *), enum irq_type_t type, void * data);
bool_t free_irq(int irq);
void enable_irq(int irq);
void disable_irq(int irq);
void interrupt_handle_exception(void * regs);

#ifdef __cplusplus
}
#endif

#endif /* __INTERRUPT_H__ */
