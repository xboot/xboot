#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

enum device_type_t {
	DEVICE_TYPE_CHAR,
	DEVICE_TYPE_BLOCK,
	DEVICE_TYPE_NET,
};

struct device_t
{
	/* the device name */
	const char * name;

	/* the type of device */
	enum device_type_t type;

	/* priv pointer */
	void * priv;
};

struct device_list_t
{
	struct device_t * device;
	struct list_head entry;
};

struct device_t * search_device(const char * name);
bool_t register_device(struct device_t * dev);
bool_t unregister_device(struct device_t * dev);

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_H__ */
