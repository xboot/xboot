#ifndef __CAMERA_H__
#define __CAMERA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <camera/video.h>

enum {
	CAMERA_MIRROR_HFLIP	= (0x1 << 0),
	CAMERA_MIRROR_VFLIP	= (0x1 << 1),
};

struct camera_t
{
	char * name;

	int (*start)(struct camera_t * cam, enum video_format_t fmt, int width, int height);
	int (*stop)(struct camera_t * cam);
	int (*capture)(struct camera_t * cam, struct video_frame_t * frame);
	int (*ioctl)(struct camera_t * cam, const char * cmd, void * arg);

	void * priv;
};

static inline int camera_ioctl(struct camera_t * cam, const char * cmd, void * arg)
{
	if(cam && cam->ioctl)
		return cam->ioctl(cam, cmd, arg);
	return -1;
}

static inline int camera_get_gain(struct camera_t * cam)
{
	int gain;

	if(camera_ioctl(cam, "camera-get-gain", &gain) >= 0)
		return gain;
	return 0;
}

static inline void camera_set_gain(struct camera_t * cam, int gain)
{
	gain = clamp(gain, -1000, 1000);
	camera_ioctl(cam, "camera-set-gain", &gain);
}

static inline int camera_get_exposure(struct camera_t * cam)
{
	int exposure;

	if(camera_ioctl(cam, "camera-get-exposure", &exposure) >= 0)
		return exposure;
	return 0;
}

static inline void camera_set_exposure(struct camera_t * cam, int exposure)
{
	exposure = clamp(exposure, -1000, 1000);
	camera_ioctl(cam, "camera-set-exposure", &exposure);
}

static inline int camera_get_white_balance(struct camera_t * cam)
{
	int wb;

	if(camera_ioctl(cam, "camera-get-white-balance", &wb) >= 0)
		return wb;
	return 0;
}

static inline void camera_set_white_balance(struct camera_t * cam, int wb)
{
	wb = clamp(wb, -1000, 1000);
	camera_ioctl(cam, "camera-set-white-balance", &wb);
}

static inline int camera_get_focus(struct camera_t * cam)
{
	int focus;

	if(camera_ioctl(cam, "camera-get-focus", &focus) >= 0)
		return focus;
	return 0;
}

static inline void camera_set_focus(struct camera_t * cam, int focus)
{
	focus = clamp(focus, -1000, 1000);
	camera_ioctl(cam, "camera-set-focus", &focus);
}

static inline int camera_get_mirror(struct camera_t * cam)
{
	int mirror;

	if(camera_ioctl(cam, "camera-get-mirror", &mirror) >= 0)
		return mirror;
	return 0;
}

static inline void camera_set_mirror(struct camera_t * cam, int mirror)
{
	mirror &= CAMERA_MIRROR_HFLIP | CAMERA_MIRROR_VFLIP;
	camera_ioctl(cam, "camera-set-mirror", &mirror);
}

static inline int camera_get_saturation(struct camera_t * cam)
{
	int saturation;

	if(camera_ioctl(cam, "camera-get-saturation", &saturation))
		return saturation;
	return 0;
}

static inline void camera_set_saturation(struct camera_t * cam, int saturation)
{
	saturation = clamp(saturation, -1000, 1000);
	camera_ioctl(cam, "camera-set-saturation", &saturation);
}

static inline int camera_get_brightness(struct camera_t * cam)
{
	int brightness;

	if(camera_ioctl(cam, "camera-get-brightness", &brightness) >= 0)
		return brightness;
	return 0;
}

static inline void camera_set_brightness(struct camera_t * cam, int brightness)
{
	brightness = clamp(brightness, -1000, 1000);
	camera_ioctl(cam, "camera-set-brightness", &brightness);
}

static inline int camera_get_contrast(struct camera_t * cam)
{
	int contrast;

	if(camera_ioctl(cam, "camera-get-contrast", &contrast) >= 0)
		return contrast;
	return 0;
}

static inline void camera_set_contrast(struct camera_t * cam, int contrast)
{
	contrast = clamp(contrast, -1000, 1000);
	camera_ioctl(cam, "camera-set-contrast", &contrast);
}

static inline int camera_get_hue(struct camera_t * cam)
{
	int hue;

	if(camera_ioctl(cam, "camera-get-hue", &hue) >= 0)
		return hue;
	return 0;
}

static inline void camera_set_hue(struct camera_t * cam, int hue)
{
	hue = clamp(hue, -1000, 1000);
	camera_ioctl(cam, "camera-set-hue", &hue);
}

static inline int camera_get_sharpness(struct camera_t * cam)
{
	int sharpness;

	if(camera_ioctl(cam, "camera-get-sharpness", &sharpness) >= 0)
		return sharpness;
	return 0;
}

static inline void camera_set_sharpness(struct camera_t * cam, int sharpness)
{
	sharpness = clamp(sharpness, -1000, 1000);
	camera_ioctl(cam, "camera-set-sharpness", &sharpness);
}

struct camera_t * search_camera(const char * name);
struct camera_t * search_first_camera(void);
struct device_t * register_camera(struct camera_t * cam, struct driver_t * drv);
void unregister_camera(struct camera_t * cam);

int camera_start(struct camera_t * cam, enum video_format_t fmt, int width, int height);
int camera_stop(struct camera_t * cam);
int camera_capture(struct camera_t * cam, struct video_frame_t * frame, int timeout);

#ifdef __cplusplus
}
#endif

#endif /* __CAMERA_H__ */
