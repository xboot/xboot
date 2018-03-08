#ifndef __H2+_IRQ_H__
#define __H2+_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#define H3_IRQ_UART0			(32)
#define H3_IRQ_UART1			(33)
#define H3_IRQ_UART2			(34)
#define H3_IRQ_UART3			(35)
#define H3_IRQ_I2C0				(38)
#define H3_IRQ_I2C1				(39)
#define H3_IRQ_I2C2				(40)
#define H3_IRQ_GPIOA			(43)
#define H3_IRQ_OWA				(44)
#define H3_IRQ_I2S0				(45)
#define H3_IRQ_I2S1				(46)
#define H3_IRQ_I2S2				(47)
#define H3_IRQ_GPIOG			(49)
#define H3_IRQ_TIMER0			(50)
#define H3_IRQ_TIMER1			(51)
#define H3_IRQ_WDOG				(57)
#define H3_IRQ_AUDIO			(61)
#define H3_IRQ_KEYADC			(62)
#define H3_IRQ_THS				(63)
#define H3_IRQ_ENMI				(64)
#define H3_IRQ_R_TIMER0			(65)
#define H3_IRQ_R_TIMER1			(66)
#define H3_IRQ_R_WDOG			(68)
#define H3_IRQ_R_CIRRX			(69)
#define H3_IRQ_R_UART			(70)
#define H3_IRQ_R_ALARM0			(72)
#define H3_IRQ_R_ALARM1			(73)
#define H3_IRQ_R_TIMER2			(74)
#define H3_IRQ_R_TIMER3			(75)
#define H3_IRQ_R_I2C			(76)
#define H3_IRQ_R_GPIOL			(77)
#define H3_IRQ_R_TWD			(78)
#define H3_IRQ_MBOX				(81)
#define H3_IRQ_DMA				(82)
#define H3_IRQ_HSTIMER			(83)
#define H3_IRQ_SMC				(88)
#define H3_IRQ_VE				(90)
#define H3_IRQ_SDMMC0			(92)
#define H3_IRQ_SDMMC1			(93)
#define H3_IRQ_SDMMC2			(94)
#define H3_IRQ_SPI0				(97)
#define H3_IRQ_SPI1				(98)
#define H3_IRQ_NAND				(102)
#define H3_IRQ_USB_OTG_DEVICE	(103)
#define H3_IRQ_USB_OTG_EHCI0	(104)
#define H3_IRQ_USB_OTG_OHCI0	(105)
#define H3_IRQ_USB_EHCI1		(106)
#define H3_IRQ_USB_OHCI1		(107)
#define H3_IRQ_USB_EHCI2		(108)
#define H3_IRQ_USB_OHCI2		(109)
#define H3_IRQ_USB_EHCI3		(110)
#define H3_IRQ_USB_OHCI3		(111)
#define H3_IRQ_SS_S				(112)
#define H3_IRQ_TS				(113)
#define H3_IRQ_EMAC				(114)
#define H3_IRQ_SCR				(115)
#define H3_IRQ_CSI				(116)
#define H3_IRQ_CSI_CCI			(117)
#define H3_IRQ_LCD0				(118)
#define H3_IRQ_LCD1				(119)
#define H3_IRQ_HDMI				(120)
#define H3_IRQ_TVE				(124)
#define H3_IRQ_DIT				(125)
#define H3_IRQ_SS_NS			(126)
#define H3_IRQ_DE				(127)
#define H3_IRQ_GPU_GP			(129)
#define H3_IRQ_GPU_GPMMU		(130)
#define H3_IRQ_GPU_PP0			(131)
#define H3_IRQ_GPU_PPMMU0		(132)
#define H3_IRQ_GPU_PMU			(133)
#define H3_IRQ_GPU_PP1			(134)
#define H3_IRQ_GPU_PPMMU1		(135)
#define H3_IRQ_GPU_CTI0			(140)
#define H3_IRQ_GPU_CTI1			(141)
#define H3_IRQ_GPU_CTI2			(142)
#define H3_IRQ_GPU_CTI3			(143)
#define H3_IRQ_GPU_COMMTX0		(144)
#define H3_IRQ_GPU_COMMTX1		(145)
#define H3_IRQ_GPU_COMMTX2		(146)
#define H3_IRQ_GPU_COMMTX3		(147)
#define H3_IRQ_GPU_COMMRX0		(148)
#define H3_IRQ_GPU_COMMRX1		(149)
#define H3_IRQ_GPU_COMMRX2		(150)
#define H3_IRQ_GPU_COMMRX3		(151)
#define H3_IRQ_GPU_PMU0			(152)
#define H3_IRQ_GPU_PMU1			(153)
#define H3_IRQ_GPU_PMU2			(154)
#define H3_IRQ_GPU_PMU3			(155)
#define H3_IRQ_GPU_AXI			(156)

#define H3_IRQ_GPIOA0			(160)
#define H3_IRQ_GPIOA1			(161)
#define H3_IRQ_GPIOA2			(162)
#define H3_IRQ_GPIOA3			(163)
#define H3_IRQ_GPIOA4			(164)
#define H3_IRQ_GPIOA5			(165)
#define H3_IRQ_GPIOA6			(166)
#define H3_IRQ_GPIOA7			(167)
#define H3_IRQ_GPIOA8			(168)
#define H3_IRQ_GPIOA9			(169)
#define H3_IRQ_GPIOA10			(170)
#define H3_IRQ_GPIOA11			(171)
#define H3_IRQ_GPIOA12			(172)
#define H3_IRQ_GPIOA13			(173)
#define H3_IRQ_GPIOA14			(174)
#define H3_IRQ_GPIOA15			(175)
#define H3_IRQ_GPIOA16			(176)
#define H3_IRQ_GPIOA17			(177)
#define H3_IRQ_GPIOA18			(178)
#define H3_IRQ_GPIOA19			(179)
#define H3_IRQ_GPIOA20			(180)
#define H3_IRQ_GPIOA21			(181)

#define H3_IRQ_GPIOG0			(192)
#define H3_IRQ_GPIOG1			(193)
#define H3_IRQ_GPIOG2			(194)
#define H3_IRQ_GPIOG3			(195)
#define H3_IRQ_GPIOG4			(196)
#define H3_IRQ_GPIOG5			(197)
#define H3_IRQ_GPIOG6			(198)
#define H3_IRQ_GPIOG7			(199)
#define H3_IRQ_GPIOG8			(200)
#define H3_IRQ_GPIOG9			(201)
#define H3_IRQ_GPIOG10			(202)
#define H3_IRQ_GPIOG11			(203)
#define H3_IRQ_GPIOG12			(204)
#define H3_IRQ_GPIOG13			(205)

#define H3_IRQ_GPIOL0			(224)
#define H3_IRQ_GPIOL1			(225)
#define H3_IRQ_GPIOL2			(226)
#define H3_IRQ_GPIOL3			(227)
#define H3_IRQ_GPIOL4			(228)
#define H3_IRQ_GPIOL5			(229)
#define H3_IRQ_GPIOL6			(230)
#define H3_IRQ_GPIOL7			(231)
#define H3_IRQ_GPIOL8			(232)
#define H3_IRQ_GPIOL9			(233)
#define H3_IRQ_GPIOL10			(234)
#define H3_IRQ_GPIOL11			(235)

#ifdef __cplusplus
}
#endif

#endif /* __H3_IRQ_H__ */
