#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum gpio_pull_t {
	GPIO_PULL_NONE			= 0,
	GPIO_PULL_UP			= 1,
	GPIO_PULL_DOWN			= 2,
};

enum gpio_drv_t {
	GPIO_DRV_NONE			= 0,
	GPIO_DRV_LOW			= 1,
	GPIO_DRV_MEDIAN			= 2,
	GPIO_DRV_HIGH			= 3,
};

enum gpio_rate_t {
	GPIO_RATE_NONE			= 0,
	GPIO_RATE_FAST			= 1,
	GPIO_RATE_SLOW			= 2,
};

enum gpio_direction_t {
	GPIO_DIRECTION_NONE		= 0,
	GPIO_DIRECTION_INPUT	= 1,
	GPIO_DIRECTION_OUTPUT	= 2,
};

struct gpiochip_t
{
	struct kobj_t * kobj;
	const char * name;
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

	void * priv;
};

struct gpiochip_t * search_gpiochip(const char * name);
bool_t register_gpiochip(struct gpiochip_t * chip);
bool_t unregister_gpiochip(struct gpiochip_t * chip);

int gpio_is_valid(int no);
void gpio_set_cfg(int no, int cfg);
int gpio_get_cfg(int no);
void gpio_set_pull(int no, enum gpio_pull_t pull);
enum gpio_pull_t gpio_get_pull(int no);
void gpio_set_drv(int no, enum gpio_drv_t drv);
enum gpio_drv_t gpio_get_drv(int no);
void gpio_set_rate(int no, enum gpio_rate_t rate);
enum gpio_rate_t gpio_get_rate(int no);
void gpio_set_direction(int no, enum gpio_direction_t dir);
enum gpio_direction_t gpio_get_direction(int no);
void gpio_set_value(int no, int value);
int gpio_get_value(int no);
void gpio_direction_output(int no, int value);
void gpio_direction_input(int no);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H__ */
