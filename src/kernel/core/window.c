/*
 * kernel/core/window.c
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
#include <input/input.h>
#include <input/keyboard.h>
#include <xboot/window.h>

static void fb_dummy_setbl(struct framebuffer_t * fb, int brightness)
{
}

static int fb_dummy_getbl(struct framebuffer_t * fb)
{
	return CONFIG_MAX_BRIGHTNESS;
}

static struct surface_t * fb_dummy_create(struct framebuffer_t * fb)
{
	return surface_alloc(fb->width, fb->height, NULL);
}

static void fb_dummy_destroy(struct framebuffer_t * fb, struct surface_t * s)
{
	surface_free(s);
}

static void fb_dummy_present(struct framebuffer_t * fb, struct surface_t * s, struct region_list_t * rl)
{
}

static struct framebuffer_t fb_dummy = {
	.name		= "fb-dummy",
	.width		= 640,
	.height		= 480,
	.pwidth		= 216,
	.pheight	= 135,
	.setbl		= fb_dummy_setbl,
	.getbl		= fb_dummy_getbl,
	.create		= fb_dummy_create,
	.destroy	= fb_dummy_destroy,
	.present	= fb_dummy_present,
	.priv		= NULL,
};

struct list_head __window_manager_list = {
	.next = &__window_manager_list,
	.prev = &__window_manager_list,
};
static spinlock_t __window_manager_lock = SPIN_LOCK_INIT();

static inline struct window_manager_t * window_manager_search(struct framebuffer_t * fb)
{
	struct window_manager_t * pos, * n;

	if(fb)
	{
		list_for_each_entry_safe(pos, n, &__window_manager_list, list)
		{
			if(pos->fb == fb)
				return pos;
		}
	}
	return NULL;
}

static const uint32_t arraw_cursor[32][22] = {
	{ 0x00000000, 0x00000000, 0x03000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x52444444, 0x352d2d2d, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x4f3a3a3a, 0xfffbfbfb, 0x36262626, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xfffbfbfb, 0x32242424, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xffffffff, 0xfffbfbfb, 0x3f2b2b2b, 0x09000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff636363, 0xffefefef, 0xfffbfbfb, 0x36262626, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff111111, 0xfff9f9f9, 0xfffbfbfb, 0x3f2b2b2b, 0x09000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff151515, 0xffefefef, 0xfffdfdfd, 0x3f2b2b2b, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff0c0c0c, 0xfff4f4f4, 0xfffdfdfd, 0x472f2f2f, 0x09000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff000000, 0xff111111, 0xfeebebeb, 0xfffdfdfd, 0x472f2f2f, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff060606, 0xffefefef, 0xffffffff, 0x4d343434, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff0c0c0c, 0xffe8e8e8, 0xfffdfdfd, 0x472f2f2f, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xfeebebeb, 0xffffffff, 0x4d343434, 0x09000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff0c0c0c, 0xffe4e4e4, 0xffffffff, 0x4d343434, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xffe4e4e4, 0xffffffff, 0x563b3b3b, 0x06000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff060606, 0xffe1e1e1, 0xffffffff, 0x563b3b3b, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xffe4e4e4, 0xffffffff, 0x5e434343, 0x06000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff282828, 0xff383838, 0xff383838, 0xff383838, 0xff383838, 0xff333333, 0xffefefef, 0xffffffff, 0x5e434343, 0x0b000000, 0x03000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff060606, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x684d4d4d, 0x06000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff000000, 0xff353535, 0xffe0e0e0, 0xff000000, 0xff000000, 0xff000000, 0xff858585, 0xffffffff, 0xa6686868, 0x7e3f3f3f, 0x763f3f3f, 0x763f3f3f, 0x763f3f3f, 0x763f3f3f, 0x6e4d4d4d, 0x11000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff000000, 0xff353535, 0xffffffff, 0xffffffff, 0xff494949, 0xff000000, 0xff000000, 0xff000000, 0xfffdfdfd, 0xe6d5d5d5, 0x21000000, 0x1b000000, 0x18000000, 0x18000000, 0x18000000, 0x14000000, 0x09000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xff5a5a5a, 0xff4b4b4b, 0xffffffff, 0xfdf3f3f3, 0xf2dedede, 0xfeebebeb, 0xff000000, 0xff000000, 0xff000000, 0xff747474, 0xffffffff, 0x684d4d4d, 0x0b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xffa6a6a6, 0xffffffff, 0xf7ebebeb, 0x5c161616, 0x89515151, 0xffffffff, 0xff636363, 0xff000000, 0xff000000, 0xff000000, 0xfff9f9f9, 0xf7e7e7e7, 0x0b000000, 0x03000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xffffffff, 0xf7ececec, 0x4d080808, 0x2b020202, 0x18000000, 0xe6d5d5d5, 0xfff4f4f4, 0xff000000, 0xff000000, 0xff000000, 0xff5a5a5a, 0xffffffff, 0x7d5f5f5f, 0x0f000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xffffffff, 0xeee1e1e1, 0x4d080808, 0x24000000, 0x09000000, 0x0f000000, 0x613a3a3a, 0xffffffff, 0xff767676, 0xff000000, 0xff000000, 0xff000000, 0xffe0e0e0, 0xfff4f4f4, 0x11000000, 0x03000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x563b3b3b, 0xf1e7e7e7, 0x41000000, 0x21000000, 0x06000000, 0x00000000, 0x03000000, 0x18000000, 0xd6bebebe, 0xffffffff, 0xff000000, 0xff000000, 0xff000000, 0xff353535, 0xffffffff, 0x8a717171, 0x0f000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x03000000, 0x352d2d2d, 0x2b020202, 0x1b000000, 0x06000000, 0x00000000, 0x00000000, 0x00000000, 0x0f000000, 0x4b1d1d1d, 0xffffffff, 0xff919191, 0xff000000, 0xff292929, 0xffc0c0c0, 0xffffffff, 0xffffffff, 0x21000000, 0x03000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x00000000, 0x09000000, 0x0b000000, 0x03000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x03000000, 0x14000000, 0xbfa6a6a6, 0xffffffff, 0xffababab, 0xffffffff, 0xffffffff, 0xf0dddddd, 0x7c444444, 0x21000000, 0x06000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0b000000, 0x32010101, 0xffffffff, 0xffffffff, 0xfeebebeb, 0x89515151, 0x32010101, 0x27000000, 0x0b000000, 0x03000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x03000000, 0x18000000, 0x9c747474, 0x905a5a5a, 0x38000000, 0x2b020202, 0x11000000, 0x06000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x06000000, 0x14000000, 0x27000000, 0x11000000, 0x06000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x03000000, 0x03000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, },
};

static inline struct surface_t * surface_alloc_cursor(void)
{
	struct surface_t * s = surface_alloc(22, 32, NULL);
	if(s)
		memcpy(s->pixels, arraw_cursor, sizeof(arraw_cursor));
	return s;
}

static struct window_manager_t * window_manager_alloc(const char * fb)
{
	struct window_manager_t * wm;
	struct framebuffer_t * dev;
	struct surface_t * s;
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

	s = surface_alloc_cursor();
	if(!s)
		return NULL;

	wm = malloc(sizeof(struct window_manager_t));
	if(!wm)
		return NULL;

	wm->fb = dev;
	wm->wcount = 0;
	wm->refresh = 0;
	wm->cursor.s = s;
	region_init(&wm->cursor.ro, 0, 0, surface_get_width(wm->cursor.s), surface_get_height(wm->cursor.s));
	region_init(&wm->cursor.rn, 0, 0, surface_get_width(wm->cursor.s), surface_get_height(wm->cursor.s));
	wm->cursor.dirty = 0;
	wm->cursor.show = 0;
	spin_lock_init(&wm->lock);
	init_list_head(&wm->list);
	init_list_head(&wm->window);
	spin_lock_irqsave(&__window_manager_lock, flags);
	list_add_tail(&wm->list, &__window_manager_list);
	spin_unlock_irqrestore(&__window_manager_lock, flags);

	return wm;
}

static void window_manager_free(struct window_manager_t * wm)
{
	struct window_manager_t * pos, * n;
	irq_flags_t flags;

	if(!wm)
		return;

	list_for_each_entry_safe(pos, n, &__window_manager_list, list)
	{
		if(pos == wm)
		{
			spin_lock_irqsave(&__window_manager_lock, flags);
			list_del(&pos->list);
			spin_unlock_irqrestore(&__window_manager_lock, flags);
			surface_free(pos->cursor.s);
			free(pos);
		}
	}
}

struct window_t * window_alloc(const char * fb, const char * input)
{
	struct window_manager_t * wm = window_manager_alloc(fb);
	struct window_t * w;
	struct input_t * dev;
	struct device_t * pos, * n;
	char * r, * p = (char *)input;
	int range[2];

	if(!wm)
		return NULL;

	w = malloc(sizeof(struct window_t));
	if(!w)
		return NULL;

	w->task = task_self();
	w->wm = wm;
	w->s = framebuffer_create_surface(w->wm->fb);
	w->rl = region_list_alloc(0);
	w->event = fifo_alloc(sizeof(struct event_t) * CONFIG_EVENT_FIFO_SIZE);
	w->launcher = 0;
	if(p)
	{
		w->map = hmap_alloc(0);
		while((r = strsep(&p, ",;:|")) != NULL)
		{
			dev = search_input(r);
			if(dev)
			{
				hmap_add(w->map, r, dev);
				if(input_ioctl(dev, "mouse-get-range", &range[0]) >= 0)
				{
					range[0] = framebuffer_get_width(w->wm->fb);
					range[1] = framebuffer_get_height(w->wm->fb);
					input_ioctl(dev, "mouse-set-range", &range[0]);
				}
			}
		}
	}
	else
	{
		w->map = NULL;
		list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_INPUT], head)
		{
			if(input_ioctl(pos->priv, "mouse-get-range", &range[0]) >= 0)
			{
				range[0] = framebuffer_get_width(w->wm->fb);
				range[1] = framebuffer_get_height(w->wm->fb);
				input_ioctl(pos->priv, "mouse-set-range", &range[0]);
			}
		}
	}
	spin_lock(&wm->lock);
	list_add(&w->list, &wm->window);
	wm->wcount++;
	wm->refresh = 1;
	spin_unlock(&wm->lock);

	return w;
}

void window_free(struct window_t * w)
{
	if(!w || !w->wm)
		return;

	spin_lock(&w->wm->lock);
	list_del(&w->list);
	w->wm->wcount--;
	w->wm->refresh = 1;
	spin_unlock(&w->wm->lock);
	if(w->wm->wcount <= 0)
		window_manager_free(w->wm);
	fifo_free(w->event);
	hmap_free(w->map, NULL);
	framebuffer_destroy_surface(w->wm->fb, w->s);
	region_list_free(w->rl);
	free(w);
}

void window_to_front(struct window_t * w)
{
	if(w && w->wm && !list_is_first(&w->list, &w->wm->window))
	{
		spin_lock(&w->wm->lock);
		list_move(&w->list, &w->wm->window);
		w->wm->refresh = 1;
		spin_unlock(&w->wm->lock);
	}
}

void window_to_back(struct window_t * w)
{
	if(w && w->wm && !list_is_last(&w->list, &w->wm->window))
	{
		spin_lock(&w->wm->lock);
		list_move_tail(&w->list, &w->wm->window);
		w->wm->refresh = 1;
		spin_unlock(&w->wm->lock);
	}
}

void window_region_list_add(struct window_t * w, struct region_t * r)
{
	struct region_t region;

	if(w)
	{
		region_init(&region, 0, 0, framebuffer_get_width(w->wm->fb), framebuffer_get_height(w->wm->fb));
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
	struct surface_t * s = w->s;
	struct region_t * r, region;
	struct matrix_t m;
	uint32_t * p, * q;
	int x1, y1, x2, y2;
	int l, x, y;
	int n, i;

	if(w->wm->refresh)
	{
		region_init(&region, 0, 0, framebuffer_get_width(w->wm->fb), framebuffer_get_height(w->wm->fb));
		region_list_clear(w->rl);
		region_list_add(w->rl, &region);
		w->wm->refresh = 0;
		w->wm->cursor.dirty = 0;
	}
	else if(w->wm->cursor.show && w->wm->cursor.dirty)
	{
		r = &w->wm->cursor.ro;
		window_region_list_add(w, &(struct region_t){ r->x - 2, r->y - 2, r->w, r->h });
		r = &w->wm->cursor.rn;
		window_region_list_add(w, &(struct region_t){ r->x - 2, r->y - 2, r->w, r->h });
		w->wm->cursor.dirty = 0;
	}
	if((n = w->rl->count) > 0)
	{
		l = s->stride >> 2;
		for(i = 0; i < n; i++)
		{
			r = &w->rl->region[i];
			x1 = r->x;
			y1 = r->y;
			x2 = r->x + r->w;
			y2 = r->y + r->h;
			q = (uint32_t *)s->pixels + y1 * l + x1;
			for(y = y1; y < y2; y++, q += l)
			{
				for(x = x1, p = q; x < x2; x++, p++)
				{
					if((x ^ y) & (1 << 3))
						*p = 0xffabb9bd;
					else
						*p = 0xff899598;
				}
			}
		}
		if(draw)
			draw(w, o);
		if(w->wm->cursor.show)
		{
			r = &w->wm->cursor.rn;
			matrix_init_translate(&m, r->x - 2, r->y - 2);
			surface_blit(s, NULL, &m, w->wm->cursor.s, RENDER_TYPE_FAST);
		}
	}
	framebuffer_present_surface(w->wm->fb, w->s, w->rl);
}

void window_exit(struct window_t * w)
{
	struct event_t e;

	if(w)
	{
		e.device = &(struct input_t){ "system", NULL, NULL };
		e.type = EVENT_TYPE_SYSTEM_EXIT;
		e.timestamp = ktime_get();
		fifo_put(w->event, (unsigned char *)&e, sizeof(struct event_t));
	}
}

int window_pump_event(struct window_t * w, struct event_t * e)
{
	if(w && (fifo_get(w->event, (unsigned char *)e, sizeof(struct event_t)) == sizeof(struct event_t)))
		return 1;
	return 0;
}

void push_event(struct event_t * e)
{
	struct window_manager_t * pos, * n;
	struct window_t * w;

	if(e)
	{
		e->timestamp = ktime_get();
		list_for_each_entry_safe(pos, n, &__window_manager_list, list)
		{
			w = (struct window_t *)list_first_entry_or_null(&pos->window, struct window_t, list);
			if(w && (!w->map || hmap_search(w->map, ((struct input_t *)e->device)->name)))
			{
				fifo_put(w->event, (unsigned char *)e, sizeof(struct event_t));
				switch(e->type)
				{
				case EVENT_TYPE_KEY_DOWN:
					if((e->e.key_down.key == KEY_TASK) || (e->e.key_down.key == KEY_HOME))
					{
						struct window_t * wpos, * wn;
						list_for_each_entry_safe(wpos, wn, &pos->window, list)
						{
							if(wpos->launcher)
							{
								window_to_front(wpos);
								break;
							}
						}
					}
					break;
				case EVENT_TYPE_KEY_UP:
					break;

				case EVENT_TYPE_MOUSE_DOWN:
					if(!pos->cursor.dirty)
					{
						region_clone(&pos->cursor.ro, &pos->cursor.rn);
						pos->cursor.dirty = 1;
					}
					region_init(&pos->cursor.rn, e->e.mouse_down.x, e->e.mouse_down.y, pos->cursor.rn.w, pos->cursor.rn.h);
					pos->cursor.show = 1;
					break;
				case EVENT_TYPE_MOUSE_MOVE:
					if(!pos->cursor.dirty)
					{
						region_clone(&pos->cursor.ro, &pos->cursor.rn);
						pos->cursor.dirty = 1;
					}
					region_init(&pos->cursor.rn, e->e.mouse_move.x, e->e.mouse_move.y, pos->cursor.rn.w, pos->cursor.rn.h);
					pos->cursor.show = 1;
					break;
				case EVENT_TYPE_MOUSE_UP:
					if(!pos->cursor.dirty)
					{
						region_clone(&pos->cursor.ro, &pos->cursor.rn);
						pos->cursor.dirty = 1;
					}
					region_init(&pos->cursor.rn, e->e.mouse_up.x, e->e.mouse_up.y, pos->cursor.rn.w, pos->cursor.rn.h);
					pos->cursor.show = 1;
					break;
				case EVENT_TYPE_MOUSE_WHEEL:
					pos->cursor.show = 1;
					break;

				case EVENT_TYPE_TOUCH_BEGIN:
					pos->cursor.show = 0;
					break;
				case EVENT_TYPE_TOUCH_MOVE:
					pos->cursor.show = 0;
					break;
				case EVENT_TYPE_TOUCH_END:
					pos->cursor.show = 0;
					break;

				default:
					break;
				}
			}
		}
	}
}
