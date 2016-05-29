/*
 * driver/bcm2836-fb.c
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

#include <bcm2836-fb.h>

struct bcm2836_fb_pdata_t {
	int width;
	int height;
	int xdpi;
	int ydpi;
	int bpp;
	int nrender;
	int count;
	int brightness;
	void * vram;
};

struct bcm2836_fb_surface_t {
	int offset;
};

static void fb_init(struct fb_t * fb)
{
}

static void fb_exit(struct fb_t * fb)
{
}

static void fb_setbl(struct fb_t * fb, int brightness)
{
	struct bcm2836_fb_pdata_t * pdat = (struct bcm2836_fb_pdata_t *)fb->priv;
	pdat->brightness = brightness;
}

static int fb_getbl(struct fb_t * fb)
{
	struct bcm2836_fb_pdata_t * pdat = (struct bcm2836_fb_pdata_t *)fb->priv;
	return pdat->brightness;
}

struct render_t * fb_create(struct fb_t * fb)
{
	struct bcm2836_fb_pdata_t * pdat = (struct bcm2836_fb_pdata_t *)fb->priv;
	struct bcm2836_fb_surface_t * surface;
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	if(pdat->count >= pdat->nrender)
		return NULL;

	pixlen = pdat->width * pdat->height * (pdat->bpp / 8);
	pixels = pdat->vram + pixlen * pdat->count;
	if(!pixels)
		return NULL;

	surface = malloc(sizeof(struct bcm2836_fb_surface_t));
	if(!surface)
		return NULL;

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(surface);
		return NULL;
	}

	surface->offset = pdat->height * pdat->count++;

	render->width = pdat->width;
	render->height = pdat->height;
	render->pitch = (pdat->width * (pdat->bpp / 8) + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;
	render->pixlen = pixlen;
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
	struct bcm2836_fb_pdata_t * pdat = (struct bcm2836_fb_pdata_t *)fb->priv;

	if(render)
	{
		if(pdat->count > 0)
			pdat->count--;
		sw_render_destroy_data(render);
		free(render->priv);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	struct bcm2836_fb_surface_t * surface = (struct bcm2836_fb_surface_t *)render->priv;

	if(render && render->pixels)
		bcm2836_mbox_fb_present(0, surface->offset);
}

static void fb_suspend(struct fb_t * fb)
{
}

static void fb_resume(struct fb_t * fb)
{
}

static bool_t bcm2836_register_framebuffer(struct resource_t * res)
{
	struct bcm2836_fb_data_t * rdat = (struct bcm2836_fb_data_t *)res->data;
	struct bcm2836_fb_pdata_t * pdat;
	struct fb_t * fb;
	char name[64];

	pdat = malloc(sizeof(struct bcm2836_fb_pdata_t));
	if(!pdat)
		return FALSE;

	fb = malloc(sizeof(struct fb_t));
	if(!fb)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->width = rdat->width;
	pdat->height = rdat->height;
	pdat->xdpi = rdat->xdpi;
	pdat->ydpi = rdat->ydpi;
	pdat->bpp = rdat->bpp;
	pdat->nrender = 8;
	pdat->count = 0;
	pdat->brightness = 0;
	pdat->vram = bcm2836_mbox_fb_alloc(pdat->width, pdat->height, pdat->bpp, pdat->nrender);

	fb->name = strdup(name);
	fb->width = pdat->width;
	fb->height = pdat->height;
	fb->xdpi = pdat->xdpi;
	fb->ydpi = pdat->ydpi;
	fb->bpp = pdat->bpp;
	fb->init = fb_init,
	fb->exit = fb_exit,
	fb->setbl = fb_setbl,
	fb->getbl = fb_getbl,
	fb->create = fb_create,
	fb->destroy = fb_destroy,
	fb->present = fb_present,
	fb->suspend = fb_suspend,
	fb->resume = fb_resume,
	fb->priv = pdat;

	if(register_framebuffer(fb))
		return TRUE;

	free(fb->priv);
	free(fb->name);
	free(fb);
	return FALSE;
}

static bool_t bcm2836_unregister_framebuffer(struct resource_t * res)
{
	struct fb_t * fb;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	fb = search_framebuffer(name);
	if(!fb)
		return FALSE;

	if(!unregister_framebuffer(fb))
		return FALSE;

	free(fb->priv);
	free(fb->name);
	free(fb);
	return TRUE;
}

static __init void bcm2836_fb_init(void)
{
	resource_for_each("bcm2836-fb", bcm2836_register_framebuffer);
}

static __exit void bcm2836_fb_exit(void)
{
	resource_for_each("bcm2836-fb", bcm2836_unregister_framebuffer);
}

postdevice_initcall(bcm2836_fb_init);
postdevice_exitcall(bcm2836_fb_exit);
