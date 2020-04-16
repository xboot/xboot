#ifndef __R328_RESET_H__
#define __R328_RESET_H__

#ifdef __cplusplus
extern "C" {
#endif

#define R328_RESET_CE			(32 *  0 + 16)
#define R328_RESET_DMA			(32 *  1 + 16)
#define R328_RESET_HSTIMER		(32 *  2 + 16)
#define R328_RESET_DBGSYS		(32 *  3 + 16)
#define R328_RESET_PSI			(32 *  4 + 16)
#define R328_RESET_PWM			(32 *  5 + 16)
#define R328_RESET_DRAM			(32 *  6 + 16)
#define R328_RESET_SMHC			(32 *  7 + 17)
#define R328_RESET_UART0		(32 *  8 + 16)
#define R328_RESET_UART1		(32 *  8 + 17)
#define R328_RESET_UART2		(32 *  8 + 18)
#define R328_RESET_UART3		(32 *  8 + 19)
#define R328_RESET_TWI0			(32 *  9 + 16)
#define R328_RESET_TWI1			(32 *  9 + 17)
#define R328_RESET_TWI2			(32 *  9 + 18)
#define R328_RESET_SPI0			(32 * 10 + 16)
#define R328_RESET_SPI1			(32 * 10 + 17)
#define R328_RESET_GPADC		(32 * 11 + 16)
#define R328_RESET_THS			(32 * 12 + 16)
#define R328_RESET_I2S0			(32 * 13 + 16)
#define R328_RESET_I2S1			(32 * 13 + 17)
#define R328_RESET_I2S2			(32 * 13 + 18)
#define R328_RESET_OWA			(32 * 14 + 16)
#define R328_RESET_DMIC			(32 * 15 + 16)
#define R328_RESET_CODEC		(32 * 16 + 16)
#define R328_RESET_USB_OHCI		(32 * 17 + 16)
#define R328_RESET_USB_EHCI		(32 * 17 + 20)
#define R328_RESET_USB_OTG		(32 * 17 + 24)
#define R328_RESET_VAD			(32 * 18 + 16)
#define R328_RESET_VAD_AD		(32 * 18 + 17)
#define R328_RESET_VAD_CFG		(32 * 18 + 18)
#define R328_RESET_LPSD			(32 * 19 + 16)
#define R328_RESET_LEDC			(32 * 20 + 16)

#ifdef __cplusplus
}
#endif

#endif /* __R328_RESET_H__ */
