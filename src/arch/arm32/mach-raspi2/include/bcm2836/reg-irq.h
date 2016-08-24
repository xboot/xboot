#ifndef __BCM2836_REG_IRQ_H__
#define __BCM2836_REG_IRQ_H__

#define BCM2836_IRQ_BASE	(0x3f00b200)

#define IRQ_PEND_BASIC		(0x00)
#define IRQ_PEND1			(0x04)
#define IRQ_PEND2			(0x08)
#define IRQ_FIQCTL			(0x0c)
#define IRQ_ENABLE1			(0x10)
#define IRQ_ENABLE2			(0x14)
#define IRQ_ENABLE_BASIC	(0x18)
#define IRQ_DISABLE1		(0x1c)
#define IRQ_DISABLE2		(0x20)
#define IRQ_DISABLE_BASIC	(0x24)

#endif /* __BCM2836_REG_IRQ_H__ */
