#ifndef __BCM2836_MBOX_H__
#define __BCM2836_MBOX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Mbox videocore */
int bcm2836_mbox_vc_get_firmware_revison(void);

/* Mbox hardware */
int bcm2836_mbox_hardware_get_model(void);
int bcm2836_mbox_hardware_get_revison(void);
int bcm2836_mbox_hardware_get_mac_address(uint8_t * mac);
int bcm2836_mbox_hardware_get_serial(uint64_t * sn);
int bcm2836_mbox_hardware_get_arm_memory(uint32_t * base, uint32_t * size);
int bcm2836_mbox_hardware_get_vc_memory(uint32_t * base, uint32_t * size);

/* Mbox clock */
enum {
	MBOX_CLOCK_ID_EMMC 		= 1,
	MBOX_CLOCK_ID_UART 		= 2,
	MBOX_CLOCK_ID_ARM 		= 3,
	MBOX_CLOCK_ID_CORE 		= 4,
	MBOX_CLOCK_ID_V3D 		= 5,
	MBOX_CLOCK_ID_H264 		= 6,
	MBOX_CLOCK_ID_ISP 		= 7,
	MBOX_CLOCK_ID_SDRAM		= 8,
	MBOX_CLOCK_ID_PIXEL 	= 9,
	MBOX_CLOCK_ID_PWM 		= 10,
};
int bcm2836_mbox_clock_get_turbo(void);
int bcm2836_mbox_clock_set_turbo(int level);
int bcm2836_mbox_clock_get_state(int id);
int bcm2836_mbox_clock_set_state(int id, int state);
int bcm2836_mbox_clock_get_rate(int id);
int bcm2836_mbox_clock_set_rate(int id, int rate);
int bcm2836_mbox_clock_get_max_rate(int id);
int bcm2836_mbox_clock_get_min_rate(int id);

/* Mbox power */
enum {
	MBOX_POWER_ID_SDCARD	= 0,
	MBOX_POWER_ID_UART0		= 1,
	MBOX_POWER_ID_UART1		= 2,
	MBOX_POWER_ID_USBHCD	= 3,
	MBOX_POWER_ID_I2C0		= 4,
	MBOX_POWER_ID_I2C1		= 5,
	MBOX_POWER_ID_I2C2		= 6,
	MBOX_POWER_ID_SPI		= 7,
	MBOX_POWER_ID_CCP2TX	= 8,
};
int bcm2836_mbox_power_get_state(int id);
int bcm2836_mbox_power_set_state(int id, int state);

/* Mbox temperature */
int bcm2836_mbox_temp_get(void);
int bcm2836_mbox_temp_get_max(void);

/* Mbox framebuffer */
uint32_t bcm2836_mbox_fb_get_gpiovirt(void);
void * bcm2836_mbox_fb_alloc(int width, int height, int bpp, int nrender);
int bcm2836_mbox_fb_present(int xoffset, int yoffset);

#ifdef __cplusplus
}
#endif

#endif /* __BCM2836_MBOX_H__ */
