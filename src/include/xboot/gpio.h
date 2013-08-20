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

struct gpio_t
{
	const char * name;
	int base;
	int ngpio;

	void (*pull)(struct gpio_t * gpio, int offset, enum gpio_pull_t mode);
	void (*input)(struct gpio_t * gpio, int offset);
	void (*output)(struct gpio_t * gpio, int offset, int value);
	void (*set)(struct gpio_t * gpio, int offset, int value);
	int (*get)(struct gpio_t * gpio, int offset);
};

bool_t register_gpio(struct gpio_t * gpio);
bool_t unregister_gpio(struct gpio_t * gpio);

void gpio_set_pull(int no, enum gpio_pull_t mode);
void gpio_direction_input(int no);
void gpio_direction_output(int no, int value);
void gpio_set_value(int no, int value);
int gpio_get_value(int no);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H__ */
