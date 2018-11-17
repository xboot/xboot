#ifndef __VEXPRESS_IRQ_H__
#define __VEXPRESS_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#define VEXPRESS_IRQ_WDOG		(32 + 0)
#define VEXPRESS_IRQ_SOFT		(32 + 1)
#define VEXPRESS_IRQ_DBGURX		(32 + 2)
#define VEXPRESS_IRQ_DBGUTX		(32 + 3)
#define VEXPRESS_IRQ_TMIER0_1	(32 + 4)
#define VEXPRESS_IRQ_TMIER2_3	(32 + 5)
#define VEXPRESS_IRQ_GPIO0		(32 + 6)
#define VEXPRESS_IRQ_GPIO1		(32 + 7)
#define VEXPRESS_IRQ_GPIO2		(32 + 8)
#define VEXPRESS_IRQ_RTC		(32 + 10)
#define VEXPRESS_IRQ_SSP		(32 + 11)
#define VEXPRESS_IRQ_UART0		(32 + 12)
#define VEXPRESS_IRQ_UART1		(32 + 13)
#define VEXPRESS_IRQ_UART2		(32 + 14)
#define VEXPRESS_IRQ_UART3		(32 + 15)
#define VEXPRESS_IRQ_SCI		(32 + 16)
#define VEXPRESS_IRQ_MMCI0A		(32 + 17)
#define VEXPRESS_IRQ_MMCI0B		(32 + 18)
#define VEXPRESS_IRQ_AACI		(32 + 19)
#define VEXPRESS_IRQ_KMI0		(32 + 20)
#define VEXPRESS_IRQ_KMI1		(32 + 21)
#define VEXPRESS_IRQ_CHARLCD	(32 + 22)
#define VEXPRESS_IRQ_LCD		(32 + 23)
#define VEXPRESS_IRQ_DMA		(32 + 24)
#define VEXPRESS_IRQ_PWRFAIL	(32 + 25)
#define VEXPRESS_IRQ_PISMO		(32 + 26)
#define VEXPRESS_IRQ_DOC		(32 + 27)
#define VEXPRESS_IRQ_ETH		(32 + 28)
#define VEXPRESS_IRQ_USB		(32 + 29)
#define VEXPRESS_IRQ_TSPEN		(32 + 30)
#define VEXPRESS_IRQ_TSKPAD		(32 + 31)

#ifdef __cplusplus
}
#endif

#endif /* __VEXPRESS_IRQ_H__ */
