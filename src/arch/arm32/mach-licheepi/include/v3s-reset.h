#ifndef __V3S_RESET_H__
#define __V3S_RESET_H__

#ifdef __cplusplus
extern "C" {
#endif

#define V3S_RESET_CE			(5)
#define V3S_RESET_DMA			(6)
#define V3S_RESET_SD0			(8)
#define V3S_RESET_SD1			(9)
#define V3S_RESET_SD2			(10)
#define V3S_RESET_SDRAM			(14)
#define V3S_RESET_EMAC			(17)
#define V3S_RESET_HSTIMER		(19)
#define V3S_RESET_SPI0			(20)
#define V3S_RESET_OTG_DEVICE	(24)
#define V3S_RESET_OTG_EHCI		(26)
#define V3S_RESET_OTG_OHCI		(29)
#define V3S_RESET_VE			(32)
#define V3S_RESET_TCON			(36)
#define V3S_RESET_CSI			(40)
#define V3S_RESET_DE			(44)
#define V3S_RESET_DBGSYS		(63)
#define V3S_RESET_EPHY			(66)
#define V3S_RESET_AC_DIGITAL	(96)
#define V3S_RESET_I2C0			(128)
#define V3S_RESET_I2C1			(129)
#define V3S_RESET_UART0			(144)
#define V3S_RESET_UART1			(145)
#define V3S_RESET_UART2			(146)

#ifdef __cplusplus
}
#endif

#endif /* __V3S_RESET_H__ */
