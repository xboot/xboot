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
#include <tui/widget/workspace.h>

static const x_s8 * tui_workspace_getid(struct tui_widget * self)
{
	return self->id;
}

static x_bool tui_workspace_setparent(struct tui_widget * self, struct tui_widget * parent)
{
	self->parent = NULL;

	return FALSE;
}

static struct tui_widget * tui_workspace_getparent(struct tui_widget * self)
{
	return self->parent;
}

static x_bool tui_workspace_addchild(struct tui_widget * self, struct tui_widget * child)
{
	struct tui_widget * list;
	struct list_head * pos;

	if(!child)
		return FALSE;

	if(child->parent)
	{
		for(pos = (&child->parent->child)->next; pos != (&child->parent->child); pos = pos->next)
		{
			list = list_entry(pos, struct tui_widget, child);
			if(list == child)
			{
				list_del(pos);
				break;
			}
		}
	}

	list_add_tail(&(self->child), &(child->entry));
	child->parent = self;

	return TRUE;
}

static x_bool tui_workspace_removechild(struct tui_widget * self, struct tui_widget * child)
{
	struct tui_widget * list;
	struct list_head * pos;

	if(!child)
		return FALSE;

	for(pos = (&self->child)->next; pos != (&self->child); pos = pos->next)
	{
		list = list_entry(pos, struct tui_widget, child);
		if(list == child)
		{
			list_del(pos);
			return TRUE;
		}
	}

	return FALSE;
}

static x_bool tui_workspace_setbounds(struct tui_widget * self, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	return TRUE;
}

static x_bool tui_workspace_getbounds(struct tui_widget * self, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h)
{
	if(x)
		*x = self->x;
	if(y)
		*y = self->y;
	if(w)
		*w = self->w;
	if(h)
		*h = self->h;

	return TRUE;
}

static x_bool tui_workspace_minsize(struct tui_widget * self, x_s32 * w, x_s32 * h)
{
	if(w)
		*w = self->w;
	if(h)
		*h = self->h;

	return TRUE;
}

static x_bool tui_workspace_setproperty(struct tui_widget * self, const x_s8 * name, const x_s8 * value)
{
	if(strcmp(name, (const x_s8 *)"id") == 0)
	{
		free(self->id);
		if(value)
			self->id = strdup(value);
		else
			self->id = strdup((const x_s8 *)"");
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

static x_bool tui_workspace_paint(struct tui_widget * self, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	struct tui_widget * list;
	struct list_head * pos;
	struct rect r, a, b;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = self->x;
	b.top = self->y;
	b.right = self->x + self->w;
	b.bottom = self->y + self->h;

	if(rect_intersect(&r, &a, & b) == FALSE)
		return TRUE;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	for(pos = (&self->child)->next; pos != (&self->child); pos = pos->next)
	{
		list = list_entry(pos, struct tui_widget, child);

		if(list->ops->paint)
			list->ops->paint(list, x, y, w, h);
	}

	return TRUE;
}

static x_bool tui_workspace_destroy(struct tui_widget * self)
{
	struct tui_workspace * workspace = self->priv;
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
	.getid				= tui_workspace_getid,
	.setparent			= tui_workspace_setparent,
	.getparent			= tui_workspace_getparent,
	.addchild			= tui_workspace_addchild,
	.removechild		= tui_workspace_removechild,
	.setbounds			= tui_workspace_setbounds,
	.getbounds			= tui_workspace_getbounds,
	.minsize			= tui_workspace_minsize,
	.setproperty		= tui_workspace_setproperty,
	.paint				= tui_workspace_paint,
	.destroy			= tui_workspace_destroy,
};

struct tui_workspace * new_tui_workspace(struct console * console)
{
	struct tui_workspace * workspace;
	enum console_color fg, bg;
	x_s32 w, h;

	if(!console || !console->putcode)
		return NULL;

	if(!console_getwh(console, &w, &h))
		return NULL;

	if(!console_getcolor(console, &fg, &bg))
		return NULL;

	workspace = malloc(sizeof(struct tui_workspace));
	if(!workspace)
		return NULL;

	workspace->widget.console = console;
	workspace->widget.id = strdup((const x_s8 *)"workspace");
	workspace->widget.x = 0;
	workspace->widget.y = 0;
	workspace->widget.w = w;
	workspace->widget.h = h;
	workspace->widget.layout = NULL;
	workspace->widget.ops = &workspace_ops;
	workspace->widget.parent = NULL;
	workspace->widget.priv = workspace;

	workspace->fg = fg;
	workspace->bg = bg;

	init_list_head(&(workspace->widget.entry));
	init_list_head(&(workspace->widget.child));

	return workspace;
}
