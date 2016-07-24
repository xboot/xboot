#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum device_type_t {
	DEVICE_TYPE_CONSOLE,
	DEVICE_TYPE_RNG,
	DEVICE_TYPE_RTC,
	DEVICE_TYPE_LED,
	DEVICE_TYPE_LEDTRIG,
	DEVICE_TYPE_BUZZER,
	DEVICE_TYPE_VIBRATOR,
	DEVICE_TYPE_FRAMEBUFFER,
	DEVICE_TYPE_AUDIO,
	DEVICE_TYPE_WATCHDOG,
	DEVICE_TYPE_BATTERY,
	DEVICE_TYPE_INPUT,
	DEVICE_TYPE_BLOCK,
	DEVICE_TYPE_DISK,
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
	char * name;

	enum device_type_t type;
	struct driver_t * driver;
	void * priv;
};

struct device_list_t
{
	struct device_t * device;
	struct list_head entry;
};

extern struct device_list_t __device_list;

char * dynamic_device_name(const char * name, int id);
struct device_t * search_device(const char * name);
struct device_t * search_device_with_type(const char * name, enum device_type_t type);
struct device_t * search_first_device_with_type(enum device_type_t type);
bool_t register_device(struct device_t * dev);
bool_t unregister_device(struct device_t * dev);
bool_t register_device_notifier(struct notifier_t * n);
bool_t unregister_device_notifier(struct notifier_t * n);
void suspend_device(const char * name);
void resume_device(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_H__ */
