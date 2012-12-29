#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
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

bool_t register_resource(struct resource * res);
bool_t unregister_resource(struct resource * res);
void * resource_get_data(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __RESOURCE_H__ */
