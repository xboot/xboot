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
#include <tui/widget/workspace.h>

static x_bool tui_workspace_setbounds(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	return TRUE;
}

static x_bool tui_workspace_getbounds(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h)
{
	struct tui_workspace * workspace = widget->priv;

	if(x)
		*x = workspace->widget.x;
	if(y)
		*y = workspace->widget.y;
	if(w)
		*w = workspace->widget.w;
	if(h)
		*h = workspace->widget.h;

	return TRUE;
}

static x_bool tui_workspace_minsize(struct tui_widget * widget, x_s32 * w, x_s32 * h)
{
	struct tui_workspace * workspace = widget->priv;

	if(w)
		*w = workspace->widget.w;
	if(h)
		*h = workspace->widget.h;

	return TRUE;
}

static x_bool tui_workspace_region(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h)
{
	struct tui_workspace * workspace = widget->priv;

	if(x)
		*x = workspace->widget.x;
	if(y)
		*y = workspace->widget.y;
	if(w)
		*w = workspace->widget.w;
	if(h)
		*h = workspace->widget.h;

	return TRUE;
}

static x_bool tui_workspace_setproperty(struct tui_widget * widget, const x_s8 * name, const x_s8 * value)
{
	return FALSE;
}

static x_bool tui_workspace_paint(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	struct tui_workspace * workspace = widget->priv;
	struct tui_widget * list;
	struct list_head * pos;
	struct rect r, a, b;
	enum console_color fg, bg;
	x_u32 code;
	x_s32 i, j;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = widget->x;
	b.top = widget->y;
	b.right = widget->x + widget->w;
	b.bottom = widget->y + widget->h;

	if(rect_intersect(&r, &a, & b) == FALSE)
		return TRUE;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	fg = get_tui_theme()->workspace.fg;
	bg = get_tui_theme()->workspace.bg;
	code = get_tui_theme()->workspace.c;

	console_setcolor(workspace->widget.console, fg, bg);

	for(j = y; j < h; j++)
	{
		console_gotoxy(workspace->widget.console, x, j);
		for(i = x; i < w; i++)
		{
			console_putcode(workspace->widget.console, code);
		}
	}

	for(pos = (&widget->child)->next; pos != (&widget->child); pos = pos->next)
	{
		list = list_entry(pos, struct tui_widget, child);

		if(list->ops->paint)
			list->ops->paint(list, x, y, w, h);
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
		list = list_entry(curr, struct tui_widget, child);

		next = curr->next;
		list_del(curr);
		if(list->ops->destroy)
			list->ops->destroy(list);
		curr = next;
	}

	list_del(&workspace->widget.entry);

	free(workspace->widget.id);
	free(workspace);

	return TRUE;
}

static struct tui_widget_ops workspace_ops = {
	.setbounds			= tui_workspace_setbounds,
	.getbounds			= tui_workspace_getbounds,
	.minsize			= tui_workspace_minsize,
	.region				= tui_workspace_region,
	.setproperty		= tui_workspace_setproperty,
	.paint				= tui_workspace_paint,
	.destroy			= tui_workspace_destroy,
};

struct tui_workspace * tui_workspace_new(struct console * console, const x_s8 * id)
{
	struct tui_workspace * workspace;
	x_s32 w, h;

	if(!id)
		return NULL;

	if(!console || !console->putcode)
		return NULL;

	if(!console_getwh(console, &w, &h))
		return NULL;

	workspace = malloc(sizeof(struct tui_workspace));
	if(!workspace)
		return NULL;

	workspace->widget.id = strdup(id);
	workspace->widget.x = 0;
	workspace->widget.y = 0;
	workspace->widget.w = w;
	workspace->widget.h = h;
	workspace->widget.console = console;
	workspace->widget.layout = NULL;
	workspace->widget.ops = &workspace_ops;
	workspace->widget.parent = NULL;
	workspace->widget.priv = workspace;

	init_list_head(&(workspace->widget.entry));
	init_list_head(&(workspace->widget.child));

	return workspace;
}
