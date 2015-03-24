#ifndef __BUZZER_GPIO_H__
#define __BUZZER_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <buzzer/buzzer.h>

struct buzzer_gpio_data_t
{
	int gpio;
	int active_low;
};

#ifdef __cplusplus
}
#endif

#endif /* __BUZZER_GPIO_H__ */
