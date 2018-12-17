#ifndef __DRIVER_H__
#define __DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <xboot/kobj.h>
#include <xboot/dtree.h>
#include <xboot/device.h>

struct device_t;

struct driver_t
{
	struct kobj_t * kobj;
	struct hlist_node node;

	char * name;
	struct device_t * (*probe)(struct driver_t * drv, struct dtnode_t * dt);
	void (*remove)(struct device_t * dev);
	void (*suspend)(struct device_t * dev);
	void (*resume)(struct device_t * dev);
};

struct driver_t * search_driver(const char * name);
bool_t register_driver(struct driver_t * drv);
bool_t unregister_driver(struct driver_t * drv);
void probe_device(const char * json, int length, const char * tips);

#ifdef __cplusplus
}
#endif

#endif /* __DRIVER_H__ */
