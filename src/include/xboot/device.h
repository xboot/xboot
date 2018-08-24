#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum device_type_t {
	DEVICE_TYPE_ADC				= 0,
	DEVICE_TYPE_AUDIO			= 1,
	DEVICE_TYPE_BATTERY			= 2,
	DEVICE_TYPE_BLOCK			= 3,
	DEVICE_TYPE_BUZZER			= 4,
	DEVICE_TYPE_CLK				= 5,
	DEVICE_TYPE_CLOCKEVENT		= 6,
	DEVICE_TYPE_CLOCKSOURCE		= 7,
	DEVICE_TYPE_COMPASS			= 8,
	DEVICE_TYPE_CONSOLE			= 9,
	DEVICE_TYPE_DAC				= 10,
	DEVICE_TYPE_DISK			= 11,
	DEVICE_TYPE_FRAMEBUFFER		= 12,
	DEVICE_TYPE_GMETER			= 13,
	DEVICE_TYPE_GPIOCHIP		= 14,
	DEVICE_TYPE_GYROSCOPE		= 15,
	DEVICE_TYPE_HYGROMETER		= 16,
	DEVICE_TYPE_I2C				= 17,
	DEVICE_TYPE_INPUT			= 18,
	DEVICE_TYPE_IRQCHIP			= 19,
	DEVICE_TYPE_LASERSCAN		= 20,
	DEVICE_TYPE_LED				= 21,
	DEVICE_TYPE_LEDSTRIP		= 22,
	DEVICE_TYPE_LEDTRIGGER		= 23,
	DEVICE_TYPE_LIGHT			= 24,
	DEVICE_TYPE_MOTOR			= 25,
	DEVICE_TYPE_NVMEM			= 26,
	DEVICE_TYPE_PRESSURE		= 27,
	DEVICE_TYPE_PROXIMITY		= 28,
	DEVICE_TYPE_PWM				= 29,
	DEVICE_TYPE_REGULATOR		= 30,
	DEVICE_TYPE_RESETCHIP		= 31,
	DEVICE_TYPE_RNG				= 32,
	DEVICE_TYPE_RTC				= 33,
	DEVICE_TYPE_SDHCI			= 34,
	DEVICE_TYPE_SERVO			= 35,
	DEVICE_TYPE_SPI				= 36,
	DEVICE_TYPE_STEPPER			= 37,
	DEVICE_TYPE_THERMOMETER		= 38,
	DEVICE_TYPE_UART			= 39,
	DEVICE_TYPE_VIBRATOR		= 40,
	DEVICE_TYPE_WATCHDOG		= 41,

	DEVICE_TYPE_MAX_COUNT		= 42,
};

enum {
	NOTIFIER_DEVICE_ADD,
	NOTIFIER_DEVICE_REMOVE,
	NOTIFIER_DEVICE_SUSPEND,
	NOTIFIER_DEVICE_RESUME,
};

struct driver_t;

struct device_t
{
	struct kobj_t * kobj;
	struct list_head list;
	struct list_head head;
	struct hlist_node node;

	char * name;
	enum device_type_t type;
	struct driver_t * driver;
	void * priv;
};

extern struct list_head __device_list;
extern struct list_head __device_head[DEVICE_TYPE_MAX_COUNT];

char * alloc_device_name(const char * name, int id);
void free_device_name(char * name);
struct device_t * search_device(const char * name, enum device_type_t type);
struct device_t * search_first_device(enum device_type_t type);
bool_t register_device(struct device_t * dev);
bool_t unregister_device(struct device_t * dev);
bool_t register_device_notifier(struct notifier_t * n);
bool_t unregister_device_notifier(struct notifier_t * n);
void suspend_device(struct device_t * dev);
void resume_device(struct device_t * dev);
void remove_device(struct device_t * dev);

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_H__ */
