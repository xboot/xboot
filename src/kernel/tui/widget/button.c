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

static struct tui_widget_ops button_ops = {
	.getid				= NULL,
	.setparent			= NULL,
	.getparent			= NULL,
	.addchild			= NULL,
	.removechild		= NULL,
	.iteratechild		= NULL,
	.setbounds			= NULL,
	.getbounds			= NULL,
	.minsize			= NULL,
	.setproperty		= NULL,
	.paint				= NULL,
	.destroy			= NULL,
};

struct tui_button * tui_button_new(struct tui_widget * parent, x_s8 * caption)
{
	struct tui_button * button;

	if(!parent || !parent->console)
		return NULL;

	button = malloc(sizeof(struct tui_button));
	if(! button)
		return NULL;

	button->widget.id = strdup((const x_s8 *)"");
	button->widget.x = 0;
	button->widget.y = 0;
	button->widget.w = 8;
	button->widget.h = 4;
	button->widget.curx = 0;
	button->widget.cury = 0;
	button->widget.console = parent->console;
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

	list_add_tail(&parent->entry, &button->widget.entry);

	return button;
}
