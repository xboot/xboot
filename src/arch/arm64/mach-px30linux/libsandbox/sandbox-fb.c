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

int sandbox_fb_surface_create(void * context, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_context_t * ctx = (struct sandbox_fb_context_t *)context;
	surface->width = ctx->vi.xres;
	surface->height = ctx->vi.yres;
	surface->stride = ctx->fi.line_length;
	surface->pixlen = ctx->vramsz;
	surface->pixels = memalign(4, ctx->vramsz);
	return 1;
}

int sandbox_fb_surface_destroy(void * context, struct sandbox_fb_surface_t * surface)
{
	if(surface && surface->pixels)
		free(surface->pixels);
	return 1;
}

int sandbox_fb_surface_present(void * context, struct sandbox_fb_surface_t * surface, struct sandbox_region_list_t * rl)
{
	struct sandbox_fb_context_t * ctx = (struct sandbox_fb_context_t *)context;
	struct sandbox_region_t * r;
	unsigned char * p, * q;
	int stride = ctx->fi.line_length;
	int bytes = ctx->vi.bits_per_pixel >> 3;
	int offset, line, height;
	int i, j;

	if(rl && (rl->count > 0))
	{
		for(i = 0; i < rl->count; i++)
		{
			r = &rl->region[i];
			offset = r->y * stride + r->x * bytes;
			line = r->w * bytes;
			height = r->h;

			p = (unsigned char *)ctx->vram + offset;
			q = (unsigned char *)surface->pixels + offset;
			for(j = 0; j < height; j++, p += stride, q += stride)
				memcpy(p, q, line);
		}
	}
	else
	{
		height = ctx->vi.yres;
		p = (unsigned char *)ctx->vram;
		q = (unsigned char *)surface->pixels;
		for(j = 0; j < height; j++, p += stride, q += stride)
			memcpy(p, q, stride);
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
