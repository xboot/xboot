#ifndef __LED_GPIO_H__
#define __LED_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <led/led.h>

struct led_gpio_data_t
{
	const char * name;
	int gpio;
	int active_low;
};

#ifdef __cplusplus
}
#endif

#endif /* __LED_GPIO_H__ */
