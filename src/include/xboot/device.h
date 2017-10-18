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
	DEVICE_TYPE_CONSOLE			= 8,
	DEVICE_TYPE_DAC				= 9,
	DEVICE_TYPE_DISK			= 10,
	DEVICE_TYPE_FB				= 11,
	DEVICE_TYPE_GMETER			= 12,
	DEVICE_TYPE_GPIOCHIP		= 13,
	DEVICE_TYPE_GYROSCOPE		= 14,
	DEVICE_TYPE_HYGROMETER		= 15,
	DEVICE_TYPE_I2C				= 16,
	DEVICE_TYPE_INPUT			= 17,
	DEVICE_TYPE_IRQCHIP			= 18,
	DEVICE_TYPE_LASERSCAN		= 19,
	DEVICE_TYPE_LED				= 20,
	DEVICE_TYPE_LEDTRIG			= 21,
	DEVICE_TYPE_LIGHT			= 22,
	DEVICE_TYPE_MAGNETOMETER	= 23,
	DEVICE_TYPE_NVMEM			= 24,
	DEVICE_TYPE_ORIENTATION		= 25,
	DEVICE_TYPE_PRESSURE		= 26,
	DEVICE_TYPE_PROXIMITY		= 27,
	DEVICE_TYPE_PWM				= 28,
	DEVICE_TYPE_REGULATOR		= 29,
	DEVICE_TYPE_RESETCHIP		= 30,
	DEVICE_TYPE_RNG				= 31,
	DEVICE_TYPE_RTC				= 32,
	DEVICE_TYPE_SDHCI			= 33,
	DEVICE_TYPE_SPI				= 34,
	DEVICE_TYPE_THERMOMETER		= 35,
	DEVICE_TYPE_UART			= 36,
	DEVICE_TYPE_VIBRATOR		= 37,
	DEVICE_TYPE_WATCHDOG		= 38,

	DEVICE_TYPE_MAX_COUNT		= 39,
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
