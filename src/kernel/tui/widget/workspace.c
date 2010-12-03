/*
 * kernel/tui/widget/workspace.c
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
#include <tui/widget/widget.h>
#include <tui/widget/workspace.h>


static x_bool tui_workspace_minsize(struct tui_widget * widget, x_s32 * width, x_s32 * height)
{
	if(width)
		*width = 2;
	if(height)
		*height = 2;

	return TRUE;
}

static x_bool tui_workspace_region(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h)
{
	if(x)
		*x = 0;
	if(y)
		*y = 0;
	if(w)
		*w = widget->width;
	if(h)
		*h = widget->height;

	return TRUE;
}

static x_bool tui_workspace_setbounds(struct tui_widget * widget, x_s32 ox, x_s32 oy, x_s32 width, x_s32 height)
{
	struct tui_theme * theme = get_tui_theme();
	struct tui_cell * cell;
	x_s32 w, h;
	x_s32 len;

	if(!tui_workspace_minsize(widget, &w, &h))
		return FALSE;

	if(width < w)
		width = w;

	if(height < h)
		height = h;

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
							theme->workspace.cp,
							theme->workspace.fg, theme->workspace.bg,
							0, 0, width, height);

	return TRUE;
}

static x_bool tui_workspace_getbounds(struct tui_widget * widget, x_s32 * ox, x_s32 * oy, x_s32 * width, x_s32 * height)
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

static x_bool tui_workspace_setproperty(struct tui_widget * widget, const x_s8 * name, const x_s8 * value)
{
	return FALSE;
}

static x_bool tui_workspace_paint(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	struct tui_workspace * workspace = widget->priv;
	struct tui_theme * theme = get_tui_theme();
	struct tui_cell * cell, * p;
	struct tui_widget * list;
	struct list_head * pos;
	struct rect r, a, b;
	x_bool cursor;
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
							theme->workspace.cp,
							theme->workspace.fg, theme->workspace.bg,
							x, y, w, h);

	for(pos = (&widget->child)->next; pos != (&widget->child); pos = pos->next)
	{
		list = list_entry(pos, struct tui_widget, entry);

		if(list->ops->paint)
			list->ops->paint(list, x, y, w, h);
	}

	if(widget->active)
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
	else
	{
		cursor = console_getcursor(workspace->console);
		console_setcursor(workspace->console, FALSE);

		for(j = y; j < h; j++)
		{
			cell = &(widget->cell[widget->width * j + x]);
			for(i = x; i < w; i++)
			{
				if(cell->dirty)
				{
					console_gotoxy(workspace->console, widget->ox + i, widget->oy + j);
					console_setcolor(workspace->console, cell->fg, cell->bg);
					console_putcode(workspace->console, cell->cp);

					cell->dirty = FALSE;
				}

				cell++;
			}
		}

		console_setcursor(workspace->console, cursor);
	}

	return TRUE;
}

static x_bool tui_workspace_destroy(struct tui_widget * widget)
{
	struct tui_workspace * workspace = widget->priv;
	struct tui_widget * list;
	struct list_head * head, * curr, * next;

	head = &workspace->widget.child;
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

	list_del(&workspace->widget.entry);

	console_setcolor(workspace->console, workspace->f, workspace->b);
	console_gotoxy(workspace->console, workspace->x, workspace->y);
	console_setcursor(workspace->console, workspace->cursor);

	free(workspace->widget.id);
	free(workspace->widget.cell);
	free(workspace);

	return TRUE;
}

static struct tui_widget_ops workspace_ops = {
	.minsize			= tui_workspace_minsize,
	.region				= tui_workspace_region,
	.setbounds			= tui_workspace_setbounds,
	.getbounds			= tui_workspace_getbounds,
	.setproperty		= tui_workspace_setproperty,
	.paint				= tui_workspace_paint,
	.destroy			= tui_workspace_destroy,
};

struct tui_workspace * tui_workspace_new(struct console * console, const x_s8 * id)
{
	struct tui_theme * theme = get_tui_theme();
	struct tui_workspace * workspace;
	enum tcolor f, b;
	x_s32 x, y;
	x_s32 w, h;

	if(!id)
		return NULL;

	if(!console || !console->putcode)
		return NULL;

	if(!console_getxy(console, &x, &y))
		return NULL;

	if(!console_getwh(console, &w, &h))
		return NULL;

	if(!console_getcolor(console, &f, &b))
		return NULL;

	workspace = malloc(sizeof(struct tui_workspace));
	if(!workspace)
		return NULL;

	workspace->widget.id = strdup(id);
	workspace->widget.active = FALSE;
	workspace->widget.ox = 0;
	workspace->widget.oy = 0;
	workspace->widget.width = w;
	workspace->widget.height = h;
	workspace->widget.layout = NULL;
	workspace->widget.ops = &workspace_ops;
	workspace->widget.parent = (struct tui_widget *)workspace;
	workspace->widget.priv = workspace;

	workspace->widget.clen = workspace->widget.width * workspace->widget.height;
	workspace->widget.cell = malloc(workspace->widget.clen * sizeof(struct tui_cell));
	if(!workspace->widget.cell)
	{
		free(workspace->widget.id);
		free(workspace);
		return NULL;
	}

	tui_widget_cell_clear(TUI_WIDGET(workspace),
							theme->workspace.cp,
							theme->workspace.fg, theme->workspace.bg,
							0, 0, workspace->widget.width, workspace->widget.height);

	workspace->console = console;
	workspace->cursor = console_getcursor(console);
	workspace->x = x;
	workspace->y = y;
	workspace->f = f;
	workspace->b = b;

	init_list_head(&(workspace->widget.entry));
	init_list_head(&(workspace->widget.child));

	return workspace;
}
