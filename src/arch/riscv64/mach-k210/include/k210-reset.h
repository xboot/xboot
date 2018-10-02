#ifndef __K210_RESET_H__
#define __K210_RESET_H__

#ifdef __cplusplus
extern "C" {
#endif

#define K210_RESET_ROM		(0)
#define K210_RESET_DMA		(1)
#define K210_RESET_AI		(2)
#define K210_RESET_DVP		(3)
#define K210_RESET_FFT		(4)
#define K210_RESET_GPIO		(5)
#define K210_RESET_SPI0		(6)
#define K210_RESET_SPI1		(7)
#define K210_RESET_SPI2		(8)
#define K210_RESET_SPI3		(9)
#define K210_RESET_I2S0		(10)
#define K210_RESET_I2S1		(11)
#define K210_RESET_I2S2		(12)
#define K210_RESET_I2C0		(13)
#define K210_RESET_I2C1		(14)
#define K210_RESET_I2C2		(15)
#define K210_RESET_UART1	(16)
#define K210_RESET_UART2	(17)
#define K210_RESET_UART3	(18)
#define K210_RESET_AES		(19)
#define K210_RESET_FPIOA	(20)
#define K210_RESET_TIMER0	(21)
#define K210_RESET_TIMER1	(22)
#define K210_RESET_TIMER2	(23)
#define K210_RESET_WDT0		(24)
#define K210_RESET_WDT1		(25)
#define K210_RESET_SHA		(26)
#define K210_RESET_RTC		(29)

#ifdef __cplusplus
}
#endif

#endif /* __K210_RESET_H__ */
