#ifndef __R128_RESET_H__
#define __R128_RESET_H__

#ifdef __cplusplus
extern "C" {
#endif

#define R128_RESET_SPI0					(32 * 0 +  0)
#define R128_RESET_SPI1					(32 * 0 +  1)
#define R128_RESET_SDC0					(32 * 0 +  4)
#define R128_RESET_UART0				(32 * 0 +  6)
#define R128_RESET_UART1				(32 * 0 +  7)
#define R128_RESET_UART2				(32 * 0 +  8)
#define R128_RESET_TWI0					(32 * 0 + 10)
#define R128_RESET_TWI1					(32 * 0 + 11)
#define R128_RESET_PWM					(32 * 0 + 14)
#define R128_RESET_IRTX					(32 * 0 + 15)
#define R128_RESET_IRRX					(32 * 0 + 16)
#define R128_RESET_LSPSRAM				(32 * 0 + 20)
#define R128_RESET_HSPSRAM				(32 * 0 + 21)
#define R128_RESET_FLASH_CTL			(32 * 0 + 22)
#define R128_RESET_FLASH_ENC			(32 * 0 + 23)
#define R128_RESET_USB_PHY				(32 * 0 + 25)
#define R128_RESET_SMCARD				(32 * 0 + 26)
#define R128_RESET_USB_OTG				(32 * 0 + 27)
#define R128_RESET_LEDC					(32 * 0 + 28)
#define R128_RESET_CSI_JPE				(32 * 0 + 29)
#define R128_RESET_USB_OHCI				(32 * 0 + 30)
#define R128_RESET_USB_EHCI				(32 * 0 + 31)

#define R128_RESET_I2S					(32 * 1 +  1)
#define R128_RESET_OWA					(32 * 1 +  2)
#define R128_RESET_DMA0					(32 * 1 +  6)
#define R128_RESET_DMA1					(32 * 1 +  7)
#define R128_RESET_SPINLOCK				(32 * 1 + 10)
#define R128_RESET_TIMER				(32 * 1 + 11)
#define R128_RESET_SS					(32 * 1 + 12)
#define R128_RESET_SPC					(32 * 1 + 13)
#define R128_RESET_TRNG					(32 * 1 + 14)
#define R128_RESET_WLAN_CTRL			(32 * 1 + 20)
#define R128_RESET_BT_CORE				(32 * 1 + 21)
#define R128_RESET_LCD					(32 * 1 + 24)
#define R128_RESET_DISPLAY				(32 * 1 + 25)
#define R128_RESET_DE					(32 * 1 + 26)
#define R128_RESET_G2D					(32 * 1 + 27)

#define R128_RESET_CPU_WDG				(32 * 2 +  0)
#define R128_RESET_CPU_MSGBOX			(32 * 2 +  1)
#define R128_RESET_CPU_CFG				(32 * 2 +  2)
#define R128_RESET_DSP_CORE				(32 * 2 +  8)
#define R128_RESET_DSP_WDG				(32 * 2 +  9)
#define R128_RESET_DSP_MSGBOX			(32 * 2 + 10)
#define R128_RESET_DSP_CFG				(32 * 2 + 11)
#define R128_RESET_DSP_TZMA				(32 * 2 + 12)
#define R128_RESET_DSP_INTC				(32 * 2 + 13)
#define R128_RESET_DSP_DEBUG			(32 * 2 + 14)
#define R128_RESET_RISCV_CORE			(32 * 2 + 16)
#define R128_RESET_RISCV_WDG			(32 * 2 + 17)
#define R128_RESET_RISCV_MSGBOX			(32 * 2 + 18)
#define R128_RESET_RISCV_CFG			(32 * 2 + 19)
#define R128_RESET_RISCV_TIMESTAMP		(32 * 2 + 20)
#define R128_RESET_RISCV_SYS_APB_SOFT	(32 * 2 + 21)

#define R128_RESET_LPUART0				(32 * 3 +  0)
#define R128_RESET_LPUART1				(32 * 3 +  1)
#define R128_RESET_GPADC				(32 * 3 +  2)
#define R128_RESET_DMIC					(32 * 3 +  3)
#define R128_RESET_MAD					(32 * 3 +  4)
#define R128_RESET_CODEC_ADC			(32 * 3 +  5)
#define R128_RESET_AON_TIMER			(32 * 3 +  6)
#define R128_RESET_LPSD					(32 * 3 +  7)
#define R128_RESET_RCCAL				(32 * 3 +  8)
#define R128_RESET_RFAS					(32 * 3 +  9)
#define R128_RESET_CODEC_DAC			(32 * 3 + 10)
#define R128_RESET_WLAN					(32 * 3 + 12)
#define R128_RESET_WLAN_CONN			(32 * 3 + 13)
#define R128_RESET_MADCFG				(32 * 3 + 15)
#define R128_RESET_BLE_RTC				(32 * 3 + 16)

#define R128_RESET_BLE_48M				(32 * 4 + 16)
#define R128_RESET_BLE_32M				(32 * 4 + 17)

#define R128_RESET_IS_PWRON				(32 * 5 +  0)
#define R128_RESET_IS_PMU				(32 * 5 +  1)
#define R128_RESET_IS_WATCHDOG_ALL		(32 * 5 +  8)

#define R128_RESET_RCO_CALIB			(32 * 6 + 28)

#ifdef __cplusplus
}
#endif

#endif /* __R128_RESET_H__ */
