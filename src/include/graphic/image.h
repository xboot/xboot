#ifndef __GRAPHIC_IMAGE_H__
#define __GRAPHIC_IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <graphic/surface.h>

struct image_loader
{
	const char * extension;
	struct surface_t * (*load)(const char * filename);
};

struct image_loader_list
{
	struct image_loader * loader;
	struct list_head entry;
};

struct surface_t * surface_load_from_file(const char * filename);
bool_t register_image_loader(struct image_loader * loader);
bool_t unregister_image_loader(struct image_loader * loader);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_IMAGE_H__ */
