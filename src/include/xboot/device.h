#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * device type
 */
enum device_type {
	CHAR_DEVICE,
	BLOCK_DEVICE,
	NET_DEVICE,
};

/*
 * the struct device.
 */
struct device
{
	/* the device name */
	const char * name;

	/* the type of device */
	enum device_type type;

	/* priv pointer */
	void * priv;
};

/*
 * the list of device
 */
struct device_list
{
	struct device * device;
	struct list_head entry;
};

struct device * search_device(const char * name);
bool_t register_device(struct device * dev);
bool_t unregister_device(struct device * dev);

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_H__ */
