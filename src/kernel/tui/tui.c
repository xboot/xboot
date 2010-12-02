/*
 * kernel/tui/tui.c
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
#include <tui/tui.h>


struct tui_widget * find_tui_widget_by_id(struct tui_widget * widget, const x_s8 * id)
{
	struct tui_widget * result;
	struct tui_widget * list;
	struct list_head * pos;

	if(!widget)
		return NULL;

	if(!id)
		return NULL;

	for(pos = (&widget->child)->next; pos != &widget->child; pos = pos->next)
	{
		list = list_entry(pos, struct tui_widget, entry);
		if(strcmp(list->id, id) == 0)
			return list;

		if(!list_empty(&list->child))
		{
			result = find_tui_widget_by_id(list, id);
			if(result != NULL)
				return result;
		}
	}

	return NULL;
}

x_bool tui_widget_setparent(struct tui_widget * widget, struct tui_widget * parent)
{
	struct tui_widget * list;
	struct list_head * pos;

	if(!widget)
		return FALSE;

	if(!parent)
		return FALSE;

	if((widget->parent != NULL) && (widget->parent != widget))
	{
		for(pos = (&widget->parent->child)->next; pos != (&widget->parent->child); pos = pos->next)
		{
			list = list_entry(pos, struct tui_widget, entry);
			if(list == widget)
			{
				list_del(pos);
				break;
			}
		}
	}

	list_add_tail(&(parent->child), &(widget->entry));
	widget->parent = parent;

	return TRUE;
}

struct tui_widget * tui_widget_getparent(struct tui_widget * widget)
{
	if(!widget)
		return NULL;

	if(widget->parent == widget)
		return NULL;

	return widget->parent;
}

x_bool tui_widget_addchild(struct tui_widget * widget, struct tui_widget * child)
{
	struct tui_widget * list;
	struct list_head * pos;

	if(!widget)
		return FALSE;

	if(!child)
		return FALSE;

	if((child->parent != NULL) && (child->parent != child))
	{
		for(pos = (&child->parent->child)->next; pos != (&child->parent->child); pos = pos->next)
		{
			list = list_entry(pos, struct tui_widget, entry);
			if(list == child)
			{
				list_del(pos);
				break;
			}
		}
	}

	list_add_tail(&(widget->child), &(child->entry));
	child->parent = widget;

	return TRUE;
}

x_bool tui_widget_removechild(struct tui_widget * widget, struct tui_widget * child)
{
	struct tui_widget * list;
	struct list_head * pos;

	if(!widget)
		return FALSE;

	if(!child)
		return FALSE;

	for(pos = (&widget->child)->next; pos != (&widget->child); pos = pos->next)
	{
		list = list_entry(pos, struct tui_widget, entry);
		if(list == child)
		{
			list_del(pos);
			return TRUE;
		}
	}

	return FALSE;
}

x_bool tui_widget_minsize(struct tui_widget * widget, x_s32 * width, x_s32 * height)
{
	if(!widget)
		return FALSE;

	return widget->ops->minsize(widget, width, height);
}

x_bool tui_widget_region(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h)
{
	if(!widget)
		return FALSE;

	return widget->ops->region(widget, x, y, w, h);
}

x_bool tui_widget_setbounds(struct tui_widget * widget, x_s32 ox, x_s32 oy, x_s32 width, x_s32 height)
{
	if(!widget)
		return FALSE;

	return widget->ops->setbounds(widget, ox, oy, width, height);
}

x_bool tui_widget_getbounds(struct tui_widget * widget, x_s32 * ox, x_s32 * oy, x_s32 * width, x_s32 * height)
{
	if(!widget)
		return FALSE;

	return widget->ops->getbounds(widget, ox, oy, width, height);
}

x_bool tui_widget_setproperty(struct tui_widget * widget, const x_s8 * name, const x_s8 * value)
{
	if(!widget)
		return FALSE;

	return widget->ops->setproperty(widget, name, value);
}

x_bool tui_widget_paint(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	if(!widget)
		return FALSE;

	return widget->ops->paint(widget, x, y, w, h);
}

x_bool tui_widget_destroy(struct tui_widget * widget)
{
	if(!widget)
		return FALSE;

	return widget->ops->destroy(widget);
}
