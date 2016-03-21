#ifndef __VIBRATOR_GPIO_H__
#define __VIBRATOR_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <vibrator/vibrator.h>

struct vibrator_gpio_data_t
{
	int gpio;
	int active_low;
};

#ifdef __cplusplus
}
#endif

#endif /* __VIBRATOR_GPIO_H__ */
