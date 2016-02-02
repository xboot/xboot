#ifndef __VIRT_IRQ_H__
#define __VIRT_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#define VIRT_IRQ_PPI_VMI		(16 + 9)
#define VIRT_IRQ_PPI_HYPTIMER	(16 + 10)
#define VIRT_IRQ_PPI_VIRTIMER	(16 + 11)
#define VIRT_IRQ_PPI_LEGACYFIQ	(16 + 12)
#define VIRT_IRQ_PPI_STIMER		(16 + 13)
#define VIRT_IRQ_PPI_NSTIMER	(16 + 14)
#define VIRT_IRQ_PPI_LEGACYIRQ	(16 + 15)

#define VIRT_IRQ_UART			(32 + 0)
#define VIRT_IRQ_RTC			(32 + 1)

#ifdef __cplusplus
}
#endif

#endif /* __VIRT_IRQ_H__ */
