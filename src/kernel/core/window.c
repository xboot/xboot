/*
 * kernel/core/window.c
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
#include <xboot/window.h>

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

static void fb_dummy_present(struct framebuffer_t * fb, struct render_t * render, struct region_list_t * rl)
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

static struct window_manager_t __window_manager = {
	.entry = {
		.next	= &(__window_manager.entry),
		.prev	= &(__window_manager.entry),
	},
};
static spinlock_t __window_manager_lock = SPIN_LOCK_INIT();

static inline struct window_manager_t * window_manager_search(struct framebuffer_t * fb)
{
	struct window_manager_t * pos, * n;

	if(fb)
	{
		list_for_each_entry_safe(pos, n, &__window_manager.entry, entry)
		{
			if(pos->fb == fb)
				return pos;
		}
	}
	return NULL;
}

struct window_manager_t * window_manager_alloc(const char * fb)
{
	struct window_manager_t * wm;
	struct framebuffer_t * dev;
	irq_flags_t flags;

	dev = search_framebuffer(fb);
	if(!dev)
	{
		dev = search_first_framebuffer();
		if(!dev)
			dev = &fb_dummy;
	}
	wm = window_manager_search(dev);
	if(wm)
		return wm;

	wm = malloc(sizeof(struct window_manager_t));
	if(!wm)
		return NULL;

	wm->fb = dev;
	init_list_head(&wm->list);
	spin_lock_init(&wm->lock);
	spin_lock_irqsave(&__window_manager_lock, flags);
	list_add_tail(&wm->entry, &__window_manager.entry);
	spin_unlock_irqrestore(&__window_manager_lock, flags);
	wm->cursor.cs = cairo_image_surface_create_from_png("/framework/assets/images/cursor.png");
	wm->cursor.width = cairo_image_surface_get_width(wm->cursor.cs);
	wm->cursor.height = cairo_image_surface_get_height(wm->cursor.cs);
	wm->cursor.ox = 0;
	wm->cursor.oy = 0;
	wm->cursor.nx = 0;
	wm->cursor.ny = 0;
	wm->cursor.dirty = 0;
	wm->cursor.show = 0;

	return wm;
}

void window_manager_free(struct window_manager_t * wm)
{
	struct window_manager_t * pos, * n;
	irq_flags_t flags;

	if(!wm)
		return;

	list_for_each_entry_safe(pos, n, &__window_manager.entry, entry)
	{
		if(pos == wm)
		{
			spin_lock_irqsave(&__window_manager_lock, flags);
			list_del(&pos->entry);
			spin_unlock_irqrestore(&__window_manager_lock, flags);
			cairo_surface_destroy(wm->cursor.cs);
			free(pos);
		}
	}
}

struct window_t * window_alloc(struct window_manager_t * wm)
{
	struct window_t * w;

	if(wm)
		return NULL;

	w = malloc(sizeof(struct window_t));
	if(!w)
		return NULL;

	w->wm = wm;
	w->rl = region_list_alloc(0);
	w->cs = cairo_xboot_surface_create(w->wm->fb);
	w->cr = cairo_create(w->cs);
	w->width = framebuffer_get_width(w->wm->fb);
	w->height = framebuffer_get_height(w->wm->fb);
	spin_lock(&wm->lock);
	list_add_tail(&w->entry, &wm->list);
	spin_unlock(&wm->lock);

	return w;
}

void window_free(struct window_t * w)
{
	if(!w || !w->wm)
		return;

	spin_lock(&w->wm->lock);
	list_del(&w->entry);
	spin_unlock(&w->wm->lock);

	cairo_destroy(w->cr);
	cairo_surface_destroy(w->cs);
	region_list_free(w->rl);
	free(w);
}

void window_to_front(struct window_t * w)
{
	if(w && w->wm && !list_is_last(&w->entry, &w->wm->list))
	{
		spin_lock(&w->wm->lock);
		list_move_tail(&w->entry, &w->wm->list);
		spin_unlock(&w->wm->lock);
	}
}

void window_to_back(struct window_t * w)
{
	if(w && w->wm && !list_is_first(&w->entry, &w->wm->list))
	{
		spin_lock(&w->wm->lock);
		list_move(&w->entry, &w->wm->list);
		spin_unlock(&w->wm->lock);
	}
}

void window_region_list_add(struct window_t * w, struct region_t * r)
{
	struct region_t region;

	if(w)
	{
		region_init(&region, 0, 0, w->width, w->height);
		if(region_intersect(&region, &region, r))
			region_list_add(w->rl, &region);
	}
}

void window_region_list_clear(struct window_t * w)
{
	if(w)
		region_list_clear(w->rl);
}

void window_present(struct window_t * w, void * o, void (*draw)(struct window_t *, void *))
{
	struct region_t * r;
	cairo_t * cr;
	int count;
	int i;

	if(w)
	{
		if((count = w->rl->count) > 0)
		{
			cr = w->cr;

			cairo_reset_clip(cr);
			for(i = 0; i < count; i++)
			{
				r = &w->rl->region[i];
				cairo_rectangle(cr, r->x, r->y, r->w, r->h);
			}
			cairo_clip(cr);
			cairo_save(cr);
			cairo_set_source_rgb(cr, 1, 1, 1);
			cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
			cairo_paint(cr);
			cairo_restore(cr);

			if(draw)
				draw(w, o);

			cairo_xboot_surface_present(w->cs, w->rl);
		}
	}
}
