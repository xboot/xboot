#include <x.h>
#include <sandbox.h>

struct fb_drm_buf_t {
	uint32_t handle;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint32_t pixlen;
	void * pixels;
	uint32_t fb;
};

struct sandbox_fb_drm_context_t {
	int fd;
	drmModeRes * res;
	drmModeConnector * conn;
	uint32_t crtc_id;
	uint32_t conn_id;
	uint32_t width;
	uint32_t height;
	uint32_t pwidth;
	uint32_t pheight;
	uint32_t stride;
	uint32_t pixlen;
	int index;
	struct fb_drm_buf_t * drmbuf[2];
	struct sandbox_region_list_t * nrl, * orl;
};

static struct fb_drm_buf_t * fb_drm_buf_create(struct sandbox_fb_drm_context_t * ctx)
{
	struct drm_mode_create_dumb creq;
	struct drm_mode_destroy_dumb dreq;
	struct drm_mode_map_dumb mreq;
	struct fb_drm_buf_t * drmbuf;

	drmbuf = malloc(sizeof(struct fb_drm_buf_t));
	if(!drmbuf)
		return NULL;

	memset(&creq, 0, sizeof(creq));
	creq.width = ctx->width;
	creq.height = ctx->height;
	creq.bpp = 32;
	if(drmIoctl(ctx->fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq) != 0)
	{
		free(drmbuf);
		return NULL;
	}

	drmbuf->handle = creq.handle;
	drmbuf->width = creq.width;
	drmbuf->height = creq.height;
	drmbuf->stride = creq.pitch;
	drmbuf->pixlen = creq.size;
	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = drmbuf->handle;

	if(drmModeAddFB(ctx->fd, drmbuf->width, drmbuf->height, 24, 32, drmbuf->stride, drmbuf->handle, &drmbuf->fb) != 0)
	{
		drmIoctl(ctx->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
		free(drmbuf);
		return NULL;
	}

	memset(&mreq, 0, sizeof(mreq));
	mreq.handle = drmbuf->handle;
	if(drmIoctl(ctx->fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq) != 0)
	{
		drmIoctl(ctx->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
		free(drmbuf);
		return NULL;
	}

	drmbuf->pixels = mmap(0, drmbuf->pixlen, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fd, mreq.offset);
	if(drmbuf->pixels == MAP_FAILED)
	{
		drmModeRmFB(ctx->fd, drmbuf->fb);
		drmIoctl(ctx->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
		free(drmbuf);
		return NULL;
	}
	memset(drmbuf->pixels, 0, drmbuf->pixlen);
	return drmbuf;
}

static void fb_drm_buf_destroy(struct sandbox_fb_drm_context_t * ctx, struct fb_drm_buf_t * drmbuf)
{
	struct drm_mode_destroy_dumb dreq;

	if(ctx && drmbuf)
	{
		munmap(drmbuf->pixels, drmbuf->pixlen);
		drmModeRmFB(ctx->fd, drmbuf->fb);
		memset(&dreq, 0, sizeof(dreq));
		dreq.handle = drmbuf->handle;
		drmIoctl(ctx->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
		free(drmbuf);
	}
}

void * sandbox_fb_drm_open(const char * dev)
{
	struct sandbox_fb_drm_context_t * ctx;
	uint64_t dumb;

	ctx = malloc(sizeof(struct sandbox_fb_drm_context_t));
	if(!ctx)
		return NULL;

	ctx->fd = open(dev, O_RDWR | O_CLOEXEC);
	if(ctx->fd < 0)
	{
		free(ctx);
		return NULL;
	}

	if(drmGetCap(ctx->fd, DRM_CAP_DUMB_BUFFER, &dumb) < 0 || !dumb)
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	ctx->res = drmModeGetResources(ctx->fd);
	if(!ctx->res)
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	ctx->crtc_id = ctx->res->crtcs[0];
	ctx->conn_id = ctx->res->connectors[0];
	ctx->conn = drmModeGetConnector(ctx->fd, ctx->conn_id);
	if(!ctx->conn)
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}
	ctx->width = ctx->conn->modes[0].hdisplay;
	ctx->height = ctx->conn->modes[0].vdisplay;
	ctx->pwidth = 256;
	ctx->pheight = 135;
	ctx->index = 0;
	ctx->drmbuf[0] = fb_drm_buf_create(ctx);
	ctx->drmbuf[1] = fb_drm_buf_create(ctx);
	ctx->nrl = sandbox_region_list_alloc(0);
	ctx->orl = sandbox_region_list_alloc(0);
	ctx->stride = ctx->drmbuf[0]->stride;
	ctx->pixlen = ctx->drmbuf[0]->pixlen;

	return ctx;
}

void sandbox_fb_drm_close(void * context)
{
	struct sandbox_fb_drm_context_t * ctx = (struct sandbox_fb_drm_context_t *)context;

	if(ctx)
	{
		fb_drm_buf_destroy(ctx, ctx->drmbuf[0]);
		fb_drm_buf_destroy(ctx, ctx->drmbuf[1]);
		sandbox_region_list_free(ctx->nrl);
		sandbox_region_list_free(ctx->orl);
		close(ctx->fd);
		free(ctx);
	}
}

int sandbox_fb_drm_get_width(void * context)
{
	struct sandbox_fb_drm_context_t * ctx = (struct sandbox_fb_drm_context_t *)context;
	return ctx->width;
}

int sandbox_fb_drm_get_height(void * context)
{
	struct sandbox_fb_drm_context_t * ctx = (struct sandbox_fb_drm_context_t *)context;
	return ctx->height;
}

int sandbox_fb_drm_get_pwidth(void * context)
{
	struct sandbox_fb_drm_context_t * ctx = (struct sandbox_fb_drm_context_t *)context;
	return ctx->pwidth;
}

int sandbox_fb_drm_get_pheight(void * context)
{
	struct sandbox_fb_drm_context_t * ctx = (struct sandbox_fb_drm_context_t *)context;
	return ctx->pheight;
}

int sandbox_fb_drm_surface_create(void * context, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_drm_context_t * ctx = (struct sandbox_fb_drm_context_t *)context;
	surface->width = ctx->width;
	surface->height = ctx->height;
	surface->stride = ctx->stride;
	surface->pixlen = ctx->pixlen;
	surface->pixels = memalign(4, ctx->pixlen);
	return 1;
}

int sandbox_fb_drm_surface_destroy(void * context, struct sandbox_fb_surface_t * surface)
{
	if(surface && surface->pixels)
		free(surface->pixels);
	return 1;
}

int sandbox_fb_drm_surface_present(void * context, struct sandbox_fb_surface_t * surface, struct sandbox_region_list_t * rl)
{
	struct sandbox_fb_drm_context_t * ctx = (struct sandbox_fb_drm_context_t *)context;
	struct sandbox_region_list_t * nrl = ctx->nrl;
	struct fb_drm_buf_t * drmbuf;
	struct sandbox_region_t * r;
	unsigned char * p, * q;
	int stride = ctx->stride;
	int offset, line, height;
	int i, j;

	sandbox_region_list_clear(nrl);
	sandbox_region_list_merge(nrl, ctx->orl);
	sandbox_region_list_merge(nrl, rl);
	sandbox_region_list_clone(ctx->orl, rl);

	ctx->index = (ctx->index + 1) & 0x1;
	drmbuf = ctx->drmbuf[ctx->index];
	if(nrl && (nrl->count > 0))
	{
		for(i = 0; i < nrl->count; i++)
		{
			r = &nrl->region[i];
			offset = r->y * stride + (r->x << 2);
			line = r->w << 2;
			height = r->h;

			p = (unsigned char *)drmbuf->pixels + offset;
			q = (unsigned char *)surface->pixels + offset;
			for(j = 0; j < height; j++, p += stride, q += stride)
				memcpy(p, q, line);
		}
	}
	else
	{
		memcpy(drmbuf->pixels, surface->pixels, surface->pixlen);
	}
	drmModeSetCrtc(ctx->fd, ctx->crtc_id, drmbuf->fb, 0, 0, &ctx->conn_id, 1, &ctx->conn->modes[0]);
	return 1;
}

void sandbox_fb_drm_set_backlight(void * context, int brightness)
{
}

int sandbox_fb_drm_get_backlight(void * context)
{
	return 0;
}
