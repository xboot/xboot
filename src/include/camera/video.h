#ifndef __VIDEO_H__
#define __VIDEO_H__

#ifdef __cplusplus
extern "C" {
#endif

enum video_format_t {
	VIDEO_FORMAT_ARGB	= 0,
	VIDEO_FORMAT_YUYV	= 1,
	VIDEO_FORMAT_NV12	= 2,
	VIDEO_FORMAT_NV21	= 3,
	VIDEO_FORMAT_YU12	= 4,
	VIDEO_FORMAT_YV12	= 5,
	VIDEO_FORMAT_MJPG	= 6,
};

struct video_frame_t {
	enum video_format_t fmt;
	int width;
	int height;
	int buflen;
	void * buf;
};

void video_frame_to_argb(struct video_frame_t * frame, void * pixels);

#ifdef __cplusplus
}
#endif

#endif /* __VIDEO_H__ */
