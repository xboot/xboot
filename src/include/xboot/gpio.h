#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum gpio_pull_t {
	GPIO_PULL_UP	= 0,
	GPIO_PULL_DOWN	= 1,
	GPIO_PULL_NONE	= 2,
};

enum gpio_drv_t {
	GPIO_DRV_LOW	= 0,
	GPIO_DRV_MEDIAN	= 1,
	GPIO_DRV_HIGH	= 2,
};

enum gpio_rate_t {
	GPIO_RATE_FAST	= 0,
	GPIO_RATE_SLOW	= 1,
};

struct gpio_t
{
	const char * name;
	int base;
	int ngpio;

	void (*cfg_pin)(struct gpio_t * gpio, int offset, int cfg);
	void (*set_pull)(struct gpio_t * gpio, int offset, enum gpio_pull_t pull);
	void (*set_drv)(struct gpio_t * gpio, int offset, enum gpio_drv_t drv);
	void (*set_rate)(struct gpio_t * gpio, int offset, enum gpio_rate_t rate);
	void (*direction_output)(struct gpio_t * gpio, int offset, int value);
	void (*direction_input)(struct gpio_t * gpio, int offset);
	void (*set_value)(struct gpio_t * gpio, int offset, int value);
	int  (*get_value)(struct gpio_t * gpio, int offset);

	/* Private data */
	void * priv;
};

bool_t register_gpio(struct gpio_t * gpio);
bool_t unregister_gpio(struct gpio_t * gpio);

int gpio_is_valid(int no);
void gpio_cfg_pin(int no, int cfg);
void gpio_set_pull(int no, enum gpio_pull_t pull);
void gpio_set_drv(int no, enum gpio_drv_t drv);
void gpio_set_rate(int no, enum gpio_rate_t rate);
void gpio_direction_output(int no, int value);
void gpio_direction_input(int no);
void gpio_set_value(int no, int value);
int gpio_get_value(int no);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H__ */
