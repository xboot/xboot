#ifndef __BCM2836_MBOXOX_H__
#define __BCM2836_MBOXOX_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	MBOX_CH_POWER_MANAGEMENT	= 0,
	MBOX_CH_FRAMEBUFFER			= 1,
	MBOX_CH_VIRTUAL_UART		= 2,
	MBOX_CH_VCHIQ				= 3,
	MBOX_CH_LEDS				= 4,
	MBOX_CH_BUTTONS				= 5,
	MBOX_CH_TOUCHSCREEN			= 6,
	MBOX_CH_UNUSED				= 7,
	MBOX_CH_TAGS_ARM_TO_VC		= 8,
	MBOX_CH_TAGS_VC_TO_ARM		= 9,
};

void bcm2836_mbox_write(int channel, int value);
int bcm2836_mbox_read(int channel);

#ifdef __cplusplus
}
#endif

#endif /* __BCM2836_MBOXOX_H__ */
