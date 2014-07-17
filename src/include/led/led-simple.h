#ifndef __LED_SIMPLE_H__
#define __LED_SIMPLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <led/led.h>

struct led_simple_data_t
{
	void (*init)(struct led_simple_data_t * dat);
	void (*set)(struct led_simple_data_t * dat, int brightness);
};

#ifdef __cplusplus
}
#endif

#endif /* __LED_SIMPLE_H__ */
