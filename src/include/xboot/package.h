#ifndef __PACKAGE_H__
#define __PACKAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <hmap.h>
#include <graphic/surface.h>

struct package_t {
	char * path;
	char * name;
	char * desc;
	struct surface_t * icon;
	struct surface_t * panel;
};

extern struct hmap_t * __package_list;

struct package_t * package_search(const char * path);
int package_removeable(struct package_t * pkg);
void package_rescan(void);

void do_init_package(void);

#ifdef __cplusplus
}
#endif

#endif /* __PACKAGE_H__ */
