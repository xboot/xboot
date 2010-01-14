#ifndef __RESOURCE_H__
#define __RESOURCE_H__


#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * the struct of resource.
 */
struct resource
{
	/* the resouce name */
	const char * name;

	/* the resouce data */
	void * data;
};

/*
 * the list of resource
 */
struct resource_list
{
	struct resource * res;
	struct list_head entry;
};


x_bool register_resource(struct resource * res);
x_bool unregister_resource(struct resource * res);
void * resource_get_data(const char * name);

#endif /* __RESOURCE_H__ */
