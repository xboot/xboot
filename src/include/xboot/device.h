#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum device_type_t {
	DEVICE_TYPE_RTC,
	DEVICE_TYPE_LED,
	DEVICE_TYPE_LED_TRIGGER,
	DEVICE_TYPE_FRAMEBUFFER,
	DEVICE_TYPE_INPUT,
	DEVICE_TYPE_BLOCK,
};

struct device_t
{
	/* Kobj binding */
	struct kobj_t * kobj;

	/* Device name */
	char * name;

	/* Device type */
	enum device_type_t type;

	/* Suspend device */
	void (*suspend)(struct device_t * dev);

	/* Resume device */
	void (*resume)(struct device_t * dev);

	/* Device driver */
	void * driver;
};

struct device_list_t
{
	struct device_t * device;
	struct list_head entry;
};

extern struct device_list_t __device_list;

struct device_t * search_device(const char * name);
struct device_t * search_device_with_type(const char * name, enum device_type_t type);
struct device_t * search_first_device_with_type(enum device_type_t type);
bool_t register_device(struct device_t * dev);
bool_t unregister_device(struct device_t * dev);
void suspend_device(const char * name);
void resume_device(const char * name);
void suspend_all_device(void);
void resume_all_device(void);

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_H__ */
