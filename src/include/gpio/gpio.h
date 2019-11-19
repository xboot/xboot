#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum gpio_pull_t {
	GPIO_PULL_UP			= 0,
	GPIO_PULL_DOWN			= 1,
	GPIO_PULL_NONE			= 2,
};

enum gpio_drv_t {
	GPIO_DRV_WEAK			= 0,
	GPIO_DRV_WEAKER			= 1,
	GPIO_DRV_STRONGER		= 2,
	GPIO_DRV_STRONG			= 3,
};

enum gpio_rate_t {
	GPIO_RATE_SLOW			= 0,
	GPIO_RATE_FAST			= 1,
};

enum gpio_direction_t {
	GPIO_DIRECTION_INPUT	= 0,
	GPIO_DIRECTION_OUTPUT	= 1,
};

struct gpiochip_t
{
	char * name;
	int base;
	int ngpio;

	void (*set_cfg)(struct gpiochip_t * chip, int offset, int cfg);
	int  (*get_cfg)(struct gpiochip_t * chip, int offset);
	void (*set_pull)(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull);
	enum gpio_pull_t (*get_pull)(struct gpiochip_t * chip, int offset);
	void (*set_drv)(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv);
	enum gpio_drv_t (*get_drv)(struct gpiochip_t * chip, int offset);
	void (*set_rate)(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate);
	enum gpio_rate_t (*get_rate)(struct gpiochip_t * chip, int offset);
	void (*set_dir)(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir);
	enum gpio_direction_t (*get_dir)(struct gpiochip_t * chip, int offset);
	void (*set_value)(struct gpiochip_t * chip, int offset, int value);
	int  (*get_value)(struct gpiochip_t * chip, int offset);
	int  (*to_irq)(struct gpiochip_t * chip, int offset);

	void * priv;
};

struct gpiochip_t * search_gpiochip(int gpio);
struct device_t * register_gpiochip(struct gpiochip_t * chip, struct driver_t * drv);
void unregister_gpiochip(struct gpiochip_t * chip);

int gpio_is_valid(int gpio);
void gpio_set_cfg(int gpio, int cfg);
int gpio_get_cfg(int gpio);
void gpio_set_pull(int gpio, enum gpio_pull_t pull);
enum gpio_pull_t gpio_get_pull(int gpio);
void gpio_set_drv(int gpio, enum gpio_drv_t drv);
enum gpio_drv_t gpio_get_drv(int gpio);
void gpio_set_rate(int gpio, enum gpio_rate_t rate);
enum gpio_rate_t gpio_get_rate(int gpio);
void gpio_set_direction(int gpio, enum gpio_direction_t dir);
enum gpio_direction_t gpio_get_direction(int gpio);
void gpio_set_value(int gpio, int value);
int gpio_get_value(int gpio);
void gpio_direction_output(int gpio, int value);
int gpio_direction_input(int gpio);
int gpio_to_irq(int gpio);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H__ */
