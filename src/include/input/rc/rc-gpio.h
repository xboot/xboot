#ifndef __RC_GPIO_H__
#define __RC_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/rc/rc-core.h>

struct rc_gpio_data_t
{
	struct rc_map_t * map;
	int size;
	int gpio;
	int active_low;
};

#ifdef __cplusplus
}
#endif

#endif /* __RC_GPIO_H__ */
