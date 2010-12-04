/*
 * kernel/tui/widget/button.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <charset.h>
#include <tui/tui.h>
#include <tui/theme.h>
#include <tui/widget/widget.h>
#include <tui/widget/button.h>


static x_bool tui_button_minsize(struct tui_widget * widget, x_s32 * width, x_s32 * height)
{
	if(width)
		*width = 3;
	if(height)
		*height = 3;

	return TRUE;
}

static x_bool tui_button_region(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h)
{
	if(x)
		*x = 1;
	if(y)
		*y = 1;
	if(w)
		*w = widget->width - 2;
	if(h)
		*h = widget->height - 2;

	return TRUE;
}

static x_bool tui_button_setbounds(struct tui_widget * widget, x_s32 ox, x_s32 oy, x_s32 width, x_s32 height)
{
	struct tui_theme * theme = get_tui_theme();
	struct tui_cell * cell;
	x_s32 w, h;
	x_s32 len;

	if(!tui_button_minsize(widget, &w, &h))
		return FALSE;

	if(width < w)
		width = w;

	if(height < h)
		height = h;

	if((widget->parent != NULL) && (widget->parent != widget))
	{
		//xxx
	}

	if((width == widget->width) && (height == widget->height))
	{
		widget->ox = ox;
		widget->oy = oy;
		widget->width = width;
		widget->height = height;

		return TRUE;
	}

	len = width * height;
	cell = malloc(len * sizeof(struct tui_cell));
	if(!cell)
		return FALSE;

	free(widget->cell);
	widget->cell = cell;
	widget->clen = len;

	widget->ox = ox;
	widget->oy = oy;
	widget->width = width;
	widget->height = height;

	tui_widget_cell_clear(widget,
							theme->button.cp,
							theme->button.fg, theme->button.bg,
							0, 0, width, height);

	return TRUE;
}

static x_bool tui_button_getbounds(struct tui_widget * widget, x_s32 * ox, x_s32 * oy, x_s32 * width, x_s32 * height)
{
	if(ox)
		*ox = widget->ox;
	if(oy)
		*oy = widget->oy;
	if(width)
		*width = widget->width;
	if(height)
		*height = widget->height;

	return TRUE;
}

static x_bool tui_button_setproperty(struct tui_widget * widget, x_u32 cmd, void * arg)
{
	struct tui_button * button = widget->priv;

	switch(cmd)
	{
	case TUI_BUTTON_SET_CAPTION:
		free(button->caption);
		if(arg)
			button->caption = utf8_strdup((const x_s8 *)arg);
		else
			button->caption = utf8_strdup((const x_s8 *)"");
		return TRUE;

	case TUI_BUTTON_GET_CAPTION:
		if(arg)
		{
			arg = (void *)button->caption;
			return TRUE;
		}
		return FALSE;

	default:
		break;
	}

	return FALSE;
}

static x_bool tui_button_paint(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	struct tui_button * button = widget->priv;
	struct tui_theme * theme = get_tui_theme();
	struct tui_cell * cell, * p;
	struct tui_widget * list;
	struct list_head * pos;
	struct rect r, a, b;
	x_s32 cx, cy, clen;
	x_s32 i, j;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = 0;
	b.top = 0;
	b.right =  widget->width;
	b.bottom = widget->height;

	if(rect_intersect(&r, &a, & b) == FALSE)
		return TRUE;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	tui_widget_cell_clear(widget,
							theme->button.cp,
							theme->button.fg, theme->button.bg,
							x, y, w, h);
	tui_widget_cell_rect(widget,
							theme->button.h, theme->button.v,
							theme->button.lt, theme->button.rt,
							theme->button.lb, theme->button.rb,
							theme->button.b_fg, theme->button.b_bg,
							x, y, w, h);

	clen = utf8_strlen(button->caption);
	if(clen > widget->width - 2)
		clen = widget->width - 2;

	cx = (widget->width - 2 - clen) / 2 + 1;
	cy = (widget->height - 2) / 2 + 1;

	tui_widget_cell_print(widget,
							button->caption,
							theme->button.c_fg, theme->button.c_bg,
							cx, cy, clen);

	for(pos = (&widget->child)->next; pos != (&widget->child); pos = pos->next)
	{
		list = list_entry(pos, struct tui_widget, entry);

		if(list->ops->paint)
			list->ops->paint(list, x, y, w, h);
	}

	if(widget->focus)
		tui_widget_cell_border(widget);

	if((widget->parent != NULL) && (widget->parent != widget))
	{
		for(j = y; j < h; j++)
		{
			p = &(widget->parent->cell[widget->parent->width * (widget->oy + j) + widget->ox + x]);
			cell = &(widget->cell[widget->width * j + x]);
			for(i = x; i < w; i++)
			{
				if(cell->dirty)
				{
					p->cp = cell->cp;
					p->fg = cell->fg;
					p->bg = cell->bg;
					p->dirty = TRUE;
				}
				else
				{
					if( (p->cp != cell->cp) || (p->fg != cell->fg) || (p->bg != cell->bg) )
					{
						p->cp = cell->cp;
						p->fg = cell->fg;
						p->bg = cell->bg;
						p->dirty = TRUE;
					}
				}

				cell->dirty = FALSE;

				p++;
				cell++;
			}
		}
	}

	return TRUE;
}

static x_bool tui_button_process(struct tui_widget * widget, struct tui_event * event)
{
	return TRUE;
}

static x_bool tui_button_destroy(struct tui_widget * widget)
{
	struct tui_button * button = widget->priv;
	struct tui_widget * list;
	struct list_head * head, * curr, * next;

	head = &button->widget.child;
	curr = head->next;

	while(curr != head)
	{
		list = list_entry(curr, struct tui_widget, entry);

		next = curr->next;
		list_del(curr);
		if(list->ops->destroy)
			list->ops->destroy(list);
		curr = next;
	}

	list_del(&button->widget.entry);

	free(button->widget.id);
	free(button->widget.cell);
	free(button->caption);
	free(button);

	return TRUE;
}

static struct tui_widget_ops button_ops = {
	.minsize			= tui_button_minsize,
	.region				= tui_button_region,
	.setbounds			= tui_button_setbounds,
	.getbounds			= tui_button_getbounds,
	.setproperty		= tui_button_setproperty,
	.paint				= tui_button_paint,
	.process			= tui_button_process,
	.destroy			= tui_button_destroy,
};

struct tui_button * tui_button_new(struct tui_widget * parent, const x_s8 * id, const x_s8 * caption)
{
	struct tui_theme * theme = get_tui_theme();
	struct tui_button * button;

	if(!parent)
		return NULL;

	if(!id || !caption)
		return NULL;

	button = malloc(sizeof(struct tui_button));
	if(!button)
		return NULL;

	button->widget.id = utf8_strdup(id);
	button->widget.align = TUI_WIDGET_ALIGN_NONE;
	button->widget.ox = 0;
	button->widget.oy = 0;
	button->widget.width = 8;
	button->widget.height = 3;
	button->widget.focus = FALSE;
	button->widget.ops = &button_ops;
	button->widget.parent = parent;
	button->widget.priv = button;

	button->widget.clen = button->widget.width * button->widget.height;
	button->widget.cell = malloc(button->widget.clen * sizeof(struct tui_cell));
	if(!button->widget.cell)
	{
		free(button->widget.id);
		free(button);
		return NULL;
	}

	tui_widget_cell_clear(TUI_WIDGET(button),
							theme->button.cp,
							theme->button.fg, theme->button.bg,
							0, 0, button->widget.width, button->widget.height);

	button->caption = utf8_strdup(caption);

	init_list_head(&(button->widget.entry));
	init_list_head(&(button->widget.child));
	list_add_tail(&(button->widget.entry), &(parent->child));

	return button;
}
