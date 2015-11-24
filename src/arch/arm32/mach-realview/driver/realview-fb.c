/*
 * driver/realview-fb.c
 *
 * realview framebuffer drivers. prime cell lcd controller (pl110)
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <realview-fb.h>

static void fb_init(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct realview_fb_data_t * dat = (struct realview_fb_data_t *)res->data;

	if(dat->init)
		dat->init(dat);

	write32(phys_to_virt(dat->regbase + CLCD_TIM0), (dat->timing.h_bp<<24) | (dat->timing.h_fp<<16) | (dat->timing.h_sw<<8) | ((dat->width/16-1)<<2));
	write32(phys_to_virt(dat->regbase + CLCD_TIM1), (dat->timing.v_bp<<24) | (dat->timing.v_fp<<16) | (dat->timing.v_sw<<10) | ((dat->height-1)<<0));
	write32(phys_to_virt(dat->regbase + CLCD_TIM2), (1<<26) | ((dat->width/16-1)<<16) | (1<<5) | (1<<0));
	write32(phys_to_virt(dat->regbase + CLCD_TIM3), (0<<0));

	write32(phys_to_virt(dat->regbase + CLCD_IMSC), 0x0);
	write32(phys_to_virt(dat->regbase + CLCD_CNTL), CNTL_LCDBPP24 | CNTL_LCDTFT | CNTL_BGR);
	write32(phys_to_virt(dat->regbase + CLCD_CNTL), (read32(phys_to_virt(dat->regbase + CLCD_CNTL)) | CNTL_LCDEN | CNTL_LCDPWR));
}

static void fb_exit(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct realview_fb_data_t * dat = (struct realview_fb_data_t *)res->data;

	if(dat->exit)
		dat->exit(dat);
}

static void fb_setbl(struct fb_t * fb, int brightness)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct realview_fb_data_t * dat = (struct realview_fb_data_t *)res->data;
	if(dat->setbl)
		dat->setbl(dat, brightness);
}

static int fb_getbl(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct realview_fb_data_t * dat = (struct realview_fb_data_t *)res->data;
	if(dat->getbl)
		return dat->getbl(dat);
	return 0;
}

struct render_t * fb_create(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct realview_fb_data_t * dat = (struct realview_fb_data_t *)res->data;
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	pixlen = dat->width * dat->height * dat->bytes_per_pixel;
	pixels = dma_alloc_coherent(pixlen);
	if(!pixels)
		return NULL;
	memset(pixels, 0, pixlen);

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(pixels);
		return NULL;
	}

	render->width = dat->width;
	render->height = dat->height;
	render->pitch = (dat->width * dat->bytes_per_pixel + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;
	render->pixlen = pixlen;
	render->priv = NULL;

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
	if(render)
	{
		sw_render_destroy_data(render);
		dma_free_coherent(render->pixels, render->pixlen);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct realview_fb_data_t * dat = (struct realview_fb_data_t *)res->data;
	void * pixels = render->pixels;

	if(pixels)
	{
		write32(phys_to_virt(dat->regbase + CLCD_UBAS), ((u32_t)pixels));
		write32(phys_to_virt(dat->regbase + CLCD_LBAS), ((u32_t)pixels + dat->width * dat->height * dat->bytes_per_pixel));
	}
}

static void fb_suspend(struct fb_t * fb)
{
}

static void fb_resume(struct fb_t * fb)
{
}

static bool_t realview_register_framebuffer(struct resource_t * res)
{
	struct realview_fb_data_t * dat = (struct realview_fb_data_t *)res->data;
	struct fb_t * fb;
	char name[64];

	fb = malloc(sizeof(struct fb_t));
	if(!fb)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	fb->name = strdup(name);
	fb->width = dat->width;
	fb->height = dat->height;
	fb->xdpi = dat->xdpi;
	fb->ydpi = dat->ydpi;
	fb->bpp = dat->bits_per_pixel;
	fb->init = fb_init,
	fb->exit = fb_exit,
	fb->setbl = fb_setbl,
	fb->getbl = fb_getbl,
	fb->create = fb_create,
	fb->destroy = fb_destroy,
	fb->present = fb_present,
	fb->suspend = fb_suspend,
	fb->resume = fb_resume,
	fb->priv = res;

	if(register_framebuffer(fb))
		return TRUE;

	free(fb->name);
	free(fb);
	return FALSE;
}

static bool_t realview_unregister_framebuffer(struct resource_t * res)
{
	struct fb_t * fb;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	fb = search_framebuffer(name);
	if(!fb)
		return FALSE;

	if(!unregister_framebuffer(fb))
		return FALSE;

	free(fb->name);
	free(fb);
	return TRUE;
}

static __init void realview_fb_init(void)
{
	resource_for_each_with_name("realview-fb", realview_register_framebuffer);
}

static __exit void realview_fb_exit(void)
{
	resource_for_each_with_name("realview-fb", realview_unregister_framebuffer);
}

device_initcall(realview_fb_init);
device_exitcall(realview_fb_exit);
