#include <x.h>
#include <sandbox.h>

struct sandbox_fb_t {
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
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

	if(ioctl(hdl->fd, FBIOGET_FSCREENINFO, &hdl->finfo) != 0)
	{
		close(hdl->fd);
		free(hdl);
		return NULL;
	}

	if(ioctl(hdl->fd, FBIOGET_VSCREENINFO, &hdl->vinfo) != 0)
	{
		close(hdl->fd);
		free(hdl);
		return NULL;
	}

	hdl->vramsz = hdl->vinfo.xres * hdl->vinfo.yres * hdl->vinfo.bits_per_pixel / 8;
	hdl->vram = mmap(0, hdl->vramsz, PROT_READ | PROT_WRITE, MAP_SHARED, hdl->fd, 0);
	if(hdl->vram == (void *)(-1))
	{
		close(hdl->fd);
		free(hdl);
		return NULL;
	}
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
	return hdl->vinfo.xres;
}

int sandbox_fb_get_height(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	return hdl->vinfo.yres;
}

int sandbox_fb_get_pwidth(void * handle)
{
	return 216;
}

int sandbox_fb_get_pheight(void * handle)
{
	return 135;
}

int sandbox_fb_get_bpp(void * handle)
{
	return 32;
}

int sandbox_fb_surface_create(void * handle, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	surface->width = hdl->vinfo.xres;
	surface->height = hdl->vinfo.yres;
	surface->pitch = hdl->vinfo.xres * hdl->vinfo.bits_per_pixel / 8;
	surface->pixels = memalign(4, hdl->vramsz);
	return 1;
}

int sandbox_fb_surface_destroy(void * handle, struct sandbox_fb_surface_t * surface)
{
	if(surface && surface->pixels)
		free(surface->pixels);
	return 1;
}

int sandbox_fb_surface_present(void * handle, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	uint32_t *fbp, *pix;
	char * p;
	char * q;
	int i, j;

	fbp = hdl->vram;
	pix = surface->pixels;
	for(i = 0; i < hdl->vinfo.yres; i++)
	{
		for(j = 0; j < hdl->vinfo.xres; j++)
		{
			q = (char *)pix;
			p = (char *)fbp;
			p[0] = q[0];
			p[1] = q[1];
			p[2] = q[2];
			p[3] = q[3];
			fbp++;
			pix++;
		}
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
