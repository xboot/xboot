#ifndef __BCM2836_IRQ_H__
#define __BCM2836_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BCM2836_IRQ_ARM_TIMER		(64 + 0)
#define BCM2836_IRQ_ARM_MBOX		(64 + 1)
#define BCM2836_IRQ_ARM_DBELL0		(64 + 2)
#define BCM2836_IRQ_ARM_DBELL1		(64 + 3)
#define BCM2836_IRQ_ARM_GPUHALT0	(64 + 4)
#define BCM2836_IRQ_ARM_GPUHALT1	(64 + 5)
#define BCM2836_IRQ_ARM_ILLEGAL1	(64 + 6)
#define BCM2836_IRQ_ARM_ILLEGAL0	(64 + 7)

#ifdef __cplusplus
}
#endif

#endif /* __BCM2836_IRQ_H__ */
