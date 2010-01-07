#ifndef __PLATFORM_DEVICE_H__
#define __PLATFORM_DEVICE_H__


#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * the struct of platform device.
 */
struct platform_device {
	/* the platform device name */
	const char * name;

	/* the platform device data */
	void * data;
};

/*
 * the list of platform device
 */
struct platform_device_list
{
	struct platform_device * pdev;
	struct hlist_node node;
};


x_bool platform_device_register(struct platform_device * pdev);
x_bool platform_device_unregister(struct platform_device * pdev);
void * platform_device_get_data(const char *name);

#endif /* __PLATFORM_DEVICE_H__ */
