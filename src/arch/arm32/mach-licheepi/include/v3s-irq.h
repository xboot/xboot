#ifndef __V3S_IRQ_H__
#define __V3S_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#define V3S_IRQ_UART0			(32)
#define V3S_IRQ_UART1			(33)
#define V3S_IRQ_UART2			(34)
#define V3S_IRQ_I2C0			(38)
#define V3S_IRQ_I2C1			(39)
#define V3S_IRQ_GPIOB			(47)
#define V3S_IRQ_GPIOG			(49)
#define V3S_IRQ_TIMER0			(50)
#define V3S_IRQ_TIMER1			(51)
#define V3S_IRQ_TIMER2			(52)
#define V3S_IRQ_WDOG			(57)
#define V3S_IRQ_AUDIO			(61)
#define V3S_IRQ_KEYADC			(62)
#define V3S_IRQ_ENMI			(64)
#define V3S_IRQ_R_ALARM0		(72)
#define V3S_IRQ_R_ALARM1		(73)
#define V3S_IRQ_DMA				(82)
#define V3S_IRQ_HSTIMER0		(83)
#define V3S_IRQ_HSTIMER1		(84)
#define V3S_IRQ_VE				(90)
#define V3S_IRQ_SDMMC0			(92)
#define V3S_IRQ_SDMMC1			(93)
#define V3S_IRQ_SDMMC2			(94)
#define V3S_IRQ_SPI0			(97)
#define V3S_IRQ_USB_OTG_DEVICE	(103)
#define V3S_IRQ_USB_OTG_EHCI0	(104)
#define V3S_IRQ_USB_OTG_OHCI0	(105)
#define V3S_IRQ_CE				(112)
#define V3S_IRQ_EMAC			(114)
#define V3S_IRQ_CSI0			(115)
#define V3S_IRQ_CSI1			(116)
#define V3S_IRQ_CSI_CCI			(117)
#define V3S_IRQ_TCON			(118)
#define V3S_IRQ_MIPI_CSI		(122)
#define V3S_IRQ_DE				(127)
#define V3S_IRQ_GPU_GP			(129)
#define V3S_IRQ_GPU_CTI0		(140)
#define V3S_IRQ_GPU_CTI1		(141)
#define V3S_IRQ_GPU_CTI2		(142)
#define V3S_IRQ_GPU_CTI3		(143)
#define V3S_IRQ_GPU_COMMTX0		(144)
#define V3S_IRQ_GPU_COMMTX1		(145)
#define V3S_IRQ_GPU_COMMTX2		(146)
#define V3S_IRQ_GPU_COMMTX3		(147)
#define V3S_IRQ_GPU_COMMRX0		(148)
#define V3S_IRQ_GPU_COMMRX1		(149)
#define V3S_IRQ_GPU_COMMRX2		(150)
#define V3S_IRQ_GPU_COMMRX3		(151)
#define V3S_IRQ_GPU_PMU0		(152)
#define V3S_IRQ_GPU_PMU1		(153)
#define V3S_IRQ_GPU_PMU2		(154)
#define V3S_IRQ_GPU_PMU3		(155)
#define V3S_IRQ_GPU_AXI			(156)

#ifdef __cplusplus
}
#endif

#endif /* __V3S_IRQ_H__ */
