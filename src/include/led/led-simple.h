#ifndef __LED_SIMPLE_H__
#define __LED_SIMPLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <led/led.h>

struct led_simple_data_t
{
	const char * name;
	void (*init)(struct led_simple_data_t * dat);
	void (*set)(struct led_simple_data_t * dat, u32_t color);
};

#ifdef __cplusplus
}
#endif

#endif /* __LED_SIMPLE_H__ */
