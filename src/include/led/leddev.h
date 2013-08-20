#ifndef __LEDDEV_H__
#define __LEDDEV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <led/led.h>

struct led_data_t
{
	void (*init)(struct led_data_t * dat);
	void (*set)(struct led_data_t * dat, int brightness);
};

#ifdef __cplusplus
}
#endif

#endif /* __LEDDEV_H__ */
