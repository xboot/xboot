#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct resource_t
{
	/* The resource name */
	const char * name;

	/* The resource data */
	void * data;
};

struct resource_list_t
{
	struct resource_t * res;
	struct list_head entry;
};

bool_t register_resource(struct resource_t * res);
bool_t unregister_resource(struct resource_t * res);
void * resource_get_data(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __RESOURCE_H__ */
