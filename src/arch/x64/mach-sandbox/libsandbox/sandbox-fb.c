#include <x.h>
#include <sandbox.h>

struct sandbox_fb_context_t {
	struct fb_fix_screeninfo fi;
	struct fb_var_screeninfo vi;
	int fd;
	int vramsz;
	void * vram;
};

void * sandbox_fb_open(const char * dev)
{
	struct sandbox_fb_context_t * ctx;

	ctx = malloc(sizeof(struct sandbox_fb_context_t));
	if(!ctx)
		return NULL;

	ctx->fd = open(dev, O_RDWR);
	if(ctx->fd < 0)
	{
		free(ctx);
		return NULL;
	}

	if(ioctl(ctx->fd, FBIOGET_FSCREENINFO, &ctx->fi) != 0)
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	if(ioctl(ctx->fd, FBIOGET_VSCREENINFO, &ctx->vi) != 0)
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	ctx->vi.red.offset = 0;
	ctx->vi.red.length = 8;
	ctx->vi.green.offset = 8;
	ctx->vi.green.length = 8;
	ctx->vi.blue.offset = 16;
	ctx->vi.blue.length = 8;
	ctx->vi.transp.offset = 24;
	ctx->vi.transp.length = 8;
	ctx->vi.bits_per_pixel = 32;
	ctx->vi.nonstd = 0;

	if(ioctl(ctx->fd, FBIOPUT_VSCREENINFO, &ctx->vi) != 0)
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}

	ctx->vramsz = ctx->vi.yres_virtual * ctx->fi.line_length;
	ctx->vram = mmap(0, ctx->vramsz, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fd, 0);
	if(ctx->vram == (void *)(-1))
	{
		close(ctx->fd);
		free(ctx);
		return NULL;
	}
	memset(ctx->vram, 0, ctx->vramsz);

	return ctx;
}

void sandbox_fb_close(void * context)
{
	struct sandbox_fb_context_t * ctx = (struct sandbox_fb_context_t *)context;

	if(!ctx)
		return;
	if(ctx->vram != (void *)(-1))
		munmap(ctx->vram, ctx->vramsz);
	if(!(ctx->fd < 0))
		close(ctx->fd);
	free(ctx);
}

int sandbox_fb_get_width(void * context)
{
	struct sandbox_fb_context_t * ctx = (struct sandbox_fb_context_t *)context;
	if(ctx)
		return ctx->vi.xres;
	return 0;
}

int sandbox_fb_get_height(void * context)
{
	struct sandbox_fb_context_t * ctx = (struct sandbox_fb_context_t *)context;
	if(ctx)
		return ctx->vi.yres;
	return 0;
}

int sandbox_fb_get_pwidth(void * context)
{
	struct sandbox_fb_context_t * ctx = (struct sandbox_fb_context_t *)context;
	if(ctx)
		return 256;
	return 0;
}

int sandbox_fb_get_pheight(void * context)
{
	struct sandbox_fb_context_t * ctx = (struct sandbox_fb_context_t *)context;
	if(ctx)
		return 135;
	return 0;
}

int sandbox_fb_get_bpp(void * context)
{
	struct sandbox_fb_context_t * ctx = (struct sandbox_fb_context_t *)context;
	if(ctx)
		return ctx->vi.bits_per_pixel;
	return 0;
}

int sandbox_fb_surface_create(void * context, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_context_t * ctx = (struct sandbox_fb_context_t *)context;
	surface->width = ctx->vi.xres;
	surface->height = ctx->vi.yres;
	surface->pitch = ctx->fi.line_length;
	surface->pixels = memalign(4, ctx->vramsz);
	return 1;
}

int sandbox_fb_surface_destroy(void * context, struct sandbox_fb_surface_t * surface)
{
	if(surface && surface->pixels)
		free(surface->pixels);
	return 1;
}

int sandbox_fb_surface_present(void * context, struct sandbox_fb_surface_t * surface, struct sandbox_fb_dirty_rect_t * rect, int nrect)
{
	struct sandbox_fb_context_t * ctx = (struct sandbox_fb_context_t *)context;
	unsigned char * p, * q;
	int stride, bytes, height, line;
	int i, j;

	if(rect && (nrect > 0))
	{
		stride = ctx->fi.line_length;
		bytes = ctx->vi.bits_per_pixel / 8;
		for(i = 0; i < nrect; i++)
		{
			height = rect[i].h;
			line = rect[i].w * bytes;
			p = (unsigned char *)surface->pixels + rect[i].y * stride + rect[i].x * bytes;
			q = (unsigned char *)ctx->vram + rect[i].y * stride + rect[i].x * bytes;
			for(j = 0; j < height; j++, p += stride, q += stride)
				memcpy(q, p, line);
		}
	}
	else
	{
		stride = ctx->fi.line_length;
		height = ctx->vi.yres;
		p = (unsigned char *)surface->pixels;
		q = (unsigned char *)ctx->vram;
		for(j = 0; j < height; j++, p += stride, q += stride)
			memcpy(q, p, stride);
	}
	return 1;
}

void sandbox_fb_set_backlight(void * context, int brightness)
{
}

int sandbox_fb_get_backlight(void * context)
{
	return 0;
}
