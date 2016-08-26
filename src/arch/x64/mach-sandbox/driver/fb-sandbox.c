/*
 * driver/fb-sandbox.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <fb/fb.h>
#include <sandbox.h>

struct fb_sandbox_pdata_t
{
	char * title;
	int width;
	int height;
	int xdpi;
	int ydpi;
	int bpp;
	int fullscreen;
	void * priv;
};

static void fb_setbl(struct fb_t * fb, int brightness)
{
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;
	sandbox_sdl_fb_set_backlight(pdat->priv, brightness);
}

static int fb_getbl(struct fb_t * fb)
{
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;
	return sandbox_sdl_fb_get_backlight(pdat->priv);
}

struct render_t * fb_create(struct fb_t * fb)
{
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;
	struct sandbox_fb_surface_t * surface;
	struct render_t * render;

	surface = malloc(sizeof(struct sandbox_fb_surface_t));
	if(!surface)
		return NULL;

	if(sandbox_sdl_fb_surface_create(pdat->priv, surface) != 0)
	{
		free(surface);
		return NULL;
	}

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		sandbox_sdl_fb_surface_destroy(pdat->priv, surface);
		free(surface);
		return NULL;
	}

	render->width = surface->width;
	render->height = surface->height;
	render->pitch = surface->pitch;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = surface->pixels;
	render->pixlen = surface->height * surface->pitch;
	render->priv = surface;

	render->clear = sw_render_clear;
	render->snapshot = sw_render_snapshot;
	render->alloc_texture = sw_render_alloc_texture;
	render->alloc_texture_similar = sw_render_alloc_texture_similar;
	render->free_texture = sw_render_free_texture;
	render->fill_texture = sw_render_fill_texture;
	render->blit_texture = sw_render_blit_texture;
	sw_render_create_data(render);

	return render;
}

void fb_destroy(struct fb_t * fb, struct render_t * render)
{
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;

	if(render)
	{
		sandbox_sdl_fb_surface_destroy(pdat->priv, render->priv);
		free(render->priv);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;
	sandbox_sdl_fb_surface_present(pdat->priv, render->priv);
}

static struct device_t * fb_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_sandbox_pdata_t * pdat;
	struct fb_t * fb;
	struct device_t * dev;

	pdat = malloc(sizeof(struct fb_sandbox_pdata_t));
	if(!pdat)
		return NULL;

	fb = malloc(sizeof(struct fb_t));
	if(!fb)
	{
		free(pdat);
		return NULL;
	}

	pdat->title = strdup(dt_read_string(n, "title", "Xboot Runtime Environment"));
	pdat->width = dt_read_int(n, "width", 640);
	pdat->height = dt_read_int(n, "height", 480);
	pdat->xdpi = dt_read_int(n, "x-dots-per-inch", 160);
	pdat->ydpi = dt_read_int(n, "y-dots-per-inch", 160);
	pdat->bpp = dt_read_int(n, "bits-per-pixel", 32);
	pdat->fullscreen = dt_read_bool(n, "fullscreen", 0);
	pdat->priv = sandbox_sdl_fb_init(pdat->title, pdat->width, pdat->height, pdat->fullscreen);
	pdat->width = sandbox_sdl_fb_get_width(pdat->priv);
	pdat->height = sandbox_sdl_fb_get_height(pdat->priv);

	fb->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	fb->width = pdat->width;
	fb->height = pdat->height;
	fb->xdpi = pdat->xdpi;
	fb->ydpi = pdat->ydpi;
	fb->bpp = pdat->bpp;
	fb->setbl = fb_setbl,
	fb->getbl = fb_getbl,
	fb->create = fb_create,
	fb->destroy = fb_destroy,
	fb->present = fb_present,
	fb->priv = pdat;

	if(!register_fb(&dev, fb))
	{
		sandbox_sdl_fb_exit(pdat->priv);
		free(pdat->title);

		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void fb_sandbox_remove(struct device_t * dev)
{
	struct fb_t * fb = (struct fb_t *)dev->priv;
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;

	if(fb && unregister_fb(fb))
	{
		sandbox_sdl_fb_exit(pdat->priv);
		free(pdat->title);

		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_sandbox_suspend(struct device_t * dev)
{
}

static void fb_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t fb_sandbox = {
	.name		= "fb-sandbox",
	.probe		= fb_sandbox_probe,
	.remove		= fb_sandbox_remove,
	.suspend	= fb_sandbox_suspend,
	.resume		= fb_sandbox_resume,
};

static __init void fb_sandbox_driver_init(void)
{
	register_driver(&fb_sandbox);
}

static __exit void fb_sandbox_driver_exit(void)
{
	unregister_driver(&fb_sandbox);
}

driver_initcall(fb_sandbox_driver_init);
driver_exitcall(fb_sandbox_driver_exit);
