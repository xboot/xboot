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
struct device_t * register_camera(struct camera_t * cam, struct driver_t * drv);
void unregister_camera(struct camera_t * cam);

#ifdef __cplusplus
}
#endif

#endif /* __CAMERA_H__ */
