/*
 * driver/fb-sandbox.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <framebuffer/framebuffer.h>
#include <sandbox.h>

struct fb_sandbox_pdata_t
{
	int width;
	int height;
	int pwidth;
	int pheight;
	void * priv;
};

static void fb_setbl(struct framebuffer_t * fb, int brightness)
{
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;
	sandbox_fb_set_backlight(pdat->priv, brightness);
}

static int fb_getbl(struct framebuffer_t * fb)
{
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;
	return sandbox_fb_get_backlight(pdat->priv);
}

static struct surface_t * fb_create(struct framebuffer_t * fb)
{
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;
	struct sandbox_fb_surface_t * surface;
	struct surface_t * s;

	surface = malloc(sizeof(struct sandbox_fb_surface_t));
	if(!surface)
		return NULL;

	if(!sandbox_fb_surface_create(pdat->priv, surface))
	{
		free(surface);
		return NULL;
	}

	s = malloc(sizeof(struct surface_t));
	if(!s)
	{
		sandbox_fb_surface_destroy(pdat->priv, surface);
		free(surface);
		return NULL;
	}

	s->width = surface->width;
	s->height = surface->height;
	s->stride = surface->stride;
	s->pixlen = surface->pixlen;
	s->pixels = surface->pixels;
	s->r = search_render();
	s->rctx = s->r->create(s);
	s->priv = surface;

	return s;
}

static void fb_destroy(struct framebuffer_t * fb, struct surface_t * s)
{
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;

	if(s)
	{
		if(s->r)
			s->r->destroy(s->rctx);
		sandbox_fb_surface_destroy(pdat->priv, s->priv);
		free(s->priv);
		free(s);
	}
}

static void fb_present(struct framebuffer_t * fb, struct surface_t * s, struct region_list_t * rl)
{
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;
	sandbox_fb_surface_present(pdat->priv, s->priv, (struct sandbox_region_list_t *)rl);
}

static struct device_t * fb_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_sandbox_pdata_t * pdat;
	struct framebuffer_t * fb;
	struct device_t * dev;
	void * ctx;

	ctx = sandbox_fb_open(dt_read_string(n, "device", NULL));
	if(!ctx)
		return NULL;

	pdat = malloc(sizeof(struct fb_sandbox_pdata_t));
	if(!pdat)
		return NULL;

	fb = malloc(sizeof(struct framebuffer_t));
	if(!fb)
	{
		free(pdat);
		return NULL;
	}

	pdat->priv = ctx;
	pdat->width = sandbox_fb_get_width(pdat->priv);
	pdat->height = sandbox_fb_get_height(pdat->priv);
	pdat->pwidth = dt_read_int(n, "physical-width", sandbox_fb_get_pwidth(pdat->priv));
	pdat->pheight = dt_read_int(n, "physical-height", sandbox_fb_get_pheight(pdat->priv));

	fb->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	fb->width = pdat->width;
	fb->height = pdat->height;
	fb->pwidth = pdat->pwidth;
	fb->pheight = pdat->pheight;
	fb->setbl = fb_setbl;
	fb->getbl = fb_getbl;
	fb->create = fb_create;
	fb->destroy = fb_destroy;
	fb->present = fb_present;
	fb->priv = pdat;

	if(!(dev = register_framebuffer(fb, drv)))
	{
		sandbox_fb_close(pdat->priv);
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
		return NULL;
	}
	return dev;
}

static void fb_sandbox_remove(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_sandbox_pdata_t * pdat = (struct fb_sandbox_pdata_t *)fb->priv;

	if(fb)
	{
		unregister_framebuffer(fb);
		sandbox_fb_close(pdat->priv);
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
