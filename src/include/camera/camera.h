#ifndef __CAMERA_H__
#define __CAMERA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <camera/video.h>

struct camera_t
{
	char * name;

	int (*start)(struct camera_t * cam, enum video_format_t fmt, int width, int height);
	int (*stop)(struct camera_t * cam);
	int (*capture)(struct camera_t * cam, struct video_frame_t * frame);
	int (*ioctl)(struct camera_t * cam, const char * cmd, void * arg);

	void * priv;
};

struct camera_t * search_camera(const char * name);
struct camera_t * search_first_camera(void);
struct device_t * register_camera(struct camera_t * cam, struct driver_t * drv);
void unregister_camera(struct camera_t * cam);

int camera_start(struct camera_t * cam, enum video_format_t fmt, int width, int height);
int camera_stop(struct camera_t * cam);
int camera_capture(struct camera_t * cam, struct video_frame_t * frame, int timeout);
int camera_ioctl(struct camera_t * cam, const char * cmd, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* __CAMERA_H__ */
