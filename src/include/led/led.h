#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

#define	LED_BRIGHTNESS_OFF		(0)
#define	LED_BRIGHTNESS_HALF		(128)
#define	LED_BRIGHTNESS_FULL		(255)

/*
 * the struct of led.
 */
struct led
{
	/* the led name */
	const char * name;

	/* initialize the led */
	void (*init)(void);

	/* set led's brightness */
	void (*set)(u8_t brightness);
};

#ifdef __cplusplus
}
#endif

#endif /* __LED_H__ */
