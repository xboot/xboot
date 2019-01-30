/*
 * kernel/core/display.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <xboot/display.h>

static void fb_dummy_setbl(struct framebuffer_t * fb, int brightness)
{
}

static int fb_dummy_getbl(struct framebuffer_t * fb)
{
	return CONFIG_MAX_BRIGHTNESS;
}

static struct render_t * fb_dummy_create(struct framebuffer_t * fb)
{
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	pixlen = fb->width * fb->height * (fb->bpp / 8);
	pixels = memalign(4, pixlen);
	if(!pixels)
		return NULL;

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(pixels);
		return NULL;
	}

	render->width = fb->width;
	render->height = fb->height;
	render->pitch = (fb->width * (fb->bpp / 8) + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;
	render->pixlen = pixlen;
	render->priv = NULL;

	return render;
}

static void fb_dummy_destroy(struct framebuffer_t * fb, struct render_t * render)
{
	if(render)
	{
		free(render->pixels);
		free(render);
	}
}

static void fb_dummy_present(struct framebuffer_t * fb, struct render_t * render, struct dirty_rect_t * rect, int nrect)
{
}

static struct framebuffer_t fb_dummy = {
	.name		= "fb-dummy",
	.width		= 640,
	.height		= 480,
	.pwidth		= 216,
	.pheight	= 135,
	.bpp		= 32,
	.setbl		= fb_dummy_setbl,
	.getbl		= fb_dummy_getbl,
	.create		= fb_dummy_create,
	.destroy	= fb_dummy_destroy,
	.present	= fb_dummy_present,
	.priv		= NULL,
};

struct display_t * display_alloc(const char * fb)
{
	struct display_t * d;
	struct framebuffer_t * dev;

	dev = search_framebuffer(fb);
	if(!dev)
	{
		dev = search_first_framebuffer();
		if(!dev)
			dev = &fb_dummy;
	}

	d = malloc(sizeof(struct display_t));
	if(!d)
		return NULL;

	d->fb = dev;
	d->cs = cairo_xboot_surface_create(d->fb);
	d->cr = cairo_create(d->cs);
	d->showobj = 0;
	d->showfps = 0;
	d->fps = 60;
	d->frame = 0;
	d->stamp = ktime_get();

	return d;
}

void display_free(struct display_t * d)
{
	if(!d)
		return;

	cairo_destroy(d->cr);
	cairo_surface_destroy(d->cs);
	free(d);
}
