#ifndef __IR_GPIO_H__
#define __IR_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/input.h>
#include <input/keyboard.h>

struct ir_gpio_data_t
{
	int gpio;
	int active_low;
};

#ifdef __cplusplus
}
#endif

#endif /* __IR_GPIO_H__ */
