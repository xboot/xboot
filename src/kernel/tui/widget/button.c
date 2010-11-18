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
#include <tui/widget/button.h>


static x_bool tui_button_setbounds(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	//TODO
	return TRUE;
}

static x_bool tui_button_getbounds(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h)
{
	struct tui_button * button = widget->priv;

	if(x)
		*x = button->widget.x;
	if(y)
		*y = button->widget.y;
	if(w)
		*w = button->widget.w;
	if(h)
		*h = button->widget.h;

	return TRUE;
}

static x_bool tui_button_minsize(struct tui_widget * widget, x_s32 * w, x_s32 * h)
{
	struct tui_button * button = widget->priv;

	//TODO
	if(w)
		*w = button->widget.w;
	if(h)
		*h = button->widget.h;

	return TRUE;
}

static x_bool tui_button_region(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h)
{
	struct tui_button * button = widget->priv;

	//TODO
	if(x)
		*x = button->widget.x;
	if(y)
		*y = button->widget.y;
	if(w)
		*w = button->widget.w;
	if(h)
		*h = button->widget.h;

	return TRUE;
}

static x_bool tui_button_setproperty(struct tui_widget * widget, const x_s8 * name, const x_s8 * value)
{
	//TODO
	return FALSE;
}

static x_bool tui_button_paint(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	//TODO
/*
	struct tui_button * button = widget->priv;
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

	b.left = button->widget.x;
	b.top = button->widget.y;
	b.right = button->widget.x + button->widget.w;
	b.bottom = button->widget.y + button->widget.h;

	if(rect_intersect(&r, &a, & b) == FALSE)
		return TRUE;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	cursor = console_getcursor(button->widget.console);
	console_setcursor(button->widget.console, FALSE);

	console_setcolor(button->widget.console, theme->button.fg, theme->button.bg);
	code = theme->button.ch;
	for(j = y; j < h; j++)
	{
		console_gotoxy(button->widget.console, x, j);
		for(i = x; i < w; i++)
		{
			console_putcode(button->widget.console, code);
		}
	}

	for(pos = (&button->widget.child)->next; pos != (&button->widget.child); pos = pos->next)
	{
		list = list_entry(pos, struct tui_widget, entry);

		if(list->ops->paint)
			list->ops->paint(list, x, y, w, h);
	}

	console_setcursor(button->widget.console, cursor);
*/

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
	free(button->caption);
	free(button);

	return TRUE;
}

static struct tui_widget_ops button_ops = {
	.setbounds			= tui_button_setbounds,
	.getbounds			= tui_button_getbounds,
	.minsize			= tui_button_minsize,
	.region				= tui_button_region,
	.setproperty		= tui_button_setproperty,
	.paint				= tui_button_paint,
	.destory			= tui_button_destory,
};

struct tui_button * tui_button_new(struct tui_widget * parent, const x_s8 * id, const x_s8 * caption)
{
	struct tui_button * button;
	x_s32 x, y, w, h;

	if(!parent)
		return NULL;

	if(!id || !caption)
		return NULL;

	if(!tui_widget_region(parent, &x, &y, &w, &h))
		return NULL;

	button = malloc(sizeof(struct tui_button));
	if(!button)
		return NULL;

	button->widget.id = strdup(id);
	button->widget.x = x + 0;
	button->widget.y = y + 0;
	button->widget.w = 8;
	button->widget.h = 4;
	button->widget.console = parent->console;
	button->widget.layout = NULL;
	button->widget.ops = &button_ops;
	button->widget.parent = parent;
	button->widget.priv = button;

	button->caption = strdup(caption);
	button->cf = CONSOLE_WHITE;
	button->cb = CONSOLE_BLACK;
	button->align = ALIGN_CENTER;
	button->shadow = TRUE;
	button->enable = TRUE;
	button->visible = TRUE;

	init_list_head(&(button->widget.entry));
	init_list_head(&(button->widget.child));
	list_add_tail(&(parent->child), &(button->widget.entry));

	return button;
}
