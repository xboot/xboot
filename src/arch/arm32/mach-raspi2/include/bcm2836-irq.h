#ifndef __BCM2836_IRQ_H__
#define __BCM2836_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BCM2836_SUBIRQ_VC_AUX		(0 + 29)
#define BCM2836_SUBIRQ_GPU_GPIO0	(32 + 17)
#define BCM2836_SUBIRQ_GPU_GPIO1	(32 + 18)
#define BCM2836_SUBIRQ_GPU_GPIO2	(32 + 19)
#define BCM2836_SUBIRQ_GPU_GPIO3	(32 + 20)

#define BCM2836_IRQ_GPU_I2C_SPI_SLV	(32 + 11)
#define BCM2836_IRQ_GPU_PWA0		(32 + 13)
#define BCM2836_IRQ_GPU_PWA1		(32 + 14)
#define BCM2836_IRQ_GPU_SMI			(32 + 16)
#define BCM2836_IRQ_GPU_I2C			(32 + 21)
#define BCM2836_IRQ_GPU_SPI			(32 + 22)
#define BCM2836_IRQ_GPU_PCM			(32 + 23)
#define BCM2836_IRQ_GPU_UART		(32 + 25)

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
