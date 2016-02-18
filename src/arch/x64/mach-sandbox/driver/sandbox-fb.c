/*
 * driver/sandbox-fb.c
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
#include <sandbox-fb.h>

static void fb_init(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct sandbox_fb_data_t * dat = (struct sandbox_fb_data_t *)res->data;
	char title[256];
	sprintf(title, "Xboot Runtime Environment - V%s", xboot_version_string());
	dat->priv = sandbox_sdl_fb_init(title, dat->width, dat->height, dat->fullscreen);
	dat->width = sandbox_sdl_fb_get_width(dat->priv);
	dat->height = sandbox_sdl_fb_get_height(dat->priv);
}

static void fb_exit(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct sandbox_fb_data_t * dat = (struct sandbox_fb_data_t *)res->data;
	sandbox_sdl_fb_exit(dat->priv);
}

static void fb_setbl(struct fb_t * fb, int brightness)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct sandbox_fb_data_t * dat = (struct sandbox_fb_data_t *)res->data;
	sandbox_sdl_fb_set_backlight(dat->priv, brightness);
}

static int fb_getbl(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct sandbox_fb_data_t * dat = (struct sandbox_fb_data_t *)res->data;
	return sandbox_sdl_fb_get_backlight(dat->priv);
}

struct render_t * fb_create(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct sandbox_fb_data_t * dat = (struct sandbox_fb_data_t *)res->data;
	struct sandbox_fb_surface_t * surface;
	struct render_t * render;

	surface = malloc(sizeof(struct sandbox_fb_surface_t));
	if(!surface)
		return NULL;

	if(sandbox_sdl_fb_surface_create(dat->priv, surface) != 0)
	{
		free(surface);
		return NULL;
	}

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		sandbox_sdl_fb_surface_destroy(dat->priv, surface);
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
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct sandbox_fb_data_t * dat = (struct sandbox_fb_data_t *)res->data;

	if(render)
	{
		sandbox_sdl_fb_surface_destroy(dat->priv, render->priv);
		free(render->priv);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct sandbox_fb_data_t * dat = (struct sandbox_fb_data_t *)res->data;

	sandbox_sdl_fb_surface_present(dat->priv, render->priv);
}

static void fb_suspend(struct fb_t * fb)
{
}

static void fb_resume(struct fb_t * fb)
{
}

static bool_t sandbox_register_framebuffer(struct resource_t * res)
{
	struct sandbox_fb_data_t * dat = (struct sandbox_fb_data_t *)res->data;
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
	fb->bpp = 32;
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

static bool_t sandbox_unregister_framebuffer(struct resource_t * res)
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

static __init void sandbox_fb_init(void)
{
	resource_for_each("sandbox-fb", sandbox_register_framebuffer);
}

static __exit void sandbox_fb_exit(void)
{
	resource_for_each("sandbox-fb", sandbox_unregister_framebuffer);
}

postdevice_initcall(sandbox_fb_init);
postdevice_exitcall(sandbox_fb_exit);
