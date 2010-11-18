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
	if(x)
		*x = widget->x;
	if(y)
		*y = widget->y;
	if(w)
		*w = widget->w;
	if(h)
		*h = widget->h;

	return TRUE;
}

static x_bool tui_button_minsize(struct tui_widget * widget, x_s32 * w, x_s32 * h)
{
	struct tui_button * button = widget->priv;

	if(button->visible)
	{
		if(button->shadow)
		{
			if(w)
				*w = 3 + 1;
			if(h)
				*h = 3 + 1;
		}
		else
		{
			if(w)
				*w = 3;
			if(h)
				*h = 3;
		}
	}
	else
	{
		if(w)
			*w = 0;
		if(h)
			*h = 0;
	}

	return TRUE;
}

static x_bool tui_button_setproperty(struct tui_widget * widget, const x_s8 * name, const x_s8 * value)
{
	struct tui_button * button = widget->priv;

	if(strcmp(name, (const x_s8 *)"id") == 0)
	{
		free(button->widget.id);
		if(value)
			button->widget.id = strdup(value);
		else
			button->widget.id = strdup((const x_s8 *)"");
	}
	else if(strcmp(name, (const x_s8 *)"caption") == 0)
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
	struct tui_widget * list;
	struct list_head * pos;
	struct rect r, a, b;

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
	//TODO

	return TRUE;
}

static x_bool tui_button_destroy(struct tui_widget * widget)
{
	struct tui_button * button = widget->priv;

	list_del(&button->widget.entry);

	free(button->caption);
	free(button->widget.id);
	free(button);

	return TRUE;
}

static struct tui_widget_ops button_ops = {
	.setbounds			= tui_button_setbounds,
	.getbounds			= tui_button_getbounds,
	.minsize			= tui_button_minsize,
	.setproperty		= tui_button_setproperty,
	.paint				= tui_button_paint,
	.destroy			= tui_button_destroy,
};

struct tui_button * new_tui_button(struct tui_widget * parent, x_s8 * caption)
{
	struct tui_button * button;

	if(!parent || !parent->console)
		return NULL;

	button = malloc(sizeof(struct tui_button));
	if(! button)
		return NULL;

	button->widget.console = parent->console;
	button->widget.id = strdup((const x_s8 *)"");
	button->widget.x = 0;
	button->widget.y = 0;
	button->widget.w = 8;
	button->widget.h = 4;
	button->widget.layout = NULL;
	button->widget.ops = &button_ops;
	button->widget.parent = parent;
	button->widget.priv = button;

	button->caption = strdup(caption);
	button->cf = CONSOLE_WHITE;
	button->cb = CONSOLE_BLACK;
	button->bf = CONSOLE_BULE;
	button->bf = CONSOLE_GREEN;
	button->sb = CONSOLE_CYAN;
	button->sb = CONSOLE_MAGENTA;
	button->align = ALIGN_CENTER;
	button->shadow = TRUE;
	button->enable = TRUE;
	button->visible = TRUE;

	init_list_head(&(button->widget.entry));
	init_list_head(&(button->widget.child));
	list_add_tail(&(parent->child), &(button->widget.entry));

	return button;
}
