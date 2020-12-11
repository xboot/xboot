#ifndef __FRAMEWORK_PACKAGE_H__
#define __FRAMEWORK_PACKAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <hmap.h>
#include <graphic/surface.h>

struct package_t {
	const char * path;
	const char * name;
	const char * desc;
	const char * developer;
	const char * version;
	const char * url;
	struct surface_t * icon;
	struct surface_t * panel;
};

struct hmap_t * get_package_list(void);
struct package_t * package_search(const char * path);
const char * package_get_path(struct package_t * pkg);
const char * package_get_name(struct package_t * pkg);
const char * package_get_desc(struct package_t * pkg);
const char * package_get_developer(struct package_t * pkg);
const char * package_get_version(struct package_t * pkg);
const char * package_get_url(struct package_t * pkg);
struct surface_t * package_get_icon(struct package_t * pkg);
struct surface_t * package_get_panel(struct package_t * pkg);
void package_rescan(void);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_PACKAGE_H__ */
