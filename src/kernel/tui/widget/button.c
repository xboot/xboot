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
#include <tui/tui.h>
#include <tui/theme.h>
#include <tui/widget/button.h>


static x_bool tui_button_minsize(struct tui_widget * widget, x_s32 * width, x_s32 * height)
{
	if(width)
		*width = 4;
	if(height)
		*height = 4;

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
	enum console_color fg, bg;
	x_u32 cp;
	x_s32 w, h;
	x_s32 i, len;

	if(!tui_button_minsize(widget, &w, &h))
		return FALSE;

	if(width < w)
		width = w;

	if(height < h)
		height = h;

	len = width * height;
	if(len != widget->clen)
	{
		cell = malloc(len * sizeof(struct tui_cell));
		if(!cell)
			return FALSE;

		free(widget->cell);

		widget->cell = cell;
		widget->clen = len;

		fg = theme->button.fg;
		bg = theme->button.bg;
		cp = theme->button.cp;

		for(i = 0; i < widget->clen; i++)
		{
			cell->cp = cp;
			cell->fg = fg;
			cell->bg = bg;
			cell->dirty = TRUE;

			cell++;
		}
	}

	widget->ox = ox;
	widget->oy = oy;
	widget->width = width;
	widget->height = height;

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

static x_bool tui_button_setproperty(struct tui_widget * widget, const x_s8 * name, const x_s8 * value)
{
	struct tui_button * button = widget->priv;

	if(strcmp(name, (const x_s8 *)"caption") == 0)
	{
		free(button->caption);
		if(value)
			button->caption = strdup(value);
		else
			button->caption = strdup((const x_s8 *)"");
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

static x_bool tui_button_paint(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	struct tui_button * button = widget->priv;
	struct tui_theme * theme = get_tui_theme();
	struct tui_cell * cell, * p;
	struct tui_widget * list;
	struct list_head * pos;
	struct rect r, a, b;
	enum console_color fg, bg;
	x_u32 cp;
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

	cp = theme->button.cp;
	fg = theme->button.fg;
	bg = theme->button.bg;

	for(j = y; j < h; j++)
	{
		cell = &(widget->cell[widget->width * j + x]);
		for(i = x; i < w; i++)
		{
			if( (cell->cp != cp) || (cell->fg != fg) || (cell->bg != bg) )
			{
				cell->cp = cp;
				cell->fg = fg;
				cell->bg = bg;
				cell->dirty = TRUE;
			}
			cell++;
		}
	}

	for(pos = (&widget->child)->next; pos != (&widget->child); pos = pos->next)
	{
		list = list_entry(pos, struct tui_widget, entry);

		if(list->ops->paint)
			list->ops->paint(list, x, y, w, h);
	}

	if((widget->parent != NULL) && (widget->parent != widget))
	{
		for(j = y; j < h; j++)
		{
			p = &(widget->parent->cell[widget->parent->width * (widget->oy + j) + widget->ox + x]);
			cell = &(widget->cell[widget->width * j + x]);
			for(i = x; i < w; i++)
			{
				if( (p->cp != cell->cp) || (p->fg != cell->fg) || (p->bg != cell->bg) )
				{
					p->cp = cell->cp;
					p->fg = cell->fg;
					p->bg = cell->bg;
					p->dirty = TRUE;
				}
				p++;
				cell->dirty = FALSE;
				cell++;
			}
		}
	}

	return TRUE;
}

static x_bool tui_button_destory(struct tui_widget * widget)
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
		if(list->ops->destory)
			list->ops->destory(list);
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
	.destory			= tui_button_destory,
};

struct tui_button * tui_button_new(struct tui_widget * parent, const x_s8 * id, const x_s8 * caption)
{
	struct tui_theme * theme = get_tui_theme();
	struct tui_button * button;
	struct tui_cell * cell;
	x_s32 i;

	if(!parent)
		return NULL;

	if(!id || !caption)
		return NULL;

	button = malloc(sizeof(struct tui_button));
	if(!button)
		return NULL;

	button->widget.id = strdup(id);
	button->widget.ox = 0;
	button->widget.oy = 0;
	button->widget.width = 8;
	button->widget.height = 4;
	button->widget.layout = NULL;
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

	cell = button->widget.cell;
	for(i = 0; i < button->widget.clen; i++)
	{
		cell->cp = theme->button.cp;
		cell->fg = theme->button.fg;
		cell->bg = theme->button.bg;
		cell->dirty = TRUE;

		cell++;
	}

	button->widget.id = strdup(id);
	button->widget.ox = 0;
	button->widget.oy = 0;
	button->widget.width = 8;
	button->widget.height = 4;
	button->widget.layout = NULL;
	button->widget.ops = &button_ops;
	button->widget.parent = parent;
	button->widget.priv = button;

	button->caption = strdup(caption);
	button->cf = CONSOLE_WHITE;
	button->cb = CONSOLE_BLACK;
	button->align = ALIGN_CENTER;

	init_list_head(&(button->widget.entry));
	init_list_head(&(button->widget.child));
	list_add_tail(&(parent->child), &(button->widget.entry));

	return button;
}
