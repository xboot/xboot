#ifndef __GUI_H__
#define __GUI_H__

#include <configs.h>
#include <default.h>

/*
 * forward declare
 */
struct gui_component_ops;
struct gui_container_ops;

/*
 * gui component
 */
struct gui_component
{
	struct gui_component_ops * ops;
	struct rect rect;
};

struct gui_container
{
	struct gui_component component;
	struct gui_container_ops * ops;
};



struct gui_component_ops
{
/*	void (*destroy)(void * self);
	char * (*get_id)(void * self);
	int (*is_instance) (void *self, const char *type);
	void (*paint) (void *self, const grub_video_rect_t *bounds);
	void (*set_parent) (void *self, grub_gui_container_t parent);
	grub_gui_container_t (*get_parent) (void *self);
	void (*set_bounds) (void *self, const grub_video_rect_t *bounds);
	void (*get_bounds) (void *self, grub_video_rect_t *bounds);
	void (*get_minimal_size) (void *self, unsigned *width, unsigned *height);
	grub_err_t (*set_property) (void *self, const char *name, const char *value);
	void (*repaint) (void *self, int second_pass);*/
};

struct gui_container_ops
{
/*  void (*add) (void *self, grub_gui_component_t comp);
  void (*remove) (void *self, grub_gui_component_t comp);
  void (*iterate_children) (void *self,
                            grub_gui_component_callback cb, void *userdata);*/
};

#endif /* __GUI_H__ */
