#include <x.h>
#include <sandbox.h>

enum {
	SANDBOX_VIDEO_FORMAT_ARGB	= 0,
	SANDBOX_VIDEO_FORMAT_YUYV	= 1,
	SANDBOX_VIDEO_FORMAT_UYVY	= 2,
	SANDBOX_VIDEO_FORMAT_NV12	= 3,
	SANDBOX_VIDEO_FORMAT_NV21	= 4,
	SANDBOX_VIDEO_FORMAT_YU12	= 5,
	SANDBOX_VIDEO_FORMAT_YV12	= 6,
	SANDBOX_VIDEO_FORMAT_MJPG	= 7,
};

struct sandbox_cam_buf_t {
    void * start;
    size_t length;
};

struct sandbox_cam_context_t {
	int fd;
	int nbuf;
	struct sandbox_cam_buf_t * buf;
};

int sandbox_cam_exist(const char * dev)
{
    struct stat st;

    if(stat(dev, &st) == 0 && S_ISCHR(st.st_mode))
    	return 1;
    return 0;
}

void * sandbox_cam_start(const char * dev, int * format, int * width, int * height)
{
	struct sandbox_cam_context_t * ctx;
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	enum v4l2_buf_type type;
	int i;

	ctx = malloc(sizeof(struct sandbox_cam_context_t));
	if(!ctx)
		return NULL;

	ctx->fd = open(dev, O_RDWR | O_NONBLOCK);
	if(ctx->fd < 0)
	{
		free(ctx);
		return NULL;
	}

	memset(&cap, 0, sizeof(struct v4l2_capability));
	if(ioctl(ctx->fd, VIDIOC_QUERYCAP, &cap) != 0)
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	if(!(cap.capabilities & V4L2_CAP_STREAMING))
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	memset(&cropcap, 0, sizeof(struct v4l2_cropcap));
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(ctx->fd, VIDIOC_CROPCAP, &cropcap) == 0)
	{
		memset(&crop, 0, sizeof(struct v4l2_crop));
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect;
		ioctl(ctx->fd, VIDIOC_S_CROP, &crop);
	}

	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = *width;
	fmt.fmt.pix.height = *height;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	switch(*format)
	{
	case SANDBOX_VIDEO_FORMAT_ARGB:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
		break;
	case SANDBOX_VIDEO_FORMAT_YUYV:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		break;
	case SANDBOX_VIDEO_FORMAT_UYVY:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
		break;
	case SANDBOX_VIDEO_FORMAT_NV12:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
		break;
	case SANDBOX_VIDEO_FORMAT_NV21:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV21;
		break;
	case SANDBOX_VIDEO_FORMAT_YU12:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
		break;
	case SANDBOX_VIDEO_FORMAT_YV12:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YVU420;
		break;
	case SANDBOX_VIDEO_FORMAT_MJPG:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
		break;
	default:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		break;
	}
	if(ioctl(ctx->fd, VIDIOC_S_FMT, &fmt) != 0)
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	if(ioctl(ctx->fd, VIDIOC_G_FMT, &fmt) == 0)
	{
		*width = fmt.fmt.pix.width;
		*height = fmt.fmt.pix.height;
		switch(fmt.fmt.pix.pixelformat)
		{
		case V4L2_PIX_FMT_RGB32:
			*format = SANDBOX_VIDEO_FORMAT_ARGB;
			break;
		case V4L2_PIX_FMT_YUYV:
			*format = SANDBOX_VIDEO_FORMAT_YUYV;
			break;
		case V4L2_PIX_FMT_UYVY:
			*format = SANDBOX_VIDEO_FORMAT_UYVY;
			break;
		case V4L2_PIX_FMT_NV12:
			*format = SANDBOX_VIDEO_FORMAT_NV12;
			break;
		case V4L2_PIX_FMT_NV21:
			*format = SANDBOX_VIDEO_FORMAT_NV21;
			break;
		case V4L2_PIX_FMT_YUV420:
			*format = SANDBOX_VIDEO_FORMAT_YU12;
			break;
		case V4L2_PIX_FMT_YVU420:
			*format = SANDBOX_VIDEO_FORMAT_YV12;
			break;
		case V4L2_PIX_FMT_MJPEG:
			*format = SANDBOX_VIDEO_FORMAT_MJPG;
			break;
		default:
			*format = SANDBOX_VIDEO_FORMAT_YUYV;
			break;
		}
	}

	memset(&req, 0, sizeof(struct v4l2_requestbuffers));
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	req.count = 4;
	if(ioctl(ctx->fd, VIDIOC_REQBUFS, &req) != 0)
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	ctx->buf = calloc(req.count, sizeof(struct sandbox_cam_buf_t));
	if(!ctx->buf)
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	for(ctx->nbuf = 0; ctx->nbuf < req.count; ctx->nbuf++)
	{
		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = ctx->nbuf;

		if(ioctl(ctx->fd, VIDIOC_QUERYBUF, &buf) != 0)
		{
			for(i = 0; i < ctx->nbuf; i++)
				munmap(ctx->buf[i].start, ctx->buf[i].length);
			close(ctx->fd);
			free(ctx->buf);
			free(ctx);
			return NULL;
		}
		ctx->buf[ctx->nbuf].length = buf.length;
		ctx->buf[ctx->nbuf].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fd, buf.m.offset);
		if(ctx->buf[ctx->nbuf].start == MAP_FAILED)
		{
			for(i = 0; i < ctx->nbuf; i++)
				munmap(ctx->buf[i].start, ctx->buf[i].length);
			close(ctx->fd);
			free(ctx->buf);
			free(ctx);
			return NULL;
		}
	}

	for(i = 0; i < ctx->nbuf; ++i)
	{
		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if(ioctl(ctx->fd, VIDIOC_QBUF, &buf) != 0)
		{
			for(i = 0; i < ctx->nbuf; i++)
				munmap(ctx->buf[i].start, ctx->buf[i].length);
			close(ctx->fd);
			free(ctx->buf);
			free(ctx);
			return NULL;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(ctx->fd, VIDIOC_STREAMON, &type) != 0)
	{
		for(i = 0; i < ctx->nbuf; i++)
			munmap(ctx->buf[i].start, ctx->buf[i].length);
		close(ctx->fd);
		free(ctx->buf);
		free(ctx);
		return NULL;
	}

	return ctx;
}

void sandbox_cam_stop(void * context)
{
	struct sandbox_cam_context_t * ctx = (struct sandbox_cam_context_t *)context;
	enum v4l2_buf_type type;
	int i;

	if(ctx)
	{
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		ioctl(ctx->fd, VIDIOC_STREAMOFF, &type);
		for(i = 0; i < ctx->nbuf; i++)
			munmap(ctx->buf[i].start, ctx->buf[i].length);
		close(ctx->fd);
		free(ctx->buf);
		free(ctx);
	}
}

int sandbox_cam_capture(void * context, void ** buf)
{
	struct sandbox_cam_context_t * ctx = (struct sandbox_cam_context_t *)context;
	struct v4l2_buffer buffer;
	int len;

	memset(&buffer, 0, sizeof(struct v4l2_buffer));
	buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buffer.memory = V4L2_MEMORY_MMAP;
	if(ioctl(ctx->fd, VIDIOC_DQBUF, &buffer) == 0)
	{
		*buf = ctx->buf[buffer.index].start;
		len = ctx->buf[buffer.index].length;
	}
	else
	{
		len = 0;
	}
	if(ioctl(ctx->fd, VIDIOC_QBUF, &buffer) != 0)
		len = 0;
	return len;
}
