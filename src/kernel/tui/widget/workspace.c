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
	struct tui_theme * theme = get_tui_theme();
	struct tui_widget * list;
	struct list_head * pos;
	struct rect r, a, b;
	x_bool cursor;
	x_u32 code;
	x_s32 i, j;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = workspace->widget.x;
	b.top = workspace->widget.y;
	b.right = workspace->widget.x + workspace->widget.w;
	b.bottom = workspace->widget.y + workspace->widget.h;

	if(rect_intersect(&r, &a, & b) == FALSE)
		return TRUE;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	cursor = console_getcursor(workspace->widget.console);
	console_setcursor(workspace->widget.console, FALSE);

	console_setcolor(workspace->widget.console, theme->workspace.fg, theme->workspace.bg);
	code = theme->workspace.ch;
	for(j = y; j < h; j++)
	{
		console_gotoxy(workspace->widget.console, x, j);
		for(i = x; i < w; i++)
		{
			console_putcode(workspace->widget.console, code);
		}
	}

	for(pos = (&workspace->widget.child)->next; pos != (&workspace->widget.child); pos = pos->next)
	{
		list = list_entry(pos, struct tui_widget, entry);

		if(list->ops->paint)
			list->ops->paint(list, x, y, w, h);
	}

	console_setcursor(workspace->widget.console, cursor);

	return TRUE;
}

static x_bool tui_workspace_destory(struct tui_widget * widget)
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
		if(list->ops->destory)
			list->ops->destory(list);
		curr = next;
	}

	console_setcolor(workspace->widget.console, workspace->f, workspace->b);
	console_gotoxy(workspace->widget.console, workspace->x, workspace->y);
	console_setcursor(workspace->widget.console, workspace->cursor);

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
	.destory			= tui_workspace_destory,
};

struct tui_workspace * tui_workspace_new(struct console * console, const x_s8 * id)
{
	struct tui_workspace * workspace;
	enum console_color f, b;
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
	workspace->widget.x = 0;
	workspace->widget.y = 0;
	workspace->widget.w = w;
	workspace->widget.h = h;
	workspace->widget.console = console;
	workspace->widget.layout = NULL;
	workspace->widget.ops = &workspace_ops;
	workspace->widget.parent = (struct tui_widget *)workspace;
	workspace->widget.priv = workspace;

	workspace->cursor = console_getcursor(console);
	workspace->x = x;
	workspace->y = y;
	workspace->f = f;
	workspace->b = b;

	init_list_head(&(workspace->widget.entry));
	init_list_head(&(workspace->widget.child));

	return workspace;
}
