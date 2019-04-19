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

static struct dirty_region_t * dirty_region_alloc(int size)
{
	struct dirty_region_t * dr;
	struct region_t * r;

	if(size < 16)
		size = 16;
	if(size & (size - 1))
		size = roundup_pow_of_two(size);

	r = malloc(size * sizeof(struct region_t));
	if(!r)
		return NULL;

	dr = malloc(sizeof(struct dirty_region_t));
	if(!dr)
	{
		free(r);
		return NULL;
	}

	dr->region = r;
	dr->count = 0;
	dr->size = size;
	return dr;
}

static void dirty_region_free(struct dirty_region_t * dr)
{
	if(dr)
	{
		free(dr->region);
		free(dr);
	}
}

static void dirty_region_add(struct dirty_region_t * dr, double x, double y, double w, double h)
{
	if(dr->count >= dr->size)
	{
		dr->size <<= 1;
		dr->region = realloc(dr->region, dr->size * sizeof(struct region_t));
	}
	dr->region[dr->count].x = x;
	dr->region[dr->count].y = y;
	dr->region[dr->count].w = w;
	dr->region[dr->count].h = h;
	dr->count++;
}

struct display_t * display_alloc(const char * fb)
{
	struct display_t * disp;
	struct framebuffer_t * dev;

	dev = search_framebuffer(fb);
	if(!dev)
	{
		dev = search_first_framebuffer();
		if(!dev)
			dev = &fb_dummy;
	}

	disp = malloc(sizeof(struct display_t));
	if(!disp)
		return NULL;

	disp->fb = dev;
	disp->dr = dirty_region_alloc(0);
	disp->cursor = cairo_image_surface_create_from_png("/framework/assets/images/cursor.png");
	disp->cs = cairo_xboot_surface_create(disp->fb);
	disp->cr = cairo_create(disp->cs);
	disp->xpos = 0;
	disp->ypos = 0;
	disp->showcur = 0;
	disp->showobj = 0;
	disp->showfps = 0;
	disp->fps = 60;
	disp->frame = 0;
	disp->stamp = ktime_get();

	return disp;
}

void display_free(struct display_t * disp)
{
	if(!disp)
		return;

	cairo_destroy(disp->cr);
	cairo_surface_destroy(disp->cursor);
	cairo_surface_destroy(disp->cs);
	dirty_region_free(disp->dr);
	free(disp);
}

void display_present(struct display_t * disp, void * o, void (*draw)(void *, struct display_t *))
{
	cairo_t * cr;
	if(disp)
	{
		cr = disp->cr;
		cairo_reset_clip(cr);
		cairo_rectangle(cr, 0, 0, display_get_width(disp), display_get_height(disp));
		cairo_clip(cr);
		cairo_save(cr);
		cairo_set_source_rgb(cr, 1, 1, 1);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint(cr);
		cairo_restore(cr);

		draw(o, disp);

		if(disp->showfps)
		{
			char buf[32];
			ktime_t now = ktime_get();
			s64_t delta = ktime_ms_delta(now, disp->stamp);
			if(delta > 0)
				disp->fps = ((double)1000.0 / (double)delta) * 0.618 + disp->fps * 0.382;
			disp->frame++;
			disp->stamp = now;
			cairo_save(cr);
			cairo_set_font_size(cr, 24);
			cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
			cairo_move_to(cr, 0, 24);
			snprintf(buf, sizeof(buf), "%.2f %ld", disp->fps, disp->frame);
			cairo_show_text(cr, buf);
			cairo_restore(cr);
		}
		if(disp->showcur)
		{
			cairo_save(cr);
			cairo_set_source_surface(cr, disp->cursor, disp->xpos - 2, disp->ypos - 2);
			cairo_paint(cr);
			cairo_restore(cr);
		}
		cairo_xboot_surface_present(disp->cs, NULL, 0);
	}
}
