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

struct gpio_t
{
	struct kobj_t * kobj;
	const char * name;
	int base;
	int ngpio;

	void (*set_cfg)(struct gpio_t * gpio, int offset, int cfg);
	int  (*get_cfg)(struct gpio_t * gpio, int offset);
	void (*set_pull)(struct gpio_t * gpio, int offset, enum gpio_pull_t pull);
	enum gpio_pull_t (*get_pull)(struct gpio_t * gpio, int offset);
	void (*set_drv)(struct gpio_t * gpio, int offset, enum gpio_drv_t drv);
	enum gpio_drv_t (*get_drv)(struct gpio_t * gpio, int offset);
	void (*set_rate)(struct gpio_t * gpio, int offset, enum gpio_rate_t rate);
	enum gpio_rate_t (*get_rate)(struct gpio_t * gpio, int offset);
	void (*set_dir)(struct gpio_t * gpio, int offset, enum gpio_direction_t dir);
	enum gpio_direction_t (*get_dir)(struct gpio_t * gpio, int offset);
	void (*set_value)(struct gpio_t * gpio, int offset, int value);
	int  (*get_value)(struct gpio_t * gpio, int offset);

	void * priv;
};

struct gpio_t * search_gpio(const char * name);
struct gpio_t * search_gpio_with_no(int no);
bool_t register_gpio(struct gpio_t * gpio);
bool_t unregister_gpio(struct gpio_t * gpio);

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
