#ifndef __DRIVER_H__
#define __DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct driver_t
{
	struct kobj_t * kobj;
	char * name;

	bool_t (*probe)(struct device_t * dev, void * dt);
	bool_t (*remove)(struct device_t * dev);
	bool_t (*shutdown)(struct device_t * dev);
	bool_t (*suspend)(struct device_t * dev, int state);
	bool_t (*resume)(struct device_t * dev);
};

struct driver_t * search_driver(const char * name);
bool_t register_driver(struct driver_t * drv);
bool_t unregister_driver(struct driver_t * drv);

#ifdef __cplusplus
}
#endif

#endif /* __DRIVER_H__ */
