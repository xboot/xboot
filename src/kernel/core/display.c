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

	pixlen = fb->width * fb->height * fb->bytes;
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
	render->pitch = (fb->width * fb->bytes + 0x3) & ~0x3;
	render->bytes = fb->bytes;
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

static void fb_dummy_present(struct framebuffer_t * fb, struct render_t * render, struct region_t * region, int n)
{
}

static struct framebuffer_t fb_dummy = {
	.name		= "fb-dummy",
	.width		= 640,
	.height		= 480,
	.pwidth		= 216,
	.pheight	= 135,
	.bytes		= 4,
	.setbl		= fb_dummy_setbl,
	.getbl		= fb_dummy_getbl,
	.create		= fb_dummy_create,
	.destroy	= fb_dummy_destroy,
	.present	= fb_dummy_present,
	.priv		= NULL,
};

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
	disp->rl = region_list_alloc(CONFIG_DISPLAY_REGION_SIZE);
	disp->cs = cairo_xboot_surface_create(disp->fb);
	disp->cr = cairo_create(disp->cs);
	disp->cursor.cs = cairo_image_surface_create_from_png("/framework/assets/images/cursor.png");
	disp->cursor.width = cairo_image_surface_get_width(disp->cursor.cs);
	disp->cursor.height = cairo_image_surface_get_height(disp->cursor.cs);
	disp->cursor.ox = 0;
	disp->cursor.oy = 0;
	disp->cursor.nx = 0;
	disp->cursor.ny = 0;
	disp->cursor.dirty = 0;
	disp->cursor.show = 0;
	disp->fps.rate = 60;
	disp->fps.frame = 0;
	disp->fps.stamp = ktime_get();
	disp->fps.show = 0;
	disp->showobj = 0;

	return disp;
}

void display_free(struct display_t * disp)
{
	if(!disp)
		return;

	cairo_destroy(disp->cr);
	cairo_surface_destroy(disp->cursor.cs);
	cairo_surface_destroy(disp->cs);
	region_list_free(disp->rl);
	free(disp);
}

void display_region_list_add(struct display_t * disp, struct region_t * r)
{
	struct region_t region;
	if(disp)
	{
		region_init(&region, 0, 0, disp->fb->width, disp->fb->height);
		if(region_intersect(&region, &region, r))
			region_list_add(disp->rl, &region);
	}
}

void display_region_list_clear(struct display_t * disp)
{
	if(disp)
		region_list_clear(disp->rl);
}

void display_present(struct display_t * disp, void * o, void (*draw)(struct display_t *, void *))
{
	cairo_t * cr;
	struct region_t rn, ro, * r;
	char fps[32];
	int count;
	int i;

	if(disp)
	{
		if(disp->cursor.show && disp->cursor.dirty)
		{
			region_init(&rn, disp->cursor.nx, disp->cursor.ny, disp->cursor.width, disp->cursor.height);
			region_init(&ro, disp->cursor.ox, disp->cursor.oy, disp->cursor.width, disp->cursor.height);
			display_region_list_add(disp, &rn);
			display_region_list_add(disp, &ro);
			disp->cursor.dirty = 0;
		}

		if(disp->fps.show)
		{
			ktime_t now = ktime_get();
			s64_t delta = ktime_ms_delta(now, disp->fps.stamp);
			if(delta > 0)
				disp->fps.rate = ((double)1000.0 / (double)delta) * 0.618 + disp->fps.rate * 0.382;
			disp->fps.frame++;
			disp->fps.stamp = now;
			int len = snprintf(fps, sizeof(fps), "%.2f %ld", disp->fps.rate, disp->fps.frame);
			region_init(&rn, 0, 0, len * (24 / 2), 24);
			display_region_list_add(disp, &rn);
		}

		if((count = disp->rl->count) > 0)
		{
			cr = disp->cr;

			cairo_reset_clip(cr);
			for(i = 0; i < count; i++)
			{
				r = &disp->rl->region[i];
				cairo_rectangle(cr, r->x, r->y, r->w, r->h);
			}
			cairo_clip(cr);
			cairo_save(cr);
			cairo_set_source_rgb(cr, 1, 1, 1);
			cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
			cairo_paint(cr);
			cairo_restore(cr);

			if(draw)
				draw(disp, o);

			#if 0
			{
				static int flag = 0;
				cairo_save(cr);
				flag = !flag;
				if(flag)
					cairo_set_source_rgba(cr, 1, 0, 0, 0.7);
				else
					cairo_set_source_rgba(cr, 0, 1, 0, 0.7);
				cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
				cairo_paint(cr);
				cairo_restore(cr);
			}
			#endif

			if(disp->cursor.show)
			{
				cairo_save(cr);
				cairo_set_source_surface(cr, disp->cursor.cs, disp->cursor.nx, disp->cursor.ny);
				cairo_paint(cr);
				cairo_restore(cr);
			}

			if(disp->fps.show)
			{
				cairo_save(cr);
				cairo_set_font_size(cr, 24);
				cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
				cairo_move_to(cr, 0, 24);
				cairo_show_text(cr, fps);
				cairo_restore(cr);
			}

			cairo_xboot_surface_present(disp->cs, disp->rl->region, count);
		}
	}
}
