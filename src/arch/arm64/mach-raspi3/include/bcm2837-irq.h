#ifndef __BCM2837_IRQ_H__
#define __BCM2837_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * GPU interrupts
 */
#define BCM2837_IRQ_TIMER0			(0)
#define BCM2837_IRQ_TIMER1			(1)
#define BCM2837_IRQ_TIMER2			(2)
#define BCM2837_IRQ_TIMER3			(3)
#define BCM2837_IRQ_CODEC0			(4)
#define BCM2837_IRQ_CODEC1			(5)
#define BCM2837_IRQ_CODEC2			(6)
#define BCM2837_IRQ_JPEG			(7)
#define BCM2837_IRQ_ISP				(8)
#define BCM2837_IRQ_USB				(9)
#define BCM2837_IRQ_3D				(10)
#define BCM2837_IRQ_TRANSPOSER		(11)
#define BCM2837_IRQ_MULTICORESYNC0	(12)
#define BCM2837_IRQ_MULTICORESYNC1	(13)
#define BCM2837_IRQ_MULTICORESYNC2	(14)
#define BCM2837_IRQ_MULTICORESYNC3	(15)
#define BCM2837_IRQ_DMA0			(16)
#define BCM2837_IRQ_DMA1			(17)
#define BCM2837_IRQ_DMA2			(18)
#define BCM2837_IRQ_DMA3			(19)
#define BCM2837_IRQ_DMA4			(20)
#define BCM2837_IRQ_DMA5			(21)
#define BCM2837_IRQ_DMA6			(22)
#define BCM2837_IRQ_DMA7			(23)
#define BCM2837_IRQ_DMA8			(24)
#define BCM2837_IRQ_DMA9			(25)
#define BCM2837_IRQ_DMA10			(26)
#define BCM2837_IRQ_DMA11			(27)
#define BCM2837_IRQ_DMA12			(28)
#define BCM2837_IRQ_AUX				(29)
#define BCM2837_IRQ_ARM				(30)
#define BCM2837_IRQ_VPUDMA			(31)
#define BCM2837_IRQ_HOSTPORT		(32)
#define BCM2837_IRQ_VIDEOSCALER		(33)
#define BCM2837_IRQ_CCP2TX			(34)
#define BCM2837_IRQ_SDC				(35)
#define BCM2837_IRQ_DSI0			(36)
#define BCM2837_IRQ_AVE				(37)
#define BCM2837_IRQ_CAM0			(38)
#define BCM2837_IRQ_CAM1			(39)
#define BCM2837_IRQ_HDMI0			(40)
#define BCM2837_IRQ_HDMI1			(41)
#define BCM2837_IRQ_PIXELVALVE1		(42)
#define BCM2837_IRQ_I2CSPISLV		(43)
#define BCM2837_IRQ_DSI1			(44)
#define BCM2837_IRQ_PWA0			(45)
#define BCM2837_IRQ_PWA1			(46)
#define BCM2837_IRQ_CPR				(47)
#define BCM2837_IRQ_SMI				(48)
#define BCM2837_IRQ_GPIO0			(49)
#define BCM2837_IRQ_GPIO1			(50)
#define BCM2837_IRQ_GPIO2			(51)
#define BCM2837_IRQ_GPIO3			(52)
#define BCM2837_IRQ_I2C				(53)
#define BCM2837_IRQ_SPI				(54)
#define BCM2837_IRQ_I2SPCM			(55)
#define BCM2837_IRQ_SDIO			(56)
#define BCM2837_IRQ_UART			(57)
#define BCM2837_IRQ_SLIMBUS			(58)
#define BCM2837_IRQ_VEC				(59)
#define BCM2837_IRQ_CPG				(60)
#define BCM2837_IRQ_RNG				(61)
#define BCM2837_IRQ_ARASANSDIO		(62)
#define BCM2837_IRQ_AVSPMON			(63)

/*
 * ARM interrupts
 */
#define BCM2837_IRQ_ARM_TIMER		(64)
#define BCM2837_IRQ_ARM_MAILBOX		(65)
#define BCM2837_IRQ_ARM_DOORBELL_0	(66)
#define BCM2837_IRQ_ARM_DOORBELL_1	(67)
#define BCM2837_IRQ_VPU0_HALTED		(68)
#define BCM2837_IRQ_VPU1_HALTED		(69)
#define BCM2837_IRQ_ILLEGAL_TYPE0	(70)
#define BCM2837_IRQ_ILLEGAL_TYPE1	(71)

#ifdef __cplusplus
}
#endif

#endif /* __BCM2837_IRQ_H__ */
