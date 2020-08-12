/*
 * kernel/xui/xui.c
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
#include <xui/xui.h>

static const struct xui_style_t xui_style_default = {
	.primary = {
		.normal = {
			.background = { 0x53, 0x6d, 0xe6, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.hover = {
			.background = { 0x3a, 0x57, 0xe2, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.focus = {
			.background = { 0x26, 0x47, 0xe0, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x53, 0x6d, 0xe6, 0x60 },
		},
	},
	.secondary = {
		.normal = {
			.background = { 0x6c, 0x75, 0x7d, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.hover = {
			.background = { 0x5a, 0x62, 0x68, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.focus = {
			.background = { 0x54, 0x5b, 0x62, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x6c, 0x75, 0x7d, 0x60 },
		},
	},
	.success = {
		.normal = {
			.background = { 0x10, 0xc4, 0x69, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.hover = {
			.background = { 0x0d, 0xa1, 0x56, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.focus = {
			.background = { 0x0c, 0x95, 0x50, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x10, 0xc4, 0x69, 0x60 },
		},
	},
	.info = {
		.normal = {
			.background = { 0x35, 0xb8, 0xe0, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.hover = {
			.background = { 0x20, 0xa6, 0xcf, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
		},
		.focus = {
			.background = { 0x1e, 0x9d, 0xc4, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x35, 0xb8, 0xe0, 0x60 },
		},
	},
	.warning = {
		.normal = {
			.background = { 0xf9, 0xc8, 0x51, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.hover = {
			.background = { 0xf8, 0xbc, 0x2c, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.focus = {
			.background = { 0xf7, 0xb8, 0x20, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0xf9, 0xc8, 0x51, 0x60 },
		},
	},
	.danger = {
		.normal = {
			.background = { 0xff, 0x5b, 0x5b, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.hover = {
			.background = { 0xff, 0x35, 0x35, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.focus = {
			.background = { 0xff, 0x28, 0x28, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0xff, 0x5b, 0x5b, 0x60 },
		},
	},
	.light = {
		.normal = {
			.background = { 0xee, 0xf2, 0xf7, 0xff },
			.foreground = { 0x32, 0x3a, 0x46, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.hover = {
			.background = { 0xd4, 0xde, 0xeb, 0xff },
			.foreground = { 0x32, 0x3a, 0x46, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.focus = {
			.background = { 0xcb, 0xd7, 0xe7, 0xff },
			.foreground = { 0x32, 0x3a, 0x46, 0xff },
			.border = { 0xee, 0xf2, 0xf7, 0x60 },
		},
	},
	.dark = {
		.normal = {
			.background = { 0x32, 0x3a, 0x46, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.hover = {
			.background = { 0x22, 0x28, 0x30, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x00, 0x00, 0x00, 0x00 },
		},
		.focus = {
			.background = { 0x1d, 0x21, 0x28, 0xff },
			.foreground = { 0xff, 0xff, 0xff, 0xff },
			.border = { 0x32, 0x3a, 0x46, 0x60 },
		},
	},

	.font = {
		.icon_family = "font-awesome",
		.font_family = "roboto",
		.color = { 0x6c, 0x75, 0x7d, 0xff },
		.size = 16,
	},

	.layout = {
		.width = 64,
		.height = 16,
		.padding = 4,
		.spacing = 4,
		.indent = 24,
	},

	.window = {
		.close_icon = 0xf057,
		.border_radius = 4,
		.border_width = 4,
		.title_height = 24,
		.face_color = { 0xff, 0xff, 0xff, 0xff },
		.border_color = { 0x26, 0x47, 0xe0, 0xff },
		.title_color = { 0x26, 0x47, 0xe0, 0xff },
		.text_color = { 0xff, 0xff, 0xff, 0xff },
	},

	.scroll = {
		.scroll_size = 12,
		.scroll_radius = 6,
		.thumb_size = 8,
		.thumb_radius = 6,
		.scroll_color = { 0xd1, 0xd6, 0xdb, 0xff },
		.thumb_color = { 0xb1, 0xb6, 0xba, 0xff },
	},

	.collapse = {
		.border_radius = 2,
		.border_width = 0,
	},

	.tree = {
		.collapsed_icon = 0xf067,
		.expanded_icon = 0xf068,
		.border_radius = 0,
		.border_width = 1,
		.normal = {
			.face_color = { 0xc4, 0x18, 0x3c, 0xff },
			.border_color = { 0x00, 0x00, 0x00, 0x00 },
			.text_color = { 0x6c, 0x75, 0x7d, 0xff },
		},
		.hover = {
			.face_color = { 0xad, 0x15, 0x35, 0xff },
			.border_color = { 0x00, 0x00, 0x00, 0x00 },
			.text_color = { 0x6c, 0x75, 0x7d, 0xff },
		},
		.focus = {
			.face_color = { 0xad, 0x15, 0x35, 0xff },
			.border_color = { 0xad, 0x15, 0x35, 0x60 },
			.text_color = { 0x6c, 0x75, 0x7d, 0xff },
		},
	},

	.button = {
		.border_radius = 4,
		.border_width = 4,
		.outline_width = 2,
	},

	.checkbox = {
		.check_icon = 0xf00c,
		.border_radius = 4,
		.border_width = 4,
		.outline_width = 2,
	},

	.radio = {
		.border_width = 4,
		.outline_width = 2,
	},

	.toggle = {
		.border_width = 4,
		.outline_width = 2,
	},

	.slider = {
		.invalid = { 0xee, 0xf2, 0xf7, 0xff },
		.border_width = 4,
	},

	.number = {
		.border_radius = 4,
		.border_width = 4,
		.outline_width = 2,
	},

	.textedit = {
		.border_radius = 4,
		.border_width = 4,
		.outline_width = 2,
	},

	.badge = {
		.border_radius = 4,
		.border_width = 4,
		.outline_width = 2,
	},

	.progress = {
		.invalid = { 0xee, 0xf2, 0xf7, 0xff },
		.border_radius = 4,
	},

	.radialbar = {
		.invalid = { 0xee, 0xf2, 0xf7, 0xff },
		.width = 8,
	},

	.spinner = {
		.width = 4,
	},

	.split = {
		.width = 2,
	},
};

static struct region_t unlimited_region = {
	.x = 0,
	.y = 0,
	.w = INT_MAX,
	.h = INT_MAX,
};

void xui_begin(struct xui_context_t * ctx)
{
	ctx->cmd_list.idx = 0;
	ctx->root_list.idx = 0;
	ctx->scroll_target = NULL;
	ctx->hover_root = ctx->next_hover_root;
	ctx->next_hover_root = NULL;
	ctx->mouse.dx = ctx->mouse.x - ctx->mouse.ox;
	ctx->mouse.dy = ctx->mouse.y - ctx->mouse.oy;
	ctx->mouse.ox = ctx->mouse.x;
	ctx->mouse.oy = ctx->mouse.y;
	ctx->now = ktime_to_ns(ktime_get());
	ctx->delta = ctx->now - ctx->last;
	ctx->last = ctx->now;
	ctx->frame++;
	if(ctx->delta > 0)
		ctx->fps = 1000000000.0 / ctx->delta * 0.382 + ctx->fps * 0.618;
}

static int compare_zindex(const void * a, const void * b)
{
	return (*(struct xui_container_t **)a)->zindex - (*(struct xui_container_t **)b)->zindex;
}

static void xui_hash(unsigned int * h, const void * buf, int size)
{
	const unsigned char * p = buf;
	while(size--)
		*h = (*h << 5) + *h + (*p++);
}

static int xui_cmd_next(struct xui_context_t * ctx, union xui_cmd_t ** cmd)
{
	if(*cmd)
		*cmd = (union xui_cmd_t *)(((char *)*cmd) + (*cmd)->base.len);
	else
		*cmd = (union xui_cmd_t *)ctx->cmd_list.items;
	while((char *)(*cmd) != ctx->cmd_list.items + ctx->cmd_list.idx)
	{
		if((*cmd)->base.type != XUI_CMD_TYPE_JUMP)
			return 1;
		*cmd = (*cmd)->jump.addr;
	}
	return 0;
}

void xui_end(struct xui_context_t * ctx)
{
	union xui_cmd_t * cmd = NULL;
	struct region_t r;
	unsigned int * ncell = ctx->cells[ctx->cindex];
	unsigned int * ocell = ctx->cells[(ctx->cindex = (ctx->cindex + 1) & 0x1)];
	unsigned int h;
	int x1, y1, x2, y2;
	int x, y;
	int i, n;

	assert(ctx->container_stack.idx == 0);
	assert(ctx->clip_stack.idx == 0);
	assert(ctx->id_stack.idx == 0);
	assert(ctx->layout_stack.idx == 0);
	if(ctx->scroll_target)
	{
		ctx->scroll_target->scroll_x += ctx->mouse.zx;
		ctx->scroll_target->scroll_y += ctx->mouse.zy;
	}
	if(!ctx->updated_focus)
		ctx->focus = 0;
	ctx->updated_focus = 0;
	if(ctx->mouse.down && ctx->next_hover_root && (ctx->next_hover_root->zindex < ctx->last_zindex) && (ctx->next_hover_root->zindex >= 0))
		xui_set_front(ctx, ctx->next_hover_root);
	ctx->mouse.down = 0;
	ctx->mouse.up = 0;
	ctx->mouse.zx = 0;
	ctx->mouse.zy = 0;
	ctx->key_pressed = 0;
	ctx->input_text[0] = '\0';
	n = ctx->root_list.idx;
	qsort(ctx->root_list.items, n, sizeof(struct xui_container_t *), compare_zindex);
	for(i = 0; i < n; i++)
	{
		struct xui_container_t * c = ctx->root_list.items[i];
		if(i == 0)
		{
			union xui_cmd_t * cmd = (union xui_cmd_t *)ctx->cmd_list.items;
			cmd->jump.addr = (char *)c->head + sizeof(struct xui_cmd_jump_t);
		}
		else
		{
			struct xui_container_t * prev = ctx->root_list.items[i - 1];
			prev->tail->jump.addr = (char *)c->head + sizeof(struct xui_cmd_jump_t);
		}
		if(i == n - 1)
			c->tail->jump.addr = ctx->cmd_list.items + ctx->cmd_list.idx;
	}
	while(xui_cmd_next(ctx, &cmd))
	{
		if(region_intersect(&r, &ctx->screen, &cmd->base.r))
		{
			h = 5381;
			xui_hash(&h, &cmd->base, cmd->base.len);
			x1 = r.x >> ctx->cpshift;
			y1 = r.y >> ctx->cpshift;
			x2 = (r.x + r.w) >> ctx->cpshift;
			y2 = (r.y + r.h) >> ctx->cpshift;
			for(y = y1; y <= y2; y++)
			{
				for(x = x1; x <= x2; x++)
				{
					xui_hash(&ncell[x + y * ctx->cwidth], &h, sizeof(unsigned int));
				}
			}
		}
	}
	region_list_clear(ctx->w->rl);
	for(y = 0; y < ctx->cheight; y++)
	{
		for(x = 0; x < ctx->cwidth; x++)
		{
			i = x + y * ctx->cwidth;
			if(ncell[i] != ocell[i])
			{
				region_init(&r, x << ctx->cpshift, y << ctx->cpshift, 1 << ctx->cpshift, 1 << ctx->cpshift);
				if(region_intersect(&r, &r, &ctx->screen))
					region_list_add(ctx->w->rl, &r);
			}
			ocell[i] = 5381;
		}
	}
}

const char * xui_format(struct xui_context_t * ctx, const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(ctx->fmtbuf, sizeof(ctx->fmtbuf), fmt, ap);
	va_end(ap);
	return (const char *)ctx->fmtbuf;
}

void xui_set_front(struct xui_context_t * ctx, struct xui_container_t * c)
{
	c->zindex = ++ctx->last_zindex;
}

void xui_set_focus(struct xui_context_t * ctx, unsigned int id)
{
	ctx->focus = id;
	ctx->updated_focus = 1;
}

unsigned int xui_get_id(struct xui_context_t * ctx, const void * data, int size)
{
	int idx = ctx->id_stack.idx;
	unsigned int h = (idx > 0) ? ctx->id_stack.items[idx - 1] : 5381;
	xui_hash(&h, data, size);
	ctx->last_id = h;
	return h;
}

void xui_push_id(struct xui_context_t * ctx, const void * data, int size)
{
	xui_push(ctx->id_stack, xui_get_id(ctx, data, size));
}

void xui_pop_id(struct xui_context_t * ctx)
{
	xui_pop(ctx->id_stack);
}

struct region_t * xui_get_clip(struct xui_context_t * ctx)
{
	assert(ctx->clip_stack.idx > 0);
	return &ctx->clip_stack.items[ctx->clip_stack.idx - 1];
}

void xui_push_clip(struct xui_context_t * ctx, struct region_t * r)
{
	struct region_t region;
	if(!region_intersect(&region, r, xui_get_clip(ctx)))
		region_init(&region, 0, 0, 0, 0);
	xui_push(ctx->clip_stack, region);
}

void xui_pop_clip(struct xui_context_t * ctx)
{
	xui_pop(ctx->clip_stack);
}

int xui_pool_init(struct xui_context_t * ctx, struct xui_pool_item_t * items, int len, unsigned int id)
{
	int i, n = -1, f = ctx->frame;
	for(i = 0; i < len; i++)
	{
		if(items[i].last_update < f)
		{
			f = items[i].last_update;
			n = i;
		}
	}
	assert(n > -1);
	items[n].id = id;
	items[n].last_update = ctx->frame;
	return n;
}

int xui_pool_get(struct xui_context_t * ctx, struct xui_pool_item_t * items, int len, unsigned int id)
{
	int i;
	for(i = 0; i < len; i++)
	{
		if(items[i].id == id)
			return i;
	}
	return -1;
}

void xui_pool_update(struct xui_context_t * ctx, struct xui_pool_item_t * items, int idx)
{
	items[idx].last_update = ctx->frame;
}

static void push_layout(struct xui_context_t * ctx, struct region_t * body, int scrollx, int scrolly)
{
	struct xui_layout_t layout;
	memset(&layout, 0, sizeof(layout));
	region_init(&layout.body, body->x - scrollx, body->y - scrolly, body->w, body->h);
	layout.max_width = INT_MIN;
	layout.max_height = INT_MIN;
	xui_push(ctx->layout_stack, layout);
	xui_layout_row(ctx, 1, (int[]){ 0 }, 0);
}

struct xui_layout_t * xui_get_layout(struct xui_context_t * ctx)
{
	return &ctx->layout_stack.items[ctx->layout_stack.idx - 1];
}

void pop_container(struct xui_context_t * ctx)
{
	struct xui_container_t * c = xui_get_current_container(ctx);
	struct xui_layout_t * layout = xui_get_layout(ctx);
	c->content_width = layout->max_width - layout->body.x;
	c->content_height = layout->max_height - layout->body.y;
	xui_pop(ctx->container_stack);
	xui_pop(ctx->layout_stack);
	xui_pop_id(ctx);
}

struct xui_container_t * get_container(struct xui_context_t * ctx, unsigned int id, int opt)
{
	struct xui_container_t * c;
	int idx = xui_pool_get(ctx, ctx->container_pool, XUI_CONTAINER_POOL_SIZE, id);
	if(idx >= 0)
	{
		if(ctx->containers[idx].open || (~opt & XUI_OPT_CLOSED))
			xui_pool_update(ctx, ctx->container_pool, idx);
		return &ctx->containers[idx];
	}
	if(opt & XUI_OPT_CLOSED)
		return NULL;
	idx = xui_pool_init(ctx, ctx->container_pool, XUI_CONTAINER_POOL_SIZE, id);
	c = &ctx->containers[idx];
	memset(c, 0, sizeof(struct xui_container_t));
	c->open = 1;
	xui_set_front(ctx, c);
	return c;
}

struct xui_container_t * xui_get_container(struct xui_context_t * ctx, const char * name)
{
	unsigned int id = xui_get_id(ctx, name, strlen(name));
	return get_container(ctx, id, 0);
}

struct xui_container_t * xui_get_current_container(struct xui_context_t * ctx)
{
	assert(ctx->container_stack.idx > 0);
	return ctx->container_stack.items[ctx->container_stack.idx - 1];
}

void xui_layout_width(struct xui_context_t * ctx, int width)
{
	xui_get_layout(ctx)->size_width = width;
}

void xui_layout_height(struct xui_context_t * ctx, int height)
{
	xui_get_layout(ctx)->size_height = height;
}

void xui_layout_row(struct xui_context_t * ctx, int items, const int * widths, int height)
{
	struct xui_layout_t * layout = xui_get_layout(ctx);
	if(widths)
	{
		assert(items <= XUI_MAX_WIDTHS);
		memcpy(layout->widths, widths, items * sizeof(widths[0]));
	}
	layout->items = items;
	layout->position_x = layout->indent;
	layout->position_y = layout->next_row;
	layout->size_height = height;
	layout->item_index = 0;
}

void xui_layout_begin_column(struct xui_context_t * ctx)
{
	push_layout(ctx, xui_layout_next(ctx), 0, 0);
}

void xui_layout_end_column(struct xui_context_t * ctx)
{
	struct xui_layout_t * a, * b;
	b = xui_get_layout(ctx);
	xui_pop(ctx->layout_stack);
	a = xui_get_layout(ctx);
	a->position_x = max(a->position_x, b->position_x + b->body.x - a->body.x);
	a->next_row = max(a->next_row, b->next_row + b->body.y - a->body.y);
	a->max_width = max(a->max_width, b->max_width);
	a->max_height = max(a->max_height, b->max_height);
}

void xui_layout_set_next(struct xui_context_t * ctx, struct region_t * r, int relative)
{
	struct xui_layout_t * layout = xui_get_layout(ctx);
	region_clone(&layout->next, r);
	layout->next_type = relative ? 1 : 2;
}

struct region_t * xui_layout_next(struct xui_context_t * ctx)
{
	struct xui_layout_t * layout = xui_get_layout(ctx);
	struct xui_style_t * style = &ctx->style;
	struct region_t r;

	if(layout->next_type)
	{
		int type = layout->next_type;
		layout->next_type = 0;
		region_clone(&r, &layout->next);
		if(type == 2)
		{
			region_clone(&ctx->last_rect, &r);
			return &ctx->last_rect;
		}
	}
	else
	{
		if(layout->item_index == layout->items)
			xui_layout_row(ctx, layout->items, NULL, layout->size_height);
		r.x = layout->position_x;
		r.y = layout->position_y;
		r.w = layout->items > 0 ? layout->widths[layout->item_index] : layout->size_width;
		r.h = layout->size_height;
		if(r.w == 0)
			r.w = style->layout.width + style->layout.padding * 2;
		if(r.h == 0)
			r.h = style->layout.height + style->layout.padding * 2;
		if(r.w < 0)
			r.w += layout->body.w - r.x + 1;
		if(r.h < 0)
			r.h += layout->body.h - r.y + 1;
		layout->item_index++;
	}

	layout->position_x += r.w + style->layout.spacing;
	layout->next_row = max(layout->next_row, r.y + r.h + style->layout.spacing);
	r.x += layout->body.x;
	r.y += layout->body.y;
	layout->max_width = max(layout->max_width, r.x + r.w);
	layout->max_height = max(layout->max_height, r.y + r.h);

	region_clone(&ctx->last_rect, &r);
	return &ctx->last_rect;
}

static union xui_cmd_t * xui_cmd_push(struct xui_context_t * ctx, enum xui_cmd_type_t type, int len, struct region_t * r)
{
	union xui_cmd_t * cmd = (union xui_cmd_t *)(ctx->cmd_list.items + ctx->cmd_list.idx);
	assert(ctx->cmd_list.idx + len < XUI_COMMAND_LIST_SIZE);
	cmd->base.type = type;
	cmd->base.len = len;
	region_clone(&cmd->base.r, r);
	ctx->cmd_list.idx += len;
	return cmd;
}

static inline union xui_cmd_t * xui_cmd_push_jump(struct xui_context_t * ctx, union xui_cmd_t * addr)
{
	union xui_cmd_t * cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_JUMP, sizeof(struct xui_cmd_jump_t), &unlimited_region);
	cmd->jump.addr = addr;
	return cmd;
}

static inline union xui_cmd_t * xui_cmd_push_clip(struct xui_context_t * ctx, struct region_t * r)
{
	return xui_cmd_push(ctx, XUI_CMD_TYPE_CLIP, sizeof(struct xui_cmd_clip_t), r);
}

static void xui_get_bound(struct region_t * r, int x, int y)
{
	int x0 = min(r->x, x);
	int y0 = min(r->y, y);
	int x1 = max(r->x + r->w, x);
	int y1 = max(r->y + r->h, y);
	region_init(r, x0, y0, x1 - x0, y1 - y0);
}

static int xui_check_clip(struct xui_context_t * ctx, struct region_t * r)
{
	struct region_t * cr = xui_get_clip(ctx);

	if((r->w <= 0) || (r->h <= 0) || (r->x > cr->x + cr->w) || (r->x + r->w < cr->x) || (r->y > cr->y + cr->h) || (r->y + r->h < cr->y))
		return 0;
	else if((r->x >= cr->x) && (r->x + r->w <= cr->x + cr->w) && (r->y >= cr->y) && (r->y + r->h <= cr->y + cr->h))
		return 1;
	else
		return -1;
}

void xui_draw_line(struct xui_context_t * ctx, struct point_t * p0, struct point_t * p1, int thickness, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, p0->x, p0->y, 1, 1);
	xui_get_bound(&r, p1->x, p1->y);
	if(thickness > 1)
		region_expand(&r, &r, iceil(thickness / 2));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_LINE, sizeof(struct xui_cmd_line_t), &r);
		cmd->line.p0.x = p0->x;
		cmd->line.p0.y = p0->y;
		cmd->line.p1.x = p1->x;
		cmd->line.p1.y = p1->y;
		cmd->line.thickness = thickness;
		memcpy(&cmd->line.c, c, sizeof(struct color_t));
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_polyline(struct xui_context_t * ctx, struct point_t * p, int n, int thickness, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int len, i;
	int clip;

	region_init(&r, p[0].x, p[0].y, 1, 1);
	for(i = 1; i < n; i++)
		xui_get_bound(&r, p[i].x, p[i].y);
	if(thickness > 1)
		region_expand(&r, &r, iceil(thickness / 2));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		len = sizeof(struct point_t) * n;
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_POLYLINE, sizeof(struct xui_cmd_polyline_t) + len, &r);
		cmd->polyline.n = n;
		cmd->polyline.thickness = thickness;
		memcpy(&cmd->polyline.c, c, sizeof(struct color_t));
		memcpy(cmd->polyline.p, p, len);
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_curve(struct xui_context_t * ctx, struct point_t * p, int n, int thickness, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int len, i;
	int clip;

	region_init(&r, p[0].x, p[0].y, 1, 1);
	for(i = 1; i < n; i++)
		xui_get_bound(&r, p[i].x, p[i].y);
	if(thickness > 1)
		region_expand(&r, &r, iceil(thickness / 2));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		len = sizeof(struct point_t) * n;
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_CURVE, sizeof(struct xui_cmd_curve_t) + len, &r);
		cmd->curve.n = n;
		cmd->curve.thickness = thickness;
		memcpy(&cmd->curve.c, c, sizeof(struct color_t));
		memcpy(cmd->curve.p, p, len);
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_triangle(struct xui_context_t * ctx, struct point_t * p0, struct point_t * p1, struct point_t * p2, int thickness, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, p0->x, p0->y, 1, 1);
	xui_get_bound(&r, p1->x, p1->y);
	xui_get_bound(&r, p2->x, p2->y);
	if(thickness > 1)
		region_expand(&r, &r, iceil(thickness / 2));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_TRIANGLE, sizeof(struct xui_cmd_triangle_t), &r);
		cmd->triangle.p0.x = p0->x;
		cmd->triangle.p0.y = p0->y;
		cmd->triangle.p1.x = p1->x;
		cmd->triangle.p1.y = p1->y;
		cmd->triangle.p2.x = p2->x;
		cmd->triangle.p2.y = p2->y;
		cmd->triangle.thickness = thickness;
		memcpy(&cmd->triangle.c, c, sizeof(struct color_t));
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_rectangle(struct xui_context_t * ctx, int x, int y, int w, int h, int radius, int thickness, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, x, y, w, h);
	if(thickness > 1)
		region_expand(&r, &r, iceil(thickness / 2));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_RECTANGLE, sizeof(struct xui_cmd_rectangle_t), &r);
		cmd->rectangle.x = x;
		cmd->rectangle.y = y;
		cmd->rectangle.w = w;
		cmd->rectangle.h = h;
		cmd->rectangle.radius = radius;
		cmd->rectangle.thickness = thickness;
		memcpy(&cmd->rectangle.c, c, sizeof(struct color_t));
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_polygon(struct xui_context_t * ctx, struct point_t * p, int n, int thickness, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int len, i;
	int clip;

	region_init(&r, p[0].x, p[0].y, 1, 1);
	for(i = 1; i < n; i++)
		xui_get_bound(&r, p[i].x, p[i].y);
	if(thickness > 1)
		region_expand(&r, &r, iceil(thickness / 2));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		len = sizeof(struct point_t) * n;
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_POLYGON, sizeof(struct xui_cmd_polygon_t) + len, &r);
		cmd->polygon.n = n;
		cmd->polygon.thickness = thickness;
		memcpy(&cmd->polygon.c, c, sizeof(struct color_t));
		memcpy(cmd->polygon.p, p, len);
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_circle(struct xui_context_t * ctx, int x, int y, int radius, int thickness, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, x - radius, y - radius, radius * 2, radius * 2);
	if(thickness > 1)
		region_expand(&r, &r, iceil(thickness / 2));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_CIRCLE, sizeof(struct xui_cmd_circle_t), &r);
		cmd->circle.x = x;
		cmd->circle.y = y;
		cmd->circle.radius = radius;
		cmd->circle.thickness = thickness;
		memcpy(&cmd->circle.c, c, sizeof(struct color_t));
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_ellipse(struct xui_context_t * ctx, int x, int y, int w, int h, int thickness, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, x - w, y - h, w * 2, h * 2);
	if(thickness > 1)
		region_expand(&r, &r, iceil(thickness / 2));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_ELLIPSE, sizeof(struct xui_cmd_ellipse_t), &r);
		cmd->ellipse.x = x;
		cmd->ellipse.y = y;
		cmd->ellipse.w = w;
		cmd->ellipse.h = h;
		cmd->ellipse.thickness = thickness;
		memcpy(&cmd->ellipse.c, c, sizeof(struct color_t));
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_arc(struct xui_context_t * ctx, int x, int y, int radius, int a1, int a2, int thickness, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, x - radius, y - radius, radius * 2, radius * 2);
	if(thickness > 1)
		region_expand(&r, &r, iceil(thickness / 2));
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_ARC, sizeof(struct xui_cmd_arc_t), &r);
		cmd->arc.x = x;
		cmd->arc.y = y;
		cmd->arc.radius = radius;
		cmd->arc.a1 = a1;
		cmd->arc.a2 = a2;
		cmd->arc.thickness = thickness;
		memcpy(&cmd->arc.c, c, sizeof(struct color_t));
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_gradient(struct xui_context_t * ctx, int x, int y, int w, int h, struct color_t * lt, struct color_t * rt, struct color_t * rb, struct color_t * lb)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, x, y, w, h);
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_GRADIENT, sizeof(struct xui_cmd_gradient_t), &r);
		cmd->gradient.x = x;
		cmd->gradient.y = y;
		cmd->gradient.w = w;
		cmd->gradient.h = h;
		memcpy(&cmd->gradient.lt, lt, sizeof(struct color_t));
		memcpy(&cmd->gradient.rt, rt, sizeof(struct color_t));
		memcpy(&cmd->gradient.rb, rb, sizeof(struct color_t));
		memcpy(&cmd->gradient.lb, lb, sizeof(struct color_t));
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_checkerboard(struct xui_context_t * ctx, int x, int y, int w, int h)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, x, y, w, h);
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_CHECKERBOARD, sizeof(struct xui_cmd_checkerboard_t), &r);
		cmd->board.x = x;
		cmd->board.y = y;
		cmd->board.w = w;
		cmd->board.h = h;
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_text(struct xui_context_t * ctx, const char * family, int size, const char * utf8, int x, int y, int wrap, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct text_t txt;
	struct region_t r;
	int len;
	int clip;

	text_init(&txt, utf8, c, wrap, ctx->f, family, size);
	region_init(&r, x, y, txt.metrics.width, txt.metrics.height);
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		len = strlen(utf8) + 1;
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_TEXT, sizeof(struct xui_cmd_text_t) + ((len + 0x3) & ~0x3), &r);
		cmd->text.family = family;
		cmd->text.size = size;
		cmd->text.x = x;
		cmd->text.y = y;
		cmd->text.wrap = wrap;
		memcpy(&cmd->text.c, c, sizeof(struct color_t));
		memcpy(cmd->text.utf8, utf8, len);
		cmd->text.utf8[len] = 0;
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

void xui_draw_icon(struct xui_context_t * ctx, const char * family, uint32_t code, int x, int y, int w, int h, struct color_t * c)
{
	union xui_cmd_t * cmd;
	struct region_t r;
	int clip;

	region_init(&r, x, y, w, h);
	if((clip = xui_check_clip(ctx, &r)))
	{
		if(clip < 0)
			xui_cmd_push_clip(ctx, xui_get_clip(ctx));
		cmd = xui_cmd_push(ctx, XUI_CMD_TYPE_ICON, sizeof(struct xui_cmd_icon_t), &r);
		cmd->icon.family = family;
		cmd->icon.code = code;
		cmd->icon.x = x;
		cmd->icon.y = y;
		cmd->icon.w = w;
		cmd->icon.h = h;
		memcpy(&cmd->icon.c, c, sizeof(struct color_t));
		if(clip < 0)
			xui_cmd_push_clip(ctx, &unlimited_region);
	}
}

static int in_hover_root(struct xui_context_t * ctx)
{
	int i = ctx->container_stack.idx;
	while(i--)
	{
		if(ctx->container_stack.items[i] == ctx->hover_root)
			return 1;
		if(ctx->container_stack.items[i]->head)
			break;
	}
	return 0;
}

static int xui_mouse_over(struct xui_context_t * ctx, struct region_t * r)
{
	return region_hit(r, ctx->mouse.x, ctx->mouse.y) && region_hit(xui_get_clip(ctx), ctx->mouse.x, ctx->mouse.y) && in_hover_root(ctx);
}

void xui_control_update(struct xui_context_t * ctx, unsigned int id, struct region_t * r, int opt)
{
	if(ctx->focus == id)
		ctx->updated_focus = 1;
	if(!(opt & XUI_OPT_NOINTERACT))
	{
		int over = xui_mouse_over(ctx, r);
		if(!ctx->mouse.state && over)
			ctx->hover = id;
		if((ctx->hover == id) && !over)
			ctx->hover = 0;
		if(ctx->focus == id)
		{
			if((ctx->mouse.up || ctx->mouse.down) && !over)
				xui_set_focus(ctx, 0);
			if(!ctx->mouse.state && (~opt & XUI_OPT_HOLDFOCUS))
				xui_set_focus(ctx, 0);
		}
		if((ctx->mouse.up || ctx->mouse.down) && over)
			xui_set_focus(ctx, id);
	}
}

void xui_control_draw_text(struct xui_context_t * ctx, const char * utf8, struct region_t * r, struct color_t * c, int opt)
{
	struct text_t txt;
	const char * family = ctx->style.font.font_family;
	int size = ctx->style.font.size;
	int tw, th;
	int x, y;

	text_init(&txt, utf8, c, 0, ctx->f, family, size);
	tw = txt.metrics.width;
	th = txt.metrics.height;

	xui_push_clip(ctx, r);
	switch(opt & (0x7 << 5))
	{
	case XUI_OPT_TEXT_LEFT:
		x = r->x + ctx->style.layout.padding;
		y = r->y + (r->h - th) / 2;
		break;
	case XUI_OPT_TEXT_RIGHT:
		x = r->x + r->w - tw - ctx->style.layout.padding;
		y = r->y + (r->h - th) / 2;
		break;
	case XUI_OPT_TEXT_TOP:
		x = r->x + (r->w - tw) / 2;
		y = r->y + ctx->style.layout.padding;
		break;
	case XUI_OPT_TEXT_BOTTOM:
		x = r->x + (r->w - tw) / 2;
		y = r->y + r->h - th - ctx->style.layout.padding;
		break;
	case XUI_OPT_TEXT_CENTER:
		x = r->x + (r->w - tw) / 2;
		y = r->y + (r->h - th) / 2;
		break;
	default:
		x = r->x + ctx->style.layout.padding;
		y = r->y + (r->h - th) / 2;
		break;
	}
	xui_draw_text(ctx, family, size, utf8, x, y, 0, c);
	xui_pop_clip(ctx);
}

static void scrollbars(struct xui_context_t * ctx, struct xui_container_t * c, struct region_t * body)
{
	struct region_t base, thumb;
	int sz = ctx->style.scroll.scroll_size;
	int width = c->content_width;
	int height = c->content_height;
	int maxscroll;
	unsigned int id;

	width += ctx->style.layout.padding * 2;
	height += ctx->style.layout.padding * 2;
	xui_push_clip(ctx, body);
	if(height > c->body.h)
		body->w -= sz;
	if(width > c->body.w)
		body->h -= sz;

	maxscroll = height - body->h;
	if(maxscroll > 0 && body->h > 0)
	{
		id = xui_get_id(ctx, "!scrollbary", 11);
		region_clone(&base, body);
		base.x = body->x + body->w;
		base.w = ctx->style.scroll.scroll_size;
		xui_control_update(ctx, id, &base, 0);
		if((ctx->focus == id) && (ctx->mouse.state & XUI_MOUSE_LEFT))
			c->scroll_y += ctx->mouse.dy * height / base.h;
		c->scroll_y = clamp(c->scroll_y, 0, maxscroll);
		xui_draw_rectangle(ctx, base.x, base.y, base.w, base.h, ctx->style.scroll.scroll_radius, 0, &ctx->style.scroll.scroll_color);
		region_clone(&thumb, &base);
		thumb.h = max(ctx->style.scroll.thumb_size, base.h * body->h / height);
		thumb.y += c->scroll_y * (base.h - thumb.h) / maxscroll;
		xui_draw_rectangle(ctx, thumb.x, thumb.y, thumb.w, thumb.h, ctx->style.scroll.thumb_radius, 0, &ctx->style.scroll.thumb_color);
		if(xui_mouse_over(ctx, body))
			ctx->scroll_target = c;
	}
	else
	{
		c->scroll_y = 0;
	}

	maxscroll = width - body->w;
	if(maxscroll > 0 && body->w > 0)
	{
		id = xui_get_id(ctx, "!scrollbarx", 11);
		region_clone(&base, body);
		base.y = body->y + body->h;
		base.h = ctx->style.scroll.scroll_size;
		xui_control_update(ctx, id, &base, 0);
		if((ctx->focus == id) && (ctx->mouse.state & XUI_MOUSE_LEFT))
			c->scroll_x += ctx->mouse.dx * width / base.w;
		c->scroll_x = clamp(c->scroll_x, 0, maxscroll);
		xui_draw_rectangle(ctx, base.x, base.y, base.w, base.h, ctx->style.scroll.scroll_radius, 0, &ctx->style.scroll.scroll_color);
		region_clone(&thumb, &base);
		thumb.w = max(ctx->style.scroll.thumb_size, base.w * body->w / width);
		thumb.x += c->scroll_x * (base.w - thumb.w) / maxscroll;
		xui_draw_rectangle(ctx, thumb.x, thumb.y, thumb.w, thumb.h, ctx->style.scroll.thumb_radius, 0, &ctx->style.scroll.thumb_color);
		if(xui_mouse_over(ctx, body))
			ctx->scroll_target = c;
	}
	else
	{
		c->scroll_x = 0;
	}
	xui_pop_clip(ctx);
}

void push_container_body(struct xui_context_t * ctx, struct xui_container_t * c, struct region_t * body, int opt)
{
	struct region_t r;
	if(~opt & XUI_OPT_NOSCROLL)
		scrollbars(ctx, c, body);
	region_expand(&r, body, -ctx->style.layout.padding);
	push_layout(ctx, &r, c->scroll_x, c->scroll_y);
	region_clone(&c->body, body);
}

void begin_root_container(struct xui_context_t * ctx, struct xui_container_t * c)
{
	xui_push(ctx->container_stack, c);
	xui_push(ctx->root_list, c);
	c->head = xui_cmd_push_jump(ctx, NULL);
	if(region_hit(&c->region, ctx->mouse.x, ctx->mouse.y) && (!ctx->next_hover_root || (c->zindex > ctx->next_hover_root->zindex)))
		ctx->next_hover_root = c;
	xui_push(ctx->clip_stack, unlimited_region);
}

void end_root_container(struct xui_context_t * ctx)
{
	struct xui_container_t * c = xui_get_current_container(ctx);
	c->tail = xui_cmd_push_jump(ctx, NULL);
	c->head->jump.addr = ctx->cmd_list.items + ctx->cmd_list.idx;
	xui_pop_clip(ctx);
	pop_container(ctx);
}

struct xui_context_t * xui_context_alloc(const char * fb, const char * input, struct xui_style_t * style, void * data)
{
	struct xui_context_t * ctx;
	int len;

	ctx = malloc(sizeof(struct xui_context_t));
	if(!ctx)
		return NULL;

	memset(ctx, 0, sizeof(struct xui_context_t));
	ctx->w = window_alloc(fb, input, NULL);
	ctx->f = font_context_alloc();
	region_init(&ctx->screen, 0, 0, window_get_width(ctx->w), window_get_height(ctx->w));
	ctx->cpshift = 7;
	ctx->cpsize = 1 << ctx->cpshift;
	ctx->cwidth = (ctx->screen.w >> ctx->cpshift) + 1;
	ctx->cheight = (ctx->screen.h >> ctx->cpshift) + 1;
	len = ctx->cwidth * ctx->cheight * sizeof(int);
	ctx->cells[0] = malloc(len);
	ctx->cells[1] = malloc(len);
	if(!ctx->cells[0] || !ctx->cells[1])
	{
		if(ctx->cells[0])
			free(ctx->cells[0]);
		if(ctx->cells[1])
			free(ctx->cells[1]);
		free(ctx);
	}
	memset(ctx->cells[0], 0xff, len);
	memset(ctx->cells[1], 0xff, len);
	ctx->cindex = 0;
	ctx->last = ctx->now = ktime_to_ns(ktime_get());
	memcpy(&ctx->style, style ? style : &xui_style_default, sizeof(struct xui_style_t));
	region_clone(&ctx->clip, &ctx->screen);
	ctx->priv = data;

	return ctx;
}

void xui_context_free(struct xui_context_t * ctx)
{
	if(ctx)
	{
		window_free(ctx->w);
		font_context_free(ctx->f);
		if(ctx->cells[0])
			free(ctx->cells[0]);
		if(ctx->cells[1])
			free(ctx->cells[1]);
		free(ctx);
	}
}

static void xui_draw(struct window_t * w, void * o)
{
	struct xui_context_t * ctx = (struct xui_context_t *)o;
	struct surface_t * s = ctx->w->s;
	struct region_t * r, * clip = &ctx->clip;
	union xui_cmd_t * cmd;
	struct matrix_t m;
	struct text_t txt;
	struct icon_t ico;
	int count, size;
	int i;

	if((count = w->rl->count) > 0)
	{
		for(i = 0; i < count; i++)
		{
			r = &w->rl->region[i];
			region_clone(clip, r);
			cmd = NULL;
			while(xui_cmd_next(ctx, &cmd))
			{
				switch(cmd->base.type)
				{
				case XUI_CMD_TYPE_CLIP:
					if(!region_intersect(clip, r, &cmd->clip.r))
						region_init(clip, 0, 0, 0, 0);
					break;
				case XUI_CMD_TYPE_LINE:
					surface_shape_line(s, clip, &cmd->line.p0, &cmd->line.p1, cmd->line.thickness, &cmd->line.c);
					break;
				case XUI_CMD_TYPE_POLYLINE:
					surface_shape_polyline(s, clip, cmd->polyline.p, cmd->polyline.n, cmd->polyline.thickness, &cmd->polyline.c);
					break;
				case XUI_CMD_TYPE_CURVE:
					surface_shape_curve(s, clip, cmd->curve.p, cmd->curve.n, cmd->curve.thickness, &cmd->curve.c);
					break;
				case XUI_CMD_TYPE_TRIANGLE:
					surface_shape_triangle(s, clip, &cmd->triangle.p0, &cmd->triangle.p1, &cmd->triangle.p2, cmd->triangle.thickness, &cmd->triangle.c);
					break;
				case XUI_CMD_TYPE_RECTANGLE:
					surface_shape_rectangle(s, clip, cmd->rectangle.x, cmd->rectangle.y, cmd->rectangle.w, cmd->rectangle.h, cmd->rectangle.radius, cmd->rectangle.thickness, &cmd->rectangle.c);
					break;
				case XUI_CMD_TYPE_POLYGON:
					surface_shape_polygon(s, clip, cmd->polygon.p, cmd->polygon.n, cmd->polygon.thickness, &cmd->polygon.c);
					break;
				case XUI_CMD_TYPE_CIRCLE:
					surface_shape_circle(s, clip, cmd->circle.x, cmd->circle.y, cmd->circle.radius, cmd->circle.thickness, &cmd->circle.c);
					break;
				case XUI_CMD_TYPE_ELLIPSE:
					surface_shape_ellipse(s, clip, cmd->ellipse.x, cmd->ellipse.y, cmd->ellipse.w, cmd->ellipse.h, cmd->ellipse.thickness, &cmd->ellipse.c);
					break;
				case XUI_CMD_TYPE_ARC:
					surface_shape_arc(s, clip, cmd->arc.x, cmd->arc.y, cmd->arc.radius, cmd->arc.a1, cmd->arc.a2, cmd->arc.thickness, &cmd->arc.c);
					break;
				case XUI_CMD_TYPE_CHECKERBOARD:
					surface_shape_checkerboard(s, clip, cmd->board.x, cmd->board.y, cmd->board.w, cmd->board.h);
					break;
				case XUI_CMD_TYPE_GRADIENT:
					surface_shape_gradient(s, clip, cmd->gradient.x, cmd->gradient.y, cmd->gradient.w, cmd->gradient.h, &cmd->gradient.lt, &cmd->gradient.rt, &cmd->gradient.rb, &cmd->gradient.lb);
					break;
				case XUI_CMD_TYPE_TEXT:
					text_init(&txt, cmd->text.utf8, &cmd->text.c, cmd->text.wrap, ctx->f, cmd->text.family, cmd->text.size);
					matrix_init_translate(&m, cmd->text.x, cmd->text.y);
					surface_text(s, clip, &m, &txt);
					break;
				case XUI_CMD_TYPE_ICON:
					size = min(cmd->icon.w, cmd->icon.h);
					icon_init(&ico, cmd->icon.code, &cmd->icon.c, ctx->f, cmd->icon.family, size);
					matrix_init_translate(&m, cmd->icon.x + (cmd->icon.w - size) / 2, cmd->icon.y + (cmd->icon.h - size) / 2);
					surface_icon(s, clip, &m, &ico);
					break;
				default:
					break;
				}
			}
		}
	}
}

void xui_loop(struct xui_context_t * ctx, void (*func)(struct xui_context_t *))
{
	struct event_t e;
	char utf8[16];
	int l, sz;

	while(1)
	{
		while(window_pump_event(ctx->w, &e))
		{
			switch(e.type)
			{
			case EVENT_TYPE_KEY_DOWN:
				switch(e.e.key_down.key)
				{
				case KEY_POWER:
					ctx->key_down |= XUI_KEY_POWER;
					ctx->key_pressed |= XUI_KEY_POWER;
					break;
				case KEY_UP:
					ctx->key_down |= XUI_KEY_UP;
					ctx->key_pressed |= XUI_KEY_UP;
					break;
				case KEY_DOWN:
					ctx->key_down |= XUI_KEY_DOWN;
					ctx->key_pressed |= XUI_KEY_DOWN;
					break;
				case KEY_LEFT:
					ctx->key_down |= XUI_KEY_LEFT;
					ctx->key_pressed |= XUI_KEY_LEFT;
					break;
				case KEY_RIGHT:
					ctx->key_down |= XUI_KEY_RIGHT;
					ctx->key_pressed |= XUI_KEY_RIGHT;
					break;
				case KEY_VOLUME_UP:
					ctx->key_down |= XUI_KEY_VOLUME_UP;
					ctx->key_pressed |= XUI_KEY_VOLUME_UP;
					break;
				case KEY_VOLUME_DOWN:
					ctx->key_down |= XUI_KEY_VOLUME_DOWN;
					ctx->key_pressed |= XUI_KEY_VOLUME_DOWN;
					break;
				case KEY_VOLUME_MUTE:
					ctx->key_down |= XUI_KEY_VOLUME_MUTE;
					ctx->key_pressed |= XUI_KEY_VOLUME_MUTE;
					break;
				case KEY_TAB:
					ctx->key_down |= XUI_KEY_TAB;
					ctx->key_pressed |= XUI_KEY_TAB;
					break;
				case KEY_TASK:
					ctx->key_down |= XUI_KEY_TASK;
					ctx->key_pressed |= XUI_KEY_TASK;
					break;
				case KEY_HOME:
					ctx->key_down |= XUI_KEY_HOME;
					ctx->key_pressed |= XUI_KEY_HOME;
					break;
				case KEY_BACK:
					ctx->key_down |= XUI_KEY_BACK;
					ctx->key_pressed |= XUI_KEY_BACK;
					break;
				case KEY_ENTER:
					ctx->key_down |= XUI_KEY_ENTER;
					ctx->key_pressed |= XUI_KEY_ENTER;
					break;
				case KEY_L_CTRL:
				case KEY_R_CTRL:
					ctx->key_down |= XUI_KEY_CTRL;
					ctx->key_pressed |= XUI_KEY_CTRL;
					break;
				case KEY_L_ALT:
				case KEY_R_ALT:
					ctx->key_down |= XUI_KEY_ALT;
					ctx->key_pressed |= XUI_KEY_ALT;
					break;
				case KEY_L_SHIFT:
				case KEY_R_SHIFT:
					ctx->key_down |= XUI_KEY_SHIFT;
					ctx->key_pressed |= XUI_KEY_SHIFT;
					break;
				default:
					if(e.e.key_up.key >= KEY_SPACE)
					{
						ucs4_to_utf8(&e.e.key_up.key, 1, utf8, sizeof(utf8));
						l = strlen(ctx->input_text);
						sz = strlen(utf8) + 1;
						if(l + sz <= sizeof(ctx->input_text))
							memcpy(ctx->input_text + l, utf8, sz);
					}
					break;
				}
				break;
			case EVENT_TYPE_KEY_UP:
				switch(e.e.key_up.key)
				{
				case KEY_POWER:
					ctx->key_down &= ~XUI_KEY_POWER;
					break;
				case KEY_UP:
					ctx->key_down &= ~XUI_KEY_UP;
					break;
				case KEY_DOWN:
					ctx->key_down &= ~XUI_KEY_DOWN;
					break;
				case KEY_LEFT:
					ctx->key_down &= ~XUI_KEY_LEFT;
					break;
				case KEY_RIGHT:
					ctx->key_down &= ~XUI_KEY_RIGHT;
					break;
				case KEY_VOLUME_UP:
					ctx->key_down &= ~XUI_KEY_VOLUME_UP;
					break;
				case KEY_VOLUME_DOWN:
					ctx->key_down &= ~XUI_KEY_VOLUME_DOWN;
					break;
				case KEY_VOLUME_MUTE:
					ctx->key_down &= ~XUI_KEY_VOLUME_MUTE;
					break;
				case KEY_TAB:
					ctx->key_down &= ~XUI_KEY_TAB;
					break;
				case KEY_TASK:
					ctx->key_down &= ~XUI_KEY_TASK;
					break;
				case KEY_HOME:
					ctx->key_down &= ~XUI_KEY_HOME;
					break;
				case KEY_BACK:
					ctx->key_down &= ~XUI_KEY_BACK;
					break;
				case KEY_ENTER:
					ctx->key_down &= ~XUI_KEY_ENTER;
					break;
				case KEY_L_CTRL:
				case KEY_R_CTRL:
					ctx->key_down &= ~XUI_KEY_CTRL;
					break;
				case KEY_L_ALT:
				case KEY_R_ALT:
					ctx->key_down &= ~XUI_KEY_ALT;
					break;
				case KEY_L_SHIFT:
				case KEY_R_SHIFT:
					ctx->key_down &= ~XUI_KEY_SHIFT;
					break;
				default:
					break;
				}
				break;
			case EVENT_TYPE_MOUSE_DOWN:
				ctx->mouse.x = e.e.mouse_down.x;
				ctx->mouse.y = e.e.mouse_down.y;
				ctx->mouse.state |= e.e.mouse_down.button;
				ctx->mouse.down |= e.e.mouse_down.button;
				break;
			case EVENT_TYPE_MOUSE_MOVE:
				ctx->mouse.x = e.e.mouse_move.x;
				ctx->mouse.y = e.e.mouse_move.y;
				break;
			case EVENT_TYPE_MOUSE_UP:
				ctx->mouse.x = e.e.mouse_up.x;
				ctx->mouse.y = e.e.mouse_up.y;
				ctx->mouse.state &= ~e.e.mouse_up.button;
				ctx->mouse.up |= e.e.mouse_up.button;
				break;
			case EVENT_TYPE_MOUSE_WHEEL:
				ctx->mouse.zx -= e.e.mouse_wheel.dx * 30;
				ctx->mouse.zy -= e.e.mouse_wheel.dy * 30;
				break;
			case EVENT_TYPE_TOUCH_BEGIN:
				if(e.e.touch_begin.id == 0)
				{
					ctx->mouse.ox = ctx->mouse.x = e.e.touch_begin.x;
					ctx->mouse.oy = ctx->mouse.y = e.e.touch_begin.y;
					ctx->mouse.state |= MOUSE_BUTTON_LEFT;
					ctx->mouse.down |= MOUSE_BUTTON_LEFT;
				}
				break;
			case EVENT_TYPE_TOUCH_MOVE:
				if(e.e.touch_move.id == 0)
				{
					ctx->mouse.x = e.e.touch_move.x;
					ctx->mouse.y = e.e.touch_move.y;
				}
				break;
			case EVENT_TYPE_TOUCH_END:
				if(e.e.touch_end.id == 0)
				{
					ctx->mouse.x = e.e.touch_end.x;
					ctx->mouse.y = e.e.touch_end.y;
					ctx->mouse.state &= ~MOUSE_BUTTON_LEFT;
					ctx->mouse.up |= MOUSE_BUTTON_LEFT;
				}
				break;
			default:
				break;
			}
		}
		if(func)
			func(ctx);
		if(window_is_active(ctx->w))
			window_present(ctx->w, ctx, xui_draw);
		task_yield();
	}
}
