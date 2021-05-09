/*
 * kernel/core/window.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
	return 1000;
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

static const unsigned char cursor_png[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x20,
	0x08, 0x03, 0x00, 0x00, 0x00, 0x21, 0x49, 0x78, 0xb9, 0x00, 0x00, 0x00,
	0xff, 0x50, 0x4c, 0x54, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb0,
	0xb0, 0xb0, 0x09, 0x09, 0x09, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x00,
	0x00, 0x00, 0xec, 0xec, 0xec, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0xad, 0xad, 0xad, 0xa9, 0xa9, 0xa9, 0xad,
	0xad, 0xad, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0xec, 0xec, 0xec, 0x96,
	0x96, 0x96, 0xbc, 0xbc, 0xbc, 0xb6, 0xb6, 0xb6, 0x1c, 0x1c, 0x1c, 0xb4,
	0xb4, 0xb4, 0xd8, 0xd8, 0xd8, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0xf5, 0xf5, 0xf5, 0xf4, 0xf4, 0xf4, 0xf3, 0xf3, 0xf3, 0xee,
	0xee, 0xee, 0xea, 0xea, 0xea, 0xf4, 0xf4, 0xf4, 0xeb, 0xeb, 0xeb, 0xf1,
	0xf1, 0xf1, 0xe2, 0xe2, 0xe2, 0xde, 0xde, 0xde, 0xa0, 0xa0, 0xa0, 0xbe,
	0xbe, 0xbe, 0xa0, 0xa0, 0xa0, 0xd1, 0xd1, 0xd1, 0x80, 0x80, 0x80, 0xc2,
	0xc2, 0xc2, 0x8b, 0x8b, 0x8b, 0xb3, 0xb3, 0xb3, 0x99, 0x99, 0x99, 0x3c,
	0x3c, 0x3c, 0xd4, 0xd4, 0xd4, 0xba, 0xba, 0xba, 0x61, 0x61, 0x61, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xb8, 0xb8, 0xb8, 0x03, 0x03, 0x03, 0x00,
	0x00, 0x00, 0xff, 0xff, 0xff, 0x5a, 0x5a, 0x5a, 0xfd, 0xfd, 0xfd, 0xfb,
	0xfb, 0xfb, 0xef, 0xef, 0xef, 0x38, 0x38, 0x38, 0xf4, 0xf4, 0xf4, 0xe4,
	0xe4, 0xe4, 0x35, 0x35, 0x35, 0x0c, 0x0c, 0x0c, 0x06, 0x06, 0x06, 0xf9,
	0xf9, 0xf9, 0xe0, 0xe0, 0xe0, 0x63, 0x63, 0x63, 0x11, 0x11, 0x11, 0xe8,
	0xe8, 0xe8, 0xe1, 0xe1, 0xe1, 0xc0, 0xc0, 0xc0, 0xab, 0xab, 0xab, 0xa6,
	0xa6, 0xa6, 0x91, 0x91, 0x91, 0x85, 0x85, 0x85, 0x76, 0x76, 0x76, 0x74,
	0x74, 0x74, 0x4b, 0x4b, 0x4b, 0x49, 0x49, 0x49, 0x33, 0x33, 0x33, 0x29,
	0x29, 0x29, 0x28, 0x28, 0x28, 0x15, 0x15, 0x15, 0xb8, 0xf0, 0xc7, 0x8a,
	0x00, 0x00, 0x00, 0x36, 0x74, 0x52, 0x4e, 0x53, 0x00, 0x03, 0x56, 0x0b,
	0x06, 0x09, 0x18, 0xfe, 0x76, 0x21, 0x11, 0x0f, 0x4d, 0x47, 0x3f, 0x2b,
	0x14, 0xe6, 0x89, 0x68, 0x5e, 0x4d, 0x36, 0x35, 0x32, 0x27, 0x1b, 0xfd,
	0xf7, 0xf7, 0xf7, 0xf2, 0xf1, 0xf0, 0xee, 0xd6, 0xbf, 0xa6, 0x9c, 0x90,
	0x8a, 0x7e, 0x7d, 0x7c, 0x6e, 0x61, 0x5c, 0x52, 0x4f, 0x4b, 0x41, 0x38,
	0x32, 0x24, 0x5e, 0xe3, 0xe1, 0x6a, 0x00, 0x00, 0x01, 0x13, 0x49, 0x44,
	0x41, 0x54, 0x28, 0xcf, 0x6d, 0xce, 0xe7, 0x6e, 0x83, 0x30, 0x18, 0x85,
	0x61, 0x1c, 0x33, 0x0a, 0x24, 0x81, 0xac, 0x66, 0x74, 0xef, 0xbd, 0x5b,
	0x4e, 0x80, 0x12, 0x20, 0xa3, 0x7b, 0xb7, 0xf7, 0x7f, 0x2d, 0xfd, 0x24,
	0x48, 0x42, 0x64, 0xbf, 0x7f, 0x90, 0x1e, 0x1d, 0x1b, 0x2b, 0x8a, 0xc2,
	0x14, 0x49, 0xec, 0xa2, 0xcd, 0x65, 0x7c, 0x19, 0x2c, 0x73, 0x09, 0x97,
	0x10, 0xdc, 0x72, 0x19, 0x23, 0xa8, 0x6a, 0x12, 0x1e, 0x87, 0x74, 0x8f,
	0xc8, 0xfd, 0xc9, 0x90, 0xf6, 0x22, 0x7b, 0x7f, 0xa1, 0x5f, 0xe5, 0x22,
	0x7b, 0x49, 0xe4, 0x57, 0x34, 0x91, 0xbd, 0x89, 0xe1, 0x57, 0xb8, 0xc8,
	0x5e, 0x1a, 0xa2, 0xcc, 0x45, 0xf6, 0x92, 0x07, 0xda, 0x0b, 0x4c, 0x19,
	0x28, 0x6b, 0x12, 0x4e, 0x06, 0x74, 0x8f, 0xc0, 0xd4, 0x00, 0x25, 0x55,
	0xc2, 0xe9, 0x23, 0x4a, 0xbc, 0xc8, 0xf3, 0x7d, 0x53, 0x5d, 0xe4, 0xdf,
	0x7b, 0xea, 0x3b, 0x44, 0x93, 0xb3, 0x22, 0xa7, 0xc8, 0x6b, 0xa8, 0x53,
	0x8e, 0x47, 0xe4, 0x6f, 0xd8, 0x3b, 0x5a, 0xa2, 0x4e, 0xad, 0x9c, 0x63,
	0xe0, 0x8b, 0xdc, 0xaf, 0x99, 0x3d, 0x5d, 0xd7, 0x5d, 0x2d, 0xe3, 0x4f,
	0xac, 0x6c, 0x18, 0xc4, 0x1f, 0x68, 0x14, 0xde, 0xfd, 0x82, 0xb5, 0xf3,
	0x3a, 0xc6, 0xe4, 0xc3, 0x75, 0xce, 0x66, 0x8c, 0xd5, 0x96, 0xa3, 0xd7,
	0x22, 0xe2, 0x3e, 0x8e, 0xed, 0x19, 0x6f, 0xb7, 0xee, 0x34, 0xfb, 0x0c,
	0xef, 0xe4, 0xa3, 0xc8, 0x62, 0x39, 0x6f, 0x5e, 0x9b, 0xaa, 0xc2, 0xf4,
	0x1d, 0x10, 0xc7, 0x38, 0xb4, 0x33, 0x6e, 0x3b, 0x3d, 0x95, 0xbe, 0xf6,
	0x15, 0x5e, 0xbd, 0x9f, 0x27, 0xc0, 0xcc, 0xe6, 0x5a, 0xf6, 0x17, 0xe6,
	0xee, 0xe2, 0x19, 0xd8, 0x3a, 0xa1, 0xa3, 0xc5, 0x78, 0x07, 0x30, 0xea,
	0x9d, 0x2e, 0xad, 0x8a, 0x31, 0x7d, 0xff, 0xe0, 0xc6, 0xb1, 0x68, 0xbb,
	0x98, 0xea, 0x76, 0x09, 0xc5, 0xd8, 0xfc, 0xf8, 0x3f, 0x8c, 0x18, 0x3b,
	0x03, 0x3e, 0x7b, 0x65, 0xcc, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
	0x44, 0xae, 0x42, 0x60, 0x82,
};

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

	s = surface_alloc_from_buf(cursor_png, sizeof(cursor_png));
	if(!s)
		return NULL;

	wm = malloc(sizeof(struct window_manager_t));
	if(!wm)
		return NULL;

	wm->fb = dev;
	wm->wcount = 0;
	wm->refresh = 0;
	wm->watermark.s = NULL;
	region_init(&wm->watermark.r, 0, 0, 0, 0);
	wm->cursor.s = s;
	wm->cursor.dirty = 0;
	wm->cursor.show = 0;
	region_init(&wm->cursor.ro, 0, 0, surface_get_width(wm->cursor.s), surface_get_height(wm->cursor.s));
	region_init(&wm->cursor.rn, 0, 0, surface_get_width(wm->cursor.s), surface_get_height(wm->cursor.s));
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
			surface_free(pos->watermark.s);
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
	struct window_manager_t * wm = w->wm;
	struct surface_t * s = w->s;
	struct region_t * r;
	struct matrix_t m;
	uint32_t * p, * q;
	int x1, y1, x2, y2;
	int l, x, y;
	int n, i;

	if(wm->refresh)
	{
		region_list_clear(w->rl);
		region_list_add(w->rl, &(struct region_t){ 0, 0, framebuffer_get_width(wm->fb), framebuffer_get_height(wm->fb) });
		wm->refresh = 0;
		wm->cursor.dirty = 0;
	}
	else
	{
		if(wm->watermark.s)
			window_region_list_add(w, &wm->watermark.r);
		if(wm->cursor.show && wm->cursor.dirty)
		{
			r = &wm->cursor.ro;
			window_region_list_add(w, &(struct region_t){ r->x - 2, r->y - 2, r->w, r->h });
			r = &wm->cursor.rn;
			window_region_list_add(w, &(struct region_t){ r->x - 2, r->y - 2, r->w, r->h });
			wm->cursor.dirty = 0;
		}
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
		if(wm->watermark.s)
		{
			r = &wm->watermark.r;
			matrix_init_translate(&m, r->x, r->y);
			surface_blit(s, NULL, &m, wm->watermark.s, RENDER_TYPE_GOOD);
		}
		if(wm->cursor.show)
		{
			r = &wm->cursor.rn;
			matrix_init_translate(&m, r->x - 2, r->y - 2);
			surface_blit(s, NULL, &m, wm->cursor.s, RENDER_TYPE_GOOD);
		}
	}
	framebuffer_present_surface(wm->fb, w->s, w->rl);
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
					if(e->e.key_down.key == KEY_HOME)
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
