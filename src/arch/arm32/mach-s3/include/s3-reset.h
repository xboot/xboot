#ifndef __S3_RESET_H__
#define __S3_RESET_H__

#ifdef __cplusplus
extern "C" {
#endif

#define S3_RESET_CE				(5)
#define S3_RESET_DMA			(6)
#define S3_RESET_SD0			(8)
#define S3_RESET_SD1			(9)
#define S3_RESET_SD2			(10)
#define S3_RESET_SDRAM			(14)
#define S3_RESET_EMAC			(17)
#define S3_RESET_HSTIMER		(19)
#define S3_RESET_SPI0			(20)
#define S3_RESET_OTG_DEVICE		(24)
#define S3_RESET_OTG_EHCI		(26)
#define S3_RESET_OTG_OHCI		(29)
#define S3_RESET_VE				(32)
#define S3_RESET_TCON			(36)
#define S3_RESET_CSI			(40)
#define S3_RESET_DE				(44)
#define S3_RESET_DBGSYS			(63)
#define S3_RESET_EPHY			(66)
#define S3_RESET_AC_DIGITAL		(96)
#define S3_RESET_I2C0			(128)
#define S3_RESET_I2C1			(129)
#define S3_RESET_UART0			(144)
#define S3_RESET_UART1			(145)
#define S3_RESET_UART2			(146)

#ifdef __cplusplus
}
#endif

#endif /* __S3_RESET_H__ */
