#ifndef __KEY_GPIO_H__
#define __KEY_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/input.h>
#include <input/keyboard.h>

struct gpio_button_t {
	u32_t code;
	int gpio;
	int active_low;
};

struct key_gpio_data_t
{
	struct gpio_button_t * buttons;
	int nbutton;
};

#ifdef __cplusplus
}
#endif

#endif /* __KEY_GPIO_H__ */
