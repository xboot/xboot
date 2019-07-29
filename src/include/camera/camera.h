#ifndef __CAMERA_H__
#define __CAMERA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct camera_t
{
	char * name;

	void (*enable)(struct camera_t * cam);
	void (*disable)(struct camera_t * cam);
	void * priv;
};

struct camera_t * search_camera(const char * name);
struct camera_t * search_first_camera(void);
bool_t register_camera(struct device_t ** device, struct camera_t * cam);
bool_t unregister_camera(struct camera_t * cam);

#ifdef __cplusplus
}
#endif

#endif /* __CAMERA_H__ */
