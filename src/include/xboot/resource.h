#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct resource_t
{
	/* Kobj binding */
	struct kobj_t * kobj;

	/* The machine name, NULL for all */
	const char * mach;

	/* The resource name */
	const char * name;

	/* The resource id */
	int id;

	/* The resource data */
	void * data;
};

bool_t register_resource(struct resource_t * res);
bool_t unregister_resource(struct resource_t * res);
void resource_for_each_with_name(const char * name, bool_t (*fn)(struct resource_t *));

#ifdef __cplusplus
}
#endif

#endif /* __RESOURCE_H__ */
