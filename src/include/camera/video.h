#ifndef __VIDEO_H__
#define __VIDEO_H__

#ifdef __cplusplus
extern "C" {
#endif

enum video_format_t {
	VIDEO_FORMAT_ARGB	= 0,	/* packed argb 32bits */
	VIDEO_FORMAT_YUYV	= 1,	/* yuyv 4:2:2 packed */
	VIDEO_FORMAT_UYVY	= 2,	/* uyvy 4:2:2 packed */
	VIDEO_FORMAT_NV12	= 3,	/* yuv 4:2:0, with one y plane and one packed u + v */
	VIDEO_FORMAT_NV21	= 4,	/* yuv 4:2:0, with one y plane and one packed v + u */
	VIDEO_FORMAT_YU12	= 5,	/* yuv 4:2:0 planar */
	VIDEO_FORMAT_YV12	= 6,	/* yvu 4:2:0 planar */
	VIDEO_FORMAT_MJPG	= 7,	/* motion jpeg */
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
