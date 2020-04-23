/*
 * kernel/xui/xui.c
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
#include <xui/xui.h>

static struct xui_style_t xui_style_default = {
	.bgcol = {
		.r = 90,
		.g = 95,
		.b = 100,
		.a = 255,
	},

	/* font | size | padding | spacing | indent */
	NULL, { 68, 10 }, 5, 4, 24,
	/* title_height | scrollbar_size | thumb_size */
	24, 12, 8,
	{
		{ 230, 230, 230, 255 }, /* MU_COLOR_TEXT */
		{ 25,  25,  25,  255 }, /* MU_COLOR_BORDER */
		{ 50,  50,  50,  255 }, /* MU_COLOR_WINDOWBG */
		{ 25,  25,  25,  255 }, /* MU_COLOR_TITLEBG */
		{ 240, 240, 240, 255 }, /* MU_COLOR_TITLETEXT */
		{ 0,   0,   0,   0   }, /* MU_COLOR_PANELBG */
		{ 75,  75,  75,  255 }, /* MU_COLOR_BUTTON */
		{ 95,  95,  95,  255 }, /* MU_COLOR_BUTTONHOVER */
		{ 115, 115, 115, 255 }, /* MU_COLOR_BUTTONFOCUS */
		{ 30,  30,  30,  255 }, /* MU_COLOR_BASE */
		{ 35,  35,  35,  255 }, /* MU_COLOR_BASEHOVER */
		{ 40,  40,  40,  255 }, /* MU_COLOR_BASEFOCUS */
		{ 43,  43,  43,  255 }, /* MU_COLOR_SCROLLBASE */
		{ 30,  30,  30,  255 }  /* MU_COLOR_SCROLLTHUMB */
	}
};

static mu_Rect expand_rect(mu_Rect rect, int n) {
  return mu_rect(rect.x - n, rect.y - n, rect.w + n * 2, rect.h + n * 2);
}


static mu_Rect intersect_rects(mu_Rect r1, mu_Rect r2) {
  int x1 = max(r1.x, r2.x);
  int y1 = max(r1.y, r2.y);
  int x2 = min(r1.x + r1.w, r2.x + r2.w);
  int y2 = min(r1.y + r1.h, r2.y + r2.h);
  if (x2 < x1) { x2 = x1; }
  if (y2 < y1) { y2 = y1; }
  return mu_rect(x1, y1, x2 - x1, y2 - y1);
}

void mu_draw_rect(struct xui_context_t *ctx, mu_Rect rect, mu_Color color) {
  union xui_command_t *cmd;
  rect = intersect_rects(rect, mu_get_clip_rect(ctx));
  if (rect.w > 0 && rect.h > 0) {
    cmd = xui_push_command(ctx, XUI_COMMAND_TYPE_RECT, sizeof(struct xui_command_rect_t));
    cmd->rect.rect = rect;
    cmd->rect.color = color;
  }
}

void mu_draw_box(struct xui_context_t *ctx, mu_Rect rect, mu_Color color) {
  mu_draw_rect(ctx, mu_rect(rect.x + 1, rect.y, rect.w - 2, 1), color);
  mu_draw_rect(ctx, mu_rect(rect.x + 1, rect.y + rect.h - 1, rect.w - 2, 1), color);
  mu_draw_rect(ctx, mu_rect(rect.x, rect.y, 1, rect.h), color);
  mu_draw_rect(ctx, mu_rect(rect.x + rect.w - 1, rect.y, 1, rect.h), color);
}

static void draw_frame(struct xui_context_t *ctx, mu_Rect rect, int colorid)
{
	mu_draw_rect(ctx, rect, ctx->style.colors[colorid]);
	if(colorid == MU_COLOR_SCROLLBASE || colorid == MU_COLOR_SCROLLTHUMB || colorid == MU_COLOR_TITLEBG)
	{
		return;
	}
	/* draw border */
	if(ctx->style.colors[MU_COLOR_BORDER].a)
	{
		mu_draw_box(ctx, expand_rect(rect, 1), ctx->style.colors[MU_COLOR_BORDER]);
	}
}

static int text_width(void * font, const char *text, int len)
{
	if(len == -1)
	{
		len = strlen(text);
	}
	return 16 * len;
}

static int text_height(void * font)
{
	return 16;
}

struct xui_context_t * xui_context_alloc(const char * fb, const char * input, struct xui_style_t * style)
{
	struct xui_context_t * ctx;

	ctx = malloc(sizeof(struct xui_context_t));
	if(!ctx)
		return NULL;

	memset(ctx, 0, sizeof(struct xui_context_t));
	ctx->w = window_alloc(fb, input, ctx);
	ctx->f = font_context_alloc();

	memcpy(&ctx->style, style ? style : &xui_style_default, sizeof(struct xui_style_t));
	region_init(&ctx->clip, 0, 0, window_get_width(ctx->w), window_get_height(ctx->w));

	ctx->draw_frame = draw_frame;
	ctx->text_width = text_width;
	ctx->text_height = text_height;
	return ctx;
}

void xui_context_free(struct xui_context_t * ctx)
{
	if(ctx)
	{
		window_free(ctx->w);
		font_context_free(ctx->f);
		free(ctx);
	}
}

static void xui_draw(struct window_t * w, void * o)
{
	struct xui_context_t * ctx = (struct xui_context_t *)o;
	struct surface_t * s = ctx->w->s;
	struct region_t * clip = &ctx->clip;
    union xui_command_t * cmd = NULL;
    struct color_t c;

	while(xui_next_command(ctx, &cmd))
	{
		switch(cmd->base.type)
		{
		case XUI_COMMAND_TYPE_JUMP:
			break;
		case XUI_COMMAND_TYPE_CLIP:
			{
				struct region_t a, b;
				region_init(&a, cmd->clip.rect.x, cmd->clip.rect.y, cmd->clip.rect.w, cmd->clip.rect.h);
				region_init(&b, 0, 0, window_get_width(ctx->w), window_get_height(ctx->w));
				region_intersect(clip, &a, &b);
			}
			break;
		case XUI_COMMAND_TYPE_RECT:
			c.r = cmd->rect.color.r;
			c.g = cmd->rect.color.g;
			c.b = cmd->rect.color.b;
			c.a = 255;
			surface_shape_rectangle(s, clip, cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h, 0, 0, &c);
			break;
		case XUI_COMMAND_TYPE_TEXT:
/*			cairo_save(cr);
			cairo_set_font_size(cr, 16);
			cairo_set_source_rgb(cr, cmd->text.color.r / 255.0, cmd->text.color.g / 255.0, cmd->text.color.b / 255.0);
			cairo_move_to(cr, cmd->text.pos.x, cmd->text.pos.y + 16);
			cairo_show_text(cr, cmd->text.str);
			cairo_restore(cr);*/
			break;
		case XUI_COMMAND_TYPE_ICON:
			//r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color);
			break;
		default:
			break;
		}
	}
}

void xui_loop(struct xui_context_t * ctx, void (*func)(struct xui_context_t *))
{
	struct event_t e;

	while(1)
	{
		if(window_pump_event(ctx->w, &e))
		{
			switch(e.type)
			{
			case EVENT_TYPE_KEY_DOWN:
				mu_input_keydown(ctx, e.e.key_down.key);
				break;
			case EVENT_TYPE_KEY_UP:
				mu_input_keyup(ctx, e.e.key_up.key);
				break;
			case EVENT_TYPE_MOUSE_DOWN:
				mu_input_mousedown(ctx, e.e.mouse_down.x, e.e.mouse_down.y, e.e.mouse_down.button);
				break;
			case EVENT_TYPE_MOUSE_MOVE:
				mu_input_mousemove(ctx, e.e.mouse_move.x, e.e.mouse_move.y);
				break;
			case EVENT_TYPE_MOUSE_UP:
				mu_input_mouseup(ctx, e.e.mouse_up.x, e.e.mouse_up.y, e.e.mouse_up.button);
				break;
			case EVENT_TYPE_MOUSE_WHEEL:
				mu_input_scroll(ctx, e.e.mouse_wheel.dx * 30, e.e.mouse_wheel.dy * -30);
				break;
			default:
				break;
			}
		}
		if(func)
			func(ctx);
		if(window_is_active(ctx->w))
		{
			ctx->w->wm->refresh = 1;
			window_present(ctx->w, &ctx->style.bgcol, ctx, xui_draw);
		}
		task_yield();
	}
}



/////////////
#define expect(x) do {                                               \
    if (!(x)) {                                                      \
      fprintf(stderr, "Fatal error: %s:%d: assertion '%s' failed\n", \
        __FILE__, __LINE__, #x);                                     \
      abort();                                                       \
    }                                                                \
  } while (0)

#define push(stk, val) do {                                                 \
    expect((stk).idx < (int) (sizeof((stk).items) / sizeof(*(stk).items))); \
    (stk).items[(stk).idx] = (val);                                         \
    (stk).idx++; /* incremented after incase `val` uses this value */       \
  } while (0)

#define pop(stk) do {      \
    expect((stk).idx > 0); \
    (stk).idx--;           \
  } while (0)


static mu_Rect unclipped_rect = { 0, 0, 0x1000000, 0x1000000 };

mu_Vec2 mu_vec2(int x, int y) {
  mu_Vec2 res;
  res.x = x; res.y = y;
  return res;
}


mu_Rect mu_rect(int x, int y, int w, int h) {
  mu_Rect res;
  res.x = x; res.y = y; res.w = w; res.h = h;
  return res;
}


mu_Color mu_color(int r, int g, int b, int a) {
  mu_Color res;
  res.r = r; res.g = g; res.b = b; res.a = a;
  return res;
}


static int rect_overlaps_vec2(mu_Rect r, mu_Vec2 p) {
  return p.x >= r.x && p.x < r.x + r.w && p.y >= r.y && p.y < r.y + r.h;
}

void xui_begin(struct xui_context_t * ctx)
{
	ctx->command_list.idx = 0;
	ctx->root_list.idx = 0;
	ctx->scroll_target = NULL;
	ctx->hover_root = ctx->next_hover_root;
	ctx->next_hover_root = NULL;
	ctx->mouse_delta.x = ctx->mouse_pos.x - ctx->last_mouse_pos.x;
	ctx->mouse_delta.y = ctx->mouse_pos.y - ctx->last_mouse_pos.y;
	ctx->frame++;
}

static int compare_zindex(const void * a, const void * b)
{
	return (*(struct xui_container_t **)a)->zindex - (*(struct xui_container_t **)b)->zindex;
}

void xui_end(struct xui_context_t * ctx)
{
	int i, n;

	expect(ctx->container_stack.idx == 0);
	expect(ctx->clip_stack.idx      == 0);
	expect(ctx->id_stack.idx        == 0);
	expect(ctx->layout_stack.idx    == 0);

	if(ctx->scroll_target)
	{
		ctx->scroll_target->scroll.x += ctx->scroll_delta.x;
		ctx->scroll_target->scroll.y += ctx->scroll_delta.y;
	}

	if(!ctx->updated_focus)
	{
		ctx->focus = 0;
	}
	ctx->updated_focus = 0;

	if(ctx->mouse_pressed && ctx->next_hover_root && (ctx->next_hover_root->zindex < ctx->last_zindex) && (ctx->next_hover_root->zindex >= 0))
	{
		xui_set_front(ctx, ctx->next_hover_root);
	}

	ctx->key_pressed = 0;
	ctx->input_text[0] = '\0';
	ctx->mouse_pressed = 0;
	ctx->scroll_delta = mu_vec2(0, 0);
	ctx->last_mouse_pos = ctx->mouse_pos;

	n = ctx->root_list.idx;
	qsort(ctx->root_list.items, n, sizeof(struct xui_container_t *), compare_zindex);

	for(i = 0; i < n; i++)
	{
		struct xui_container_t * c = ctx->root_list.items[i];
		if(i == 0)
		{
			union xui_command_t * cmd = (union xui_command_t *)ctx->command_list.items;
			cmd->jump.addr = (char *)c->head + sizeof(struct xui_command_jump_t);
		}
		else
		{
			struct xui_container_t * prev = ctx->root_list.items[i - 1];
			prev->tail->jump.addr = (char *)c->head + sizeof(struct xui_command_jump_t);
		}
		if(i == n - 1)
		{
			c->tail->jump.addr = ctx->command_list.items + ctx->command_list.idx;
		}
	}
}

void xui_set_focus(struct xui_context_t * ctx, unsigned int id)
{
	ctx->focus = id;
	ctx->updated_focus = 1;
}

void xui_set_front(struct xui_context_t * ctx, struct xui_container_t * c)
{
	c->zindex = ++ctx->last_zindex;
}

static void xui_hash(unsigned int * hash, const void * data, int size)
{
	const unsigned char * p = data;
	while(size--)
	{
		*hash = (*hash ^ *p++) * 16777619;
	}
}

unsigned int xui_get_id(struct xui_context_t * ctx, const void * data, int size)
{
	int idx = ctx->id_stack.idx;
	unsigned int res = (idx > 0) ? ctx->id_stack.items[idx - 1] : 2166136261;
	xui_hash(&res, data, size);
	ctx->last_id = res;
	return res;
}

void xui_push_id(struct xui_context_t * ctx, const void * data, int size)
{
	push(ctx->id_stack, xui_get_id(ctx, data, size));
}

void xui_pop_id(struct xui_context_t * ctx)
{
	pop(ctx->id_stack);
}

mu_Rect mu_get_clip_rect(struct xui_context_t * ctx)
{
	expect(ctx->clip_stack.idx > 0);
	return ctx->clip_stack.items[ctx->clip_stack.idx - 1];
}

void mu_push_clip_rect(struct xui_context_t * ctx, mu_Rect rect)
{
	mu_Rect last = mu_get_clip_rect(ctx);
	push(ctx->clip_stack, intersect_rects(rect, last));
}

void mu_pop_clip_rect(struct xui_context_t * ctx)
{
	pop(ctx->clip_stack);
}

int mu_check_clip(struct xui_context_t * ctx, mu_Rect r)
{
	mu_Rect cr = mu_get_clip_rect(ctx);
	if((r.x > cr.x + cr.w) || (r.x + r.w < cr.x) || (r.y > cr.y + cr.h) || (r.y + r.h < cr.y))
	{
		return MU_CLIP_ALL;
	}
	if((r.x >= cr.x) && (r.x + r.w <= cr.x + cr.w) && (r.y >= cr.y) && (r.y + r.h <= cr.y + cr.h))
	{
		return 0;
	}
	return MU_CLIP_PART;
}

static void push_layout(struct xui_context_t * ctx, mu_Rect body, mu_Vec2 scroll)
{
	struct xui_layout_t layout;
	int width = 0;
	memset(&layout, 0, sizeof(layout));
	layout.body = mu_rect(body.x - scroll.x, body.y - scroll.y, body.w, body.h);
	layout.max = mu_vec2(-0x1000000, -0x1000000);
	push(ctx->layout_stack, layout);
	xui_layout_row(ctx, 1, &width, 0);
}

static struct xui_layout_t * get_layout(struct xui_context_t * ctx)
{
	return &ctx->layout_stack.items[ctx->layout_stack.idx - 1];
}

static void pop_container(struct xui_context_t * ctx)
{
	struct xui_container_t * c = xui_get_current_container(ctx);
	struct xui_layout_t * layout = get_layout(ctx);
	c->content_size.x = layout->max.x - layout->body.x;
	c->content_size.y = layout->max.y - layout->body.y;
	pop(ctx->container_stack);
	pop(ctx->layout_stack);
	xui_pop_id(ctx);
}

struct xui_container_t * xui_get_current_container(struct xui_context_t * ctx)
{
	expect(ctx->container_stack.idx > 0);
	return ctx->container_stack.items[ctx->container_stack.idx - 1];
}

static struct xui_container_t * get_container(struct xui_context_t * ctx, unsigned int id, int opt)
{
	struct xui_container_t * c;
	int idx = xui_pool_get(ctx, ctx->container_pool, MU_CONTAINERPOOL_SIZE, id);
	if(idx >= 0)
	{
		if(ctx->containers[idx].open || (~opt & MU_OPT_CLOSED))
		{
			xui_pool_update(ctx, ctx->container_pool, idx);
		}
		return &ctx->containers[idx];
	}
	if(opt & MU_OPT_CLOSED)
	{
		return NULL;
	}
	idx = xui_pool_init(ctx, ctx->container_pool, MU_CONTAINERPOOL_SIZE, id);
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
	expect(n > -1);
	items[n].id = id;
	xui_pool_update(ctx, items, n);
	return n;
}

int xui_pool_get(struct xui_context_t * ctx, struct xui_pool_item_t * items, int len, unsigned int id)
{
	int i;
	for(i = 0; i < len; i++)
	{
		if(items[i].id == id)
		{
			return i;
		}
	}
	return -1;
}

void xui_pool_update(struct xui_context_t * ctx, struct xui_pool_item_t * items, int idx)
{
	items[idx].last_update = ctx->frame;
}

void mu_input_mousemove(struct xui_context_t * ctx, int x, int y)
{
	ctx->mouse_pos = mu_vec2(x, y);
}

void mu_input_mousedown(struct xui_context_t * ctx, int x, int y, int btn)
{
	mu_input_mousemove(ctx, x, y);
	ctx->mouse_down |= btn;
	ctx->mouse_pressed |= btn;
}

void mu_input_mouseup(struct xui_context_t * ctx, int x, int y, int btn)
{
	mu_input_mousemove(ctx, x, y);
	ctx->mouse_down &= ~btn;
}

void mu_input_scroll(struct xui_context_t * ctx, int x, int y)
{
	ctx->scroll_delta.x += x;
	ctx->scroll_delta.y += y;
}

void mu_input_keydown(struct xui_context_t * ctx, int key)
{
	ctx->key_pressed |= key;
	ctx->key_down |= key;
}

void mu_input_keyup(struct xui_context_t * ctx, int key)
{
	ctx->key_down &= ~key;
}

void mu_input_text(struct xui_context_t * ctx, const char * text)
{
	int len = strlen(ctx->input_text);
	int size = strlen(text) + 1;
	expect(len + size <= (int ) sizeof(ctx->input_text));
	memcpy(ctx->input_text + len, text, size);
}

union xui_command_t * xui_push_command(struct xui_context_t * ctx, enum xui_command_type_t type, int size)
{
	union xui_command_t * cmd = (union xui_command_t *)(ctx->command_list.items + ctx->command_list.idx);
	expect(ctx->command_list.idx + size < MU_COMMANDLIST_SIZE);
	cmd->base.type = type;
	cmd->base.size = size;
	ctx->command_list.idx += size;
	return cmd;
}

int xui_next_command(struct xui_context_t * ctx, union xui_command_t ** cmd)
{
	if(*cmd)
	{
		*cmd = (union xui_command_t *)(((char *)*cmd) + (*cmd)->base.size);
	}
	else
	{
		*cmd = (union xui_command_t *)ctx->command_list.items;
	}
	while((char *)(*cmd) != ctx->command_list.items + ctx->command_list.idx)
	{
		if((*cmd)->base.type != XUI_COMMAND_TYPE_JUMP)
		{
			return 1;
		}
		*cmd = (*cmd)->jump.addr;
	}
	return 0;
}

static union xui_command_t * push_jump(struct xui_context_t * ctx, union xui_command_t * addr)
{
	union xui_command_t * cmd;
	cmd = xui_push_command(ctx, XUI_COMMAND_TYPE_JUMP, sizeof(struct xui_command_jump_t));
	cmd->jump.addr = addr;
	return cmd;
}

void xui_set_clip(struct xui_context_t * ctx, mu_Rect rect)
{
	union xui_command_t * cmd;
	cmd = xui_push_command(ctx, XUI_COMMAND_TYPE_CLIP, sizeof(struct xui_command_clip_t));
	cmd->clip.rect = rect;
}

void mu_draw_text(struct xui_context_t * ctx, void * font, const char * str, int len, mu_Vec2 pos, mu_Color color)
{
	union xui_command_t *cmd;
	mu_Rect rect = mu_rect(pos.x, pos.y, ctx->text_width(font, str, len), ctx->text_height(font));
	int clipped = mu_check_clip(ctx, rect);
	if(clipped == MU_CLIP_ALL)
	{
		return;
	}
	if(clipped == MU_CLIP_PART)
	{
		xui_set_clip(ctx, mu_get_clip_rect(ctx));
	}
	/* add command */
	if(len < 0)
	{
		len = strlen(str);
	}
	cmd = xui_push_command(ctx, XUI_COMMAND_TYPE_TEXT, sizeof(struct xui_command_text_t) + len);
	memcpy(cmd->text.str, str, len);
	cmd->text.str[len] = '\0';
	cmd->text.pos = pos;
	cmd->text.color = color;
	cmd->text.font = font;
	/* reset clipping if it was set */
	if(clipped)
	{
		xui_set_clip(ctx, unclipped_rect);
	}
}

void mu_draw_icon(struct xui_context_t *ctx, int id, mu_Rect rect, mu_Color color)
{
	union xui_command_t *cmd;
	/* do clip command if the rect isn't fully contained within the cliprect */
	int clipped = mu_check_clip(ctx, rect);
	if(clipped == MU_CLIP_ALL)
	{
		return;
	}
	if(clipped == MU_CLIP_PART)
	{
		xui_set_clip(ctx, mu_get_clip_rect(ctx));
	}
	/* do icon command */
	cmd = xui_push_command(ctx, XUI_COMMAND_TYPE_ICON, sizeof(struct xui_command_icon_t));
	cmd->icon.id = id;
	cmd->icon.rect = rect;
	cmd->icon.color = color;
	/* reset clipping if it was set */
	if(clipped)
	{
		xui_set_clip(ctx, unclipped_rect);
	}
}

enum { RELATIVE = 1, ABSOLUTE = 2 };

void xui_layout_width(struct xui_context_t * ctx, int width)
{
	get_layout(ctx)->size.x = width;
}

void xui_layout_height(struct xui_context_t * ctx, int height)
{
	get_layout(ctx)->size.y = height;
}

void xui_layout_row(struct xui_context_t * ctx, int items, const int * widths, int height)
{
	struct xui_layout_t * layout = get_layout(ctx);
	if(widths)
	{
		expect(items <= MU_MAX_WIDTHS);
		memcpy(layout->widths, widths, items * sizeof(widths[0]));
	}
	layout->items = items;
	layout->position = mu_vec2(layout->indent, layout->next_row);
	layout->size.y = height;
	layout->item_index = 0;
}

void xui_layout_begin_column(struct xui_context_t * ctx)
{
	push_layout(ctx, xui_layout_next(ctx), mu_vec2(0, 0));
}

void xui_layout_end_column(struct xui_context_t * ctx)
{
	struct xui_layout_t * a, * b;
	b = get_layout(ctx);
	pop(ctx->layout_stack);
	a = get_layout(ctx);
	a->position.x = max(a->position.x, b->position.x + b->body.x - a->body.x);
	a->next_row = max(a->next_row, b->next_row + b->body.y - a->body.y);
	a->max.x = max(a->max.x, b->max.x);
	a->max.y = max(a->max.y, b->max.y);
}

void xui_layout_set_next(struct xui_context_t * ctx, mu_Rect r, int relative)
{
	struct xui_layout_t * layout = get_layout(ctx);
	layout->next = r;
	layout->next_type = relative ? RELATIVE : ABSOLUTE;
}

mu_Rect xui_layout_next(struct xui_context_t * ctx)
{
	struct xui_layout_t * layout = get_layout(ctx);
	struct xui_style_t * style = &ctx->style;
	mu_Rect res;

	if(layout->next_type)
	{
		int type = layout->next_type;
		layout->next_type = 0;
		res = layout->next;
		if(type == ABSOLUTE)
			return (ctx->last_rect = res);
	}
	else
	{
		if(layout->item_index == layout->items)
			xui_layout_row(ctx, layout->items, NULL, layout->size.y);

		res.x = layout->position.x;
		res.y = layout->position.y;

		res.w = layout->items > 0 ? layout->widths[layout->item_index] : layout->size.x;
		res.h = layout->size.y;
		if(res.w == 0)
			res.w = style->size.x + style->padding * 2;
		if(res.h == 0)
			res.h = style->size.y + style->padding * 2;
		if(res.w < 0)
			res.w += layout->body.w - res.x + 1;
		if(res.h < 0)
			res.h += layout->body.h - res.y + 1;
		layout->item_index++;
	}

	layout->position.x += res.w + style->spacing;
	layout->next_row = max(layout->next_row, res.y + res.h + style->spacing);

	res.x += layout->body.x;
	res.y += layout->body.y;

	layout->max.x = max(layout->max.x, res.x + res.w);
	layout->max.y = max(layout->max.y, res.y + res.h);

	return (ctx->last_rect = res);
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

void xui_draw_control_frame(struct xui_context_t * ctx, unsigned int id, mu_Rect rect, int colorid, int opt)
{
	if(opt & MU_OPT_NOFRAME)
		return;
	colorid += (ctx->focus == id) ? 2 : (ctx->hover == id) ? 1 : 0;
	ctx->draw_frame(ctx, rect, colorid);
}

void xui_draw_control_text(struct xui_context_t * ctx, const char * str, mu_Rect rect, int colorid, int opt)
{
	mu_Vec2 pos;
	void * font = ctx->style.font;
	int tw = ctx->text_width(font, str, -1);
	mu_push_clip_rect(ctx, rect);
	pos.y = rect.y + (rect.h - ctx->text_height(font)) / 2;
	if(opt & MU_OPT_ALIGNCENTER)
		pos.x = rect.x + (rect.w - tw) / 2;
	else if(opt & MU_OPT_ALIGNRIGHT)
		pos.x = rect.x + rect.w - tw - ctx->style.padding;
	else
		pos.x = rect.x + ctx->style.padding;
	mu_draw_text(ctx, font, str, -1, pos, ctx->style.colors[colorid]);
	mu_pop_clip_rect(ctx);
}

int xui_mouse_over(struct xui_context_t * ctx, mu_Rect rect)
{
	return rect_overlaps_vec2(rect, ctx->mouse_pos) && rect_overlaps_vec2(mu_get_clip_rect(ctx), ctx->mouse_pos) && in_hover_root(ctx);
}

void xui_update_control(struct xui_context_t *ctx, unsigned int id, mu_Rect rect, int opt)
{
	int mouseover = xui_mouse_over(ctx, rect);

	if(ctx->focus == id)
		ctx->updated_focus = 1;
	if(opt & MU_OPT_NOINTERACT)
		return;
	if(mouseover && !ctx->mouse_down)
		ctx->hover = id;
	if(ctx->focus == id)
	{
		if(ctx->mouse_pressed && !mouseover)
			xui_set_focus(ctx, 0);
		if(!ctx->mouse_down && (~opt & MU_OPT_HOLDFOCUS))
			xui_set_focus(ctx, 0);
	}
	if(ctx->hover == id)
	{
		if(ctx->mouse_pressed)
			xui_set_focus(ctx, id);
		else if(!mouseover)
			ctx->hover = 0;
	}
}

void mu_text(struct xui_context_t *ctx, const char *text) {
  const char *start, *end, *p = text;
  int width = -1;
  void * font = ctx->style.font;
  mu_Color color = ctx->style.colors[MU_COLOR_TEXT];
  xui_layout_begin_column(ctx);
  xui_layout_row(ctx, 1, &width, ctx->text_height(font));
  do {
    mu_Rect r = xui_layout_next(ctx);
    int w = 0;
    start = end = p;
    do {
      const char* word = p;
      while (*p && *p != ' ' && *p != '\n') { p++; }
      w += ctx->text_width(font, word, p - word);
      if (w > r.w && end != start) { break; }
      w += ctx->text_width(font, p, 1);
      end = p++;
    } while (*end && *end != '\n');
    mu_draw_text(ctx, font, start, end - start, mu_vec2(r.x, r.y), color);
    p = end + 1;
  } while (*end);
  xui_layout_end_column(ctx);
}


void mu_label(struct xui_context_t *ctx, const char *text) {
  xui_draw_control_text(ctx, text, xui_layout_next(ctx), MU_COLOR_TEXT, 0);
}


int mu_button_ex(struct xui_context_t *ctx, const char *label, int icon, int opt) {
  int res = 0;
  unsigned int id = label ? xui_get_id(ctx, label, strlen(label))
                   : xui_get_id(ctx, &icon, sizeof(icon));
  mu_Rect r = xui_layout_next(ctx);
  xui_update_control(ctx, id, r, opt);
  /* handle click */
  if (ctx->mouse_pressed == MU_MOUSE_LEFT && ctx->focus == id) {
    res |= MU_RES_SUBMIT;
  }
  /* draw */
  xui_draw_control_frame(ctx, id, r, MU_COLOR_BUTTON, opt);
  if (label) { xui_draw_control_text(ctx, label, r, MU_COLOR_TEXT, opt); }
  if (icon) { mu_draw_icon(ctx, icon, r, ctx->style.colors[MU_COLOR_TEXT]); }
  return res;
}


int mu_checkbox(struct xui_context_t *ctx, const char *label, int *state) {
  int res = 0;
  unsigned int id = xui_get_id(ctx, &state, sizeof(state));
  mu_Rect r = xui_layout_next(ctx);
  mu_Rect box = mu_rect(r.x, r.y, r.h, r.h);
  xui_update_control(ctx, id, r, 0);
  /* handle click */
  if (ctx->mouse_pressed == MU_MOUSE_LEFT && ctx->focus == id) {
    res |= MU_RES_CHANGE;
    *state = !*state;
  }
  /* draw */
  xui_draw_control_frame(ctx, id, box, MU_COLOR_BASE, 0);
  if (*state) {
    mu_draw_icon(ctx, MU_ICON_CHECK, box, ctx->style.colors[MU_COLOR_TEXT]);
  }
  r = mu_rect(r.x + box.w, r.y, r.w - box.w, r.h);
  xui_draw_control_text(ctx, label, r, MU_COLOR_TEXT, 0);
  return res;
}


int mu_textbox_raw(struct xui_context_t *ctx, char *buf, int bufsz, unsigned int id, mu_Rect r,
  int opt)
{
  int res = 0;
  xui_update_control(ctx, id, r, opt | MU_OPT_HOLDFOCUS);

  if (ctx->focus == id) {
    /* handle text input */
    int len = strlen(buf);
    int n = min(bufsz - len - 1, (int) strlen(ctx->input_text));
    if (n > 0) {
      memcpy(buf + len, ctx->input_text, n);
      len += n;
      buf[len] = '\0';
      res |= MU_RES_CHANGE;
    }
    /* handle backspace */
    if ((ctx->key_pressed & MU_KEY_BACKSPACE) && len > 0) {
      /* skip utf-8 continuation bytes */
      while ((buf[--len] & 0xc0) == 0x80 && len > 0);
      buf[len] = '\0';
      res |= MU_RES_CHANGE;
    }
    /* handle return */
    if (ctx->key_pressed & MU_KEY_RETURN) {
      xui_set_focus(ctx, 0);
      res |= MU_RES_SUBMIT;
    }
  }

  /* draw */
  xui_draw_control_frame(ctx, id, r, MU_COLOR_BASE, opt);
  if (ctx->focus == id) {
    mu_Color color = ctx->style.colors[MU_COLOR_TEXT];
    void * font = ctx->style.font;
    int textw = ctx->text_width(font, buf, -1);
    int texth = ctx->text_height(font);
    int ofx = r.w - ctx->style.padding - textw - 1;
    int textx = r.x + min(ofx, ctx->style.padding);
    int texty = r.y + (r.h - texth) / 2;
    mu_push_clip_rect(ctx, r);
    mu_draw_text(ctx, font, buf, -1, mu_vec2(textx, texty), color);
    mu_draw_rect(ctx, mu_rect(textx + textw, texty, 1, texth), color);
    mu_pop_clip_rect(ctx);
  } else {
    xui_draw_control_text(ctx, buf, r, MU_COLOR_TEXT, opt);
  }

  return res;
}


static int number_textbox(struct xui_context_t *ctx, float *value, mu_Rect r, unsigned int id) {
  if (ctx->mouse_pressed == MU_MOUSE_LEFT && (ctx->key_down & MU_KEY_SHIFT) &&
      ctx->hover == id
  ) {
    ctx->number_edit = id;
    sprintf(ctx->number_edit_buf, MU_REAL_FMT, *value);
  }
  if (ctx->number_edit == id) {
    int res = mu_textbox_raw(
      ctx, ctx->number_edit_buf, sizeof(ctx->number_edit_buf), id, r, 0);
    if ((res & MU_RES_SUBMIT) || ctx->focus != id) {
      *value = strtod(ctx->number_edit_buf, NULL);
      ctx->number_edit = 0;
    } else {
      return 1;
    }
  }
  return 0;
}


int mu_textbox_ex(struct xui_context_t *ctx, char *buf, int bufsz, int opt) {
  unsigned int id = xui_get_id(ctx, &buf, sizeof(buf));
  mu_Rect r = xui_layout_next(ctx);
  return mu_textbox_raw(ctx, buf, bufsz, id, r, opt);
}


int mu_slider_ex(struct xui_context_t *ctx, float *value, float low, float high,
  float step, const char *fmt, int opt)
{
  char buf[MU_MAX_FMT + 1];
  mu_Rect thumb;
  int x, w, res = 0;
  float last = *value, v = last;
  unsigned int id = xui_get_id(ctx, &value, sizeof(value));
  mu_Rect base = xui_layout_next(ctx);

  /* handle text input mode */
  if (number_textbox(ctx, &v, base, id)) { return res; }

  /* handle normal mode */
  xui_update_control(ctx, id, base, opt);

  /* handle input */
  if (ctx->focus == id && ctx->mouse_down == MU_MOUSE_LEFT) {
    v = low + (ctx->mouse_pos.x - base.x) * (high - low) / base.w;
    if (step) { v = (((v + step / 2) / step)) * step; }
  }
  /* clamp and store value, update res */
  *value = v = clamp(v, low, high);
  if (last != v) { res |= MU_RES_CHANGE; }

  /* draw base */
  xui_draw_control_frame(ctx, id, base, MU_COLOR_BASE, opt);
  /* draw thumb */
  w = ctx->style.thumb_size;
  x = (v - low) * (base.w - w) / (high - low);
  thumb = mu_rect(base.x + x, base.y, w, base.h);
  xui_draw_control_frame(ctx, id, thumb, MU_COLOR_BUTTON, opt);
  /* draw text  */
  sprintf(buf, fmt, v);
  xui_draw_control_text(ctx, buf, base, MU_COLOR_TEXT, opt);

  return res;
}


int mu_number_ex(struct xui_context_t *ctx, float *value, float step,
  const char *fmt, int opt)
{
  char buf[MU_MAX_FMT + 1];
  int res = 0;
  unsigned int id = xui_get_id(ctx, &value, sizeof(value));
  mu_Rect base = xui_layout_next(ctx);
  float last = *value;

  /* handle text input mode */
  if (number_textbox(ctx, value, base, id)) { return res; }

  /* handle normal mode */
  xui_update_control(ctx, id, base, opt);

  /* handle input */
  if (ctx->focus == id && ctx->mouse_down == MU_MOUSE_LEFT) {
    *value += ctx->mouse_delta.x * step;
  }
  /* set flag if value changed */
  if (*value != last) { res |= MU_RES_CHANGE; }

  /* draw base */
  xui_draw_control_frame(ctx, id, base, MU_COLOR_BASE, opt);
  /* draw text  */
  sprintf(buf, fmt, *value);
  xui_draw_control_text(ctx, buf, base, MU_COLOR_TEXT, opt);

  return res;
}


static int header(struct xui_context_t *ctx, const char *label, int istreenode, int opt) {
  mu_Rect r;
  int active, expanded;
  unsigned int id = xui_get_id(ctx, label, strlen(label));
  int idx = xui_pool_get(ctx, ctx->treenode_pool, MU_TREENODEPOOL_SIZE, id);
  int width = -1;
  xui_layout_row(ctx, 1, &width, 0);

  active = (idx >= 0);
  expanded = (opt & MU_OPT_EXPANDED) ? !active : active;
  r = xui_layout_next(ctx);
  xui_update_control(ctx, id, r, 0);

  /* handle click */
  active ^= (ctx->mouse_pressed == MU_MOUSE_LEFT && ctx->focus == id);

  /* update pool ref */
  if (idx >= 0) {
    if (active) { xui_pool_update(ctx, ctx->treenode_pool, idx); }
           else { memset(&ctx->treenode_pool[idx], 0, sizeof(struct xui_pool_item_t)); }
  } else if (active) {
    xui_pool_init(ctx, ctx->treenode_pool, MU_TREENODEPOOL_SIZE, id);
  }

  /* draw */
  if (istreenode) {
    if (ctx->hover == id) { ctx->draw_frame(ctx, r, MU_COLOR_BUTTONHOVER); }
  } else {
    xui_draw_control_frame(ctx, id, r, MU_COLOR_BUTTON, 0);
  }
  mu_draw_icon(
    ctx, expanded ? MU_ICON_EXPANDED : MU_ICON_COLLAPSED,
    mu_rect(r.x, r.y, r.h, r.h), ctx->style.colors[MU_COLOR_TEXT]);
  r.x += r.h - ctx->style.padding;
  r.w -= r.h - ctx->style.padding;
  xui_draw_control_text(ctx, label, r, MU_COLOR_TEXT, 0);

  return expanded ? MU_RES_ACTIVE : 0;
}


int mu_header_ex(struct xui_context_t *ctx, const char *label, int opt) {
  return header(ctx, label, 0, opt);
}


int mu_begin_treenode_ex(struct xui_context_t *ctx, const char *label, int opt) {
  int res = header(ctx, label, 1, opt);
  if (res & MU_RES_ACTIVE) {
    get_layout(ctx)->indent += ctx->style.indent;
    push(ctx->id_stack, ctx->last_id);
  }
  return res;
}


void mu_end_treenode(struct xui_context_t *ctx) {
  get_layout(ctx)->indent -= ctx->style.indent;
  xui_pop_id(ctx);
}


#define scrollbar(ctx, c, b, cs, x, y, w, h)                              \
  do {                                                                      \
    /* only add scrollbar if content size is larger than body */            \
    int maxscroll = cs.y - b->h;                                            \
                                                                            \
    if (maxscroll > 0 && b->h > 0) {                                        \
      mu_Rect base, thumb;                                                  \
      unsigned int id = xui_get_id(ctx, "!scrollbar" #y, 11);                       \
                                                                            \
      /* get sizing / positioning */                                        \
      base = *b;                                                            \
      base.x = b->x + b->w;                                                 \
      base.w = ctx->style.scrollbar_size;                                  \
                                                                            \
      /* handle input */                                                    \
      xui_update_control(ctx, id, base, 0);                                  \
      if (ctx->focus == id && ctx->mouse_down == MU_MOUSE_LEFT) {           \
        c->scroll.y += ctx->mouse_delta.y * cs.y / base.h;                \
      }                                                                     \
      /* clamp scroll to limits */                                          \
      c->scroll.y = clamp(c->scroll.y, 0, maxscroll);  	                \
                                                                            \
      /* draw base and thumb */                                             \
      ctx->draw_frame(ctx, base, MU_COLOR_SCROLLBASE);                      \
      thumb = base;                                                         \
      thumb.h = max(ctx->style.thumb_size, base.h * b->h / cs.y);       \
      thumb.y += c->scroll.y * (base.h - thumb.h) / maxscroll;            \
      ctx->draw_frame(ctx, thumb, MU_COLOR_SCROLLTHUMB);                    \
                                                                            \
      /* set this as the scroll_target (will get scrolled on mousewheel) */ \
      /* if the mouse is over it */                                         \
      if (xui_mouse_over(ctx, *b)) { ctx->scroll_target = c; }             \
    } else {                                                                \
      c->scroll.y = 0;                                                    \
    }                                                                       \
  } while (0)


static void scrollbars(struct xui_context_t *ctx, struct xui_container_t *c, mu_Rect *body) {
  int sz = ctx->style.scrollbar_size;
  mu_Vec2 cs = c->content_size;
  cs.x += ctx->style.padding * 2;
  cs.y += ctx->style.padding * 2;
  mu_push_clip_rect(ctx, *body);
  /* resize body to make room for scrollbars */
  if (cs.y > c->body.h) { body->w -= sz; }
  if (cs.x > c->body.w) { body->h -= sz; }
  /* to create a horizontal or vertical scrollbar almost-identical code is
  ** used; only the references to `x|y` `w|h` need to be switched */
  scrollbar(ctx, c, body, cs, x, y, w, h);
  scrollbar(ctx, c, body, cs, y, x, h, w);
  mu_pop_clip_rect(ctx);
}


static void push_container_body(
  struct xui_context_t *ctx, struct xui_container_t *c, mu_Rect body, int opt
) {
  if (~opt & MU_OPT_NOSCROLL) { scrollbars(ctx, c, &body); }
  push_layout(ctx, expand_rect(body, -ctx->style.padding), c->scroll);
  c->body = body;
}


static void begin_root_container(struct xui_context_t *ctx, struct xui_container_t *c) {
  push(ctx->container_stack, c);
  /* push container to roots list and push head command */
  push(ctx->root_list, c);
  c->head = push_jump(ctx, NULL);
  /* set as hover root if the mouse is overlapping this container and it has a
  ** higher zindex than the current hover root */
  if (rect_overlaps_vec2(c->rect, ctx->mouse_pos) &&
      (!ctx->next_hover_root || c->zindex > ctx->next_hover_root->zindex)
  ) {
    ctx->next_hover_root = c;
  }
  /* clipping is reset here in case a root-container is made within
  ** another root-containers's begin/end block; this prevents the inner
  ** root-container being clipped to the outer */
  push(ctx->clip_stack, unclipped_rect);
}


static void end_root_container(struct xui_context_t *ctx) {
  /* push tail 'goto' jump command and set head 'skip' command. the final steps
  ** on initing these are done in xui_end() */
  struct xui_container_t *c = xui_get_current_container(ctx);
  c->tail = push_jump(ctx, NULL);
  c->head->jump.addr = ctx->command_list.items + ctx->command_list.idx;
  /* pop base clip rect and container */
  mu_pop_clip_rect(ctx);
  pop_container(ctx);
}


int mu_begin_window_ex(struct xui_context_t *ctx, const char *title, mu_Rect rect, int opt) {
  mu_Rect body;
  unsigned int id = xui_get_id(ctx, title, strlen(title));
  struct xui_container_t *c = get_container(ctx, id, opt);
  if (!c || !c->open) { return 0; }
  push(ctx->id_stack, id);

  if (c->rect.w == 0) { c->rect = rect; }
  begin_root_container(ctx, c);
  rect = body = c->rect;

  /* draw frame */
  if (~opt & MU_OPT_NOFRAME) {
    ctx->draw_frame(ctx, rect, MU_COLOR_WINDOWBG);
  }

  /* do title bar */
  if (~opt & MU_OPT_NOTITLE) {
    mu_Rect tr = rect;
    tr.h = ctx->style.title_height;
    ctx->draw_frame(ctx, tr, MU_COLOR_TITLEBG);

    /* do title text */
    if (~opt & MU_OPT_NOTITLE) {
      unsigned int id = xui_get_id(ctx, "!title", 6);
      xui_update_control(ctx, id, tr, opt);
      xui_draw_control_text(ctx, title, tr, MU_COLOR_TITLETEXT, opt);
      if (id == ctx->focus && ctx->mouse_down == MU_MOUSE_LEFT) {
        c->rect.x += ctx->mouse_delta.x;
        c->rect.y += ctx->mouse_delta.y;
      }
      body.y += tr.h;
      body.h -= tr.h;
    }

    /* do `close` button */
    if (~opt & MU_OPT_NOCLOSE) {
      unsigned int id = xui_get_id(ctx, "!close", 6);
      mu_Rect r = mu_rect(tr.x + tr.w - tr.h, tr.y, tr.h, tr.h);
      tr.w -= r.w;
      mu_draw_icon(ctx, MU_ICON_CLOSE, r, ctx->style.colors[MU_COLOR_TITLETEXT]);
      xui_update_control(ctx, id, r, opt);
      if (ctx->mouse_pressed == MU_MOUSE_LEFT && id == ctx->focus) {
        c->open = 0;
      }
    }
  }

  push_container_body(ctx, c, body, opt);

  /* do `resize` handle */
  if (~opt & MU_OPT_NORESIZE) {
    int sz = ctx->style.title_height;
    unsigned int id = xui_get_id(ctx, "!resize", 7);
    mu_Rect r = mu_rect(rect.x + rect.w - sz, rect.y + rect.h - sz, sz, sz);
    xui_update_control(ctx, id, r, opt);
    if (id == ctx->focus && ctx->mouse_down == MU_MOUSE_LEFT) {
      c->rect.w = max(96, c->rect.w + ctx->mouse_delta.x);
      c->rect.h = max(64, c->rect.h + ctx->mouse_delta.y);
    }
  }

  /* resize to content size */
  if (opt & MU_OPT_AUTOSIZE) {
    mu_Rect r = get_layout(ctx)->body;
    c->rect.w = c->content_size.x + (c->rect.w - r.w);
    c->rect.h = c->content_size.y + (c->rect.h - r.h);
  }

  /* close if this is a popup window and elsewhere was clicked */
  if ((opt & MU_OPT_POPUP) && ctx->mouse_pressed && ctx->hover_root != c) {
    c->open = 0;
  }

  mu_push_clip_rect(ctx, c->body);
  return MU_RES_ACTIVE;
}


void mu_end_window(struct xui_context_t *ctx) {
  mu_pop_clip_rect(ctx);
  end_root_container(ctx);
}


void mu_open_popup(struct xui_context_t *ctx, const char *name) {
  struct xui_container_t *c = xui_get_container(ctx, name);
  /* set as hover root so popup isn't closed in begin_window_ex()  */
  ctx->hover_root = ctx->next_hover_root = c;
  /* position at mouse cursor, open and bring-to-front */
  c->rect = mu_rect(ctx->mouse_pos.x, ctx->mouse_pos.y, 1, 1);
  c->open = 1;
  xui_set_front(ctx, c);
}


int mu_begin_popup(struct xui_context_t *ctx, const char *name) {
  int opt = MU_OPT_POPUP | MU_OPT_AUTOSIZE | MU_OPT_NORESIZE |
            MU_OPT_NOSCROLL | MU_OPT_NOTITLE | MU_OPT_CLOSED;
  return mu_begin_window_ex(ctx, name, mu_rect(0, 0, 0, 0), opt);
}

void mu_end_popup(struct xui_context_t *ctx) {
  mu_end_window(ctx);
}

void mu_begin_panel_ex(struct xui_context_t *ctx, const char *name, int opt) {
  struct xui_container_t *c;
  xui_push_id(ctx, name, strlen(name));
  c = get_container(ctx, ctx->last_id, opt);
  c->rect = xui_layout_next(ctx);
  if (~opt & MU_OPT_NOFRAME) {
    ctx->draw_frame(ctx, c->rect, MU_COLOR_PANELBG);
  }
  push(ctx->container_stack, c);
  push_container_body(ctx, c, c->rect, opt);
  mu_push_clip_rect(ctx, c->body);
}

void mu_end_panel(struct xui_context_t *ctx) {
  mu_pop_clip_rect(ctx);
  pop_container(ctx);
}
