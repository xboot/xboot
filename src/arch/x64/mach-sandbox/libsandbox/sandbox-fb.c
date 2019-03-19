#include <x.h>
#include <sandbox.h>

struct sandbox_fb_t {
	struct fb_fix_screeninfo fi;
	struct fb_var_screeninfo vi;
	int fd;
	int vramsz;
	void * vram;
};

void * sandbox_fb_open(const char * dev)
{
	struct sandbox_fb_t * hdl;

	hdl = malloc(sizeof(struct sandbox_fb_t));
	if(!hdl)
		return NULL;

	hdl->fd = open(dev, O_RDWR);
	if(hdl->fd < 0)
	{
		free(hdl);
		return NULL;
	}

	if(ioctl(hdl->fd, FBIOGET_FSCREENINFO, &hdl->fi) != 0)
	{
		close(hdl->fd);
		free(hdl);
		return NULL;
	}

	if(ioctl(hdl->fd, FBIOGET_VSCREENINFO, &hdl->vi) != 0)
	{
		close(hdl->fd);
		free(hdl);
		return NULL;
	}

	hdl->vi.red.offset = 0;
	hdl->vi.red.length = 8;
	hdl->vi.green.offset = 8;
	hdl->vi.green.length = 8;
	hdl->vi.blue.offset = 16;
	hdl->vi.blue.length = 8;
	hdl->vi.transp.offset = 24;
	hdl->vi.transp.length = 8;
	hdl->vi.bits_per_pixel = 32;
	hdl->vi.nonstd = 0;

	if(ioctl(hdl->fd, FBIOPUT_VSCREENINFO, &hdl->vi) != 0)
	{
		close(hdl->fd);
		free(hdl);
		return NULL;
	}

	hdl->vramsz = hdl->vi.yres_virtual * hdl->fi.line_length;
	hdl->vram = mmap(0, hdl->vramsz, PROT_READ | PROT_WRITE, MAP_SHARED, hdl->fd, 0);
	if(hdl->vram == (void *)(-1))
	{
		close(hdl->fd);
		free(hdl);
		return NULL;
	}
	memset(hdl->vram, 0, hdl->vramsz);

	return hdl;
}

void sandbox_fb_close(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;

	if(!hdl)
		return;
	if(hdl->vram != (void *)(-1))
		munmap(hdl->vram, hdl->vramsz);
	if(!(hdl->fd < 0))
		close(hdl->fd);
	free(hdl);
}

int sandbox_fb_get_width(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	if(hdl)
		return hdl->vi.xres;
	return 0;
}

int sandbox_fb_get_height(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	if(hdl)
		return hdl->vi.yres;
	return 0;
}

int sandbox_fb_get_pwidth(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	if(hdl)
		return 256;
	return 0;
}

int sandbox_fb_get_pheight(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	if(hdl)
		return 135;
	return 0;
}

int sandbox_fb_get_bpp(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	if(hdl)
		return hdl->vi.bits_per_pixel;
	return 0;
}

int sandbox_fb_surface_create(void * handle, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	surface->width = hdl->vi.xres;
	surface->height = hdl->vi.yres;
	surface->pitch = hdl->fi.line_length;
	surface->pixels = memalign(4, hdl->vramsz);
	return 1;
}

int sandbox_fb_surface_destroy(void * handle, struct sandbox_fb_surface_t * surface)
{
	if(surface && surface->pixels)
		free(surface->pixels);
	return 1;
}

int sandbox_fb_surface_present(void * handle, struct sandbox_fb_surface_t * surface, struct sandbox_fb_dirty_rect_t * rect, int nrect)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	unsigned char * p, * q;
	int stride, bytes, height, line;
	int i, j;

	if(rect && (nrect > 0))
	{
		stride = hdl->fi.line_length;
		bytes = hdl->vi.bits_per_pixel / 8;
		for(i = 0; i < nrect; i++)
		{
			height = rect[i].h;
			line = rect[i].w * bytes;
			p = (unsigned char *)surface->pixels + rect[i].y * stride + rect[i].x * bytes;
			q = (unsigned char *)hdl->vram + rect[i].y * stride + rect[i].x * bytes;
			for(j = 0; j < height; j++, p += stride, q += stride)
				memcpy(q, p, line);
		}
	}
	else
	{
		stride = hdl->fi.line_length;
		height = hdl->vi.yres;
		p = (unsigned char *)surface->pixels;
		q = (unsigned char *)hdl->vram;
		for(j = 0; j < height; j++, p += stride, q += stride)
			memcpy(q, p, stride);
	}
	return 1;
}

void sandbox_fb_set_backlight(void * handle, int brightness)
{
}

int sandbox_fb_get_backlight(void * handle)
{
	return 0;
}
