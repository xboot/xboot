#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum device_type_t {
	DEVICE_TYPE_AUDIO		= 0,
	DEVICE_TYPE_BATTERY		= 1,
	DEVICE_TYPE_BLOCK		= 2,
	DEVICE_TYPE_BUZZER		= 3,
	DEVICE_TYPE_CONSOLE		= 4,
	DEVICE_TYPE_DISK		= 5,
	DEVICE_TYPE_FB			= 6,
	DEVICE_TYPE_GPIOCHIP	= 7,
	DEVICE_TYPE_I2C			= 8,
	DEVICE_TYPE_INPUT		= 9,
	DEVICE_TYPE_IRQCHIP		= 10,
	DEVICE_TYPE_LED			= 11,
	DEVICE_TYPE_LEDTRIG		= 12,
	DEVICE_TYPE_PWM			= 13,
	DEVICE_TYPE_RNG			= 14,
	DEVICE_TYPE_RTC			= 15,
	DEVICE_TYPE_SPI			= 16,
	DEVICE_TYPE_UART		= 17,
	DEVICE_TYPE_VIBRATOR	= 18,
	DEVICE_TYPE_WATCHDOG	= 19,

	DEVICE_TYPE_MAX_COUNT	= 20,
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
	struct hlist_node node;

	char * name;
	enum device_type_t type;
	struct driver_t * driver;
	void * priv;
};

extern struct hlist_head __device_hash[DEVICE_TYPE_MAX_COUNT];
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
