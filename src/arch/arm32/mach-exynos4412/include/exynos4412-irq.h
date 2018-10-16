#ifndef __EXYNOS4412_IRQ_H__
#define __EXYNOS4412_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EXYNOS4412_IRQ_INTG0			(32)
#define EXYNOS4412_IRQ_INTG1			(33)
#define EXYNOS4412_IRQ_INTG2			(34)
#define EXYNOS4412_IRQ_INTG3			(35)
#define EXYNOS4412_IRQ_INTG4			(36)
#define EXYNOS4412_IRQ_INTG5			(37)
#define EXYNOS4412_IRQ_INTG6			(38)
#define EXYNOS4412_IRQ_INTG7			(39)
#define EXYNOS4412_IRQ_INTG8			(40)
#define EXYNOS4412_IRQ_INTG9			(41)
#define EXYNOS4412_IRQ_INTG10			(42)
#define EXYNOS4412_IRQ_INTG11			(43)
#define EXYNOS4412_IRQ_INTG12			(44)
#define EXYNOS4412_IRQ_INTG13			(45)
#define EXYNOS4412_IRQ_INTG14			(46)
#define EXYNOS4412_IRQ_INTG15			(47)
#define EXYNOS4412_IRQ_EINT0			(48)
#define EXYNOS4412_IRQ_EINT1			(49)
#define EXYNOS4412_IRQ_EINT2			(50)
#define EXYNOS4412_IRQ_EINT3			(51)
#define EXYNOS4412_IRQ_EINT4			(52)
#define EXYNOS4412_IRQ_EINT5			(53)
#define EXYNOS4412_IRQ_EINT6			(54)
#define EXYNOS4412_IRQ_EINT7			(55)
#define EXYNOS4412_IRQ_EINT8			(56)
#define EXYNOS4412_IRQ_EINT9			(57)
#define EXYNOS4412_IRQ_EINT10			(58)
#define EXYNOS4412_IRQ_EINT11			(59)
#define EXYNOS4412_IRQ_EINT12			(60)
#define EXYNOS4412_IRQ_EINT13			(61)
#define EXYNOS4412_IRQ_EINT14			(62)
#define EXYNOS4412_IRQ_EINT15			(63)
#define EXYNOS4412_IRQ_EINT16_31		(64)
#define EXYNOS4412_IRQ_C2C_SSCM0		(65)
#define EXYNOS4412_IRQ_MDMA				(66)
#define EXYNOS4412_IRQ_PMDA0			(67)
#define EXYNOS4412_IRQ_PMDA1			(68)
#define EXYNOS4412_IRQ_TIMER0			(69)
#define EXYNOS4412_IRQ_TIMER1			(70)
#define EXYNOS4412_IRQ_TIMER2			(71)
#define EXYNOS4412_IRQ_TIMER3			(72)
#define EXYNOS4412_IRQ_TIMER4			(73)
#define EXYNOS4412_IRQ_INTG19			(74)
#define EXYNOS4412_IRQ_WDT				(75)
#define EXYNOS4412_IRQ_RTC_ALARM		(76)
#define EXYNOS4412_IRQ_RTC_TIC			(77)
#define EXYNOS4412_IRQ_GPIO_RT			(78)
#define EXYNOS4412_IRQ_GPIO_LB			(79)
#define EXYNOS4412_IRQ_INTG18			(80)
#define EXYNOS4412_IRQ_IEM_APC			(81)
#define EXYNOS4412_IRQ_IEM_IEC			(82)
#define EXYNOS4412_IRQ_NFC				(83)
#define EXYNOS4412_IRQ_UART0			(84)
#define EXYNOS4412_IRQ_UART1			(85)
#define EXYNOS4412_IRQ_UART2			(86)
#define EXYNOS4412_IRQ_UART3			(87)
#define EXYNOS4412_IRQ_G0_IRQ			(89)
#define EXYNOS4412_IRQ_I2C0				(90)
#define EXYNOS4412_IRQ_I2C1				(91)
#define EXYNOS4412_IRQ_I2C2				(92)
#define EXYNOS4412_IRQ_I2C3				(93)
#define EXYNOS4412_IRQ_I2C4				(94)
#define EXYNOS4412_IRQ_I2C5				(95)
#define EXYNOS4412_IRQ_I2C6				(96)
#define EXYNOS4412_IRQ_I2C7				(97)
#define EXYNOS4412_IRQ_SPI0				(98)
#define EXYNOS4412_IRQ_SPI1				(99)
#define EXYNOS4412_IRQ_SPI2				(100)
#define EXYNOS4412_IRQ_G1_IRQ			(101)
#define EXYNOS4412_IRQ_UHOST			(102)
#define EXYNOS4412_IRQ_HSOTG			(103)
#define EXYNOS4412_IRQ_GPIO_C2C			(104)
#define EXYNOS4412_IRQ_HSMMC0			(105)
#define EXYNOS4412_IRQ_HSMMC1			(106)
#define EXYNOS4412_IRQ_HSMMC2			(107)
#define EXYNOS4412_IRQ_HSMMC3			(108)
#define EXYNOS4412_IRQ_SDMMC			(109)
#define EXYNOS4412_IRQ_MIPI_CSI_4LANE	(110)
#define EXYNOS4412_IRQ_MIPI_DSI_4LANE	(111)
#define EXYNOS4412_IRQ_MIPI_CSI_2LANE	(112)
#define EXYNOS4412_IRQ_ROTATOR			(115)
#define EXYNOS4412_IRQ_FIMC0			(116)
#define EXYNOS4412_IRQ_FIMC1			(117)
#define EXYNOS4412_IRQ_FIMC2			(118)
#define EXYNOS4412_IRQ_FIMC3			(119)
#define EXYNOS4412_IRQ_JPEG				(120)
#define EXYNOS4412_IRQ_G2D				(121)
#define EXYNOS4412_IRQ_ISP0				(122)
#define EXYNOS4412_IRQ_MIXER			(123)
#define EXYNOS4412_IRQ_HDMI				(124)
#define EXYNOS4412_IRQ_HDMI_I2C			(125)
#define EXYNOS4412_IRQ_MFC				(126)
#define EXYNOS4412_IRQ_ISP1				(127)
#define EXYNOS4412_IRQ_AUDIO_SS			(128)
#define EXYNOS4412_IRQ_I2S0				(129)
#define EXYNOS4412_IRQ_I2S1				(130)
#define EXYNOS4412_IRQ_I2S2				(131)
#define EXYNOS4412_IRQ_AC97				(132)
#define EXYNOS4412_IRQ_PCM0				(133)
#define EXYNOS4412_IRQ_PCM1				(134)
#define EXYNOS4412_IRQ_PCM2				(135)
#define EXYNOS4412_IRQ_SPDDIF			(136)
#define EXYNOS4412_IRQ_FIMC_LITE0		(137)
#define EXYNOS4412_IRQ_FIMC_LITE1		(138)
#define EXYNOS4412_IRQ_INTG16			(139)
#define EXYNOS4412_IRQ_INTG17			(140)
#define EXYNOS4412_IRQ_KEYPAD			(141)
#define EXYNOS4412_IRQ_PMU				(142)
#define EXYNOS4412_IRQ_GPS				(143)
#define EXYNOS4412_IRQ_SSS				(144)
#define EXYNOS4412_IRQ_SLIMBUS			(145)
#define EXYNOS4412_IRQ_CEC				(146)
#define EXYNOS4412_IRQ_TSI				(147)
#define EXYNOS4412_IRQ_C2C_SSCM1		(148)
#define EXYNOS4412_IRQ_G3D_IRQPMU		(149)
#define EXYNOS4412_IRQ_G3D_IRQPPMMU0	(150)
#define EXYNOS4412_IRQ_G3D_IRQPPMMU1	(151)
#define EXYNOS4412_IRQ_G3D_IRQPPMMU2	(152)
#define EXYNOS4412_IRQ_G3D_IRQPPMMU3	(153)
#define EXYNOS4412_IRQ_G3D_IRQGPMMU		(154)
#define EXYNOS4412_IRQ_G3D_IRQPP0		(155)
#define EXYNOS4412_IRQ_G3D_IRQPP1		(156)
#define EXYNOS4412_IRQ_G3D_IRQPP2		(157)
#define EXYNOS4412_IRQ_G3D_IRQPP3		(158)
#define EXYNOS4412_IRQ_G3D_IRQGP		(159)

#ifdef __cplusplus
}
#endif

#endif /* __EXYNOS4412_IRQ_H__ */