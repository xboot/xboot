/*
 * kernel/tui/theme.c
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
#include <tui/theme.h>


static struct tui_theme current_tui_theme =
{
	.workspace 	= {
		.cp		= UNICODE_SPACE,
		.fg		= TCOLOR_WHITE,
		.bg		= TCOLOR_BULE,
	},

	.button 	= {
		.cp		= UNICODE_SPACE,
		.fg		= TCOLOR_GREEN,
		.bg		= TCOLOR_GREEN,

		.c_fg	= TCOLOR_WHITE,
		.c_bg	= TCOLOR_BULE,

		.h		= UNICODE_HLINE,
		.v		= UNICODE_VLINE,
		.lt		= UNICODE_LEFTTOP,
		.rt		= UNICODE_RIGHTTOP,
		.lb		= UNICODE_LEFTBOTTOM,
		.rb		= UNICODE_RIGHTBOTTOM,
		.b_fg	= TCOLOR_WHITE,
		.b_bg	= TCOLOR_GREEN,
	},
};

struct tui_theme * get_tui_theme(void)
{
	return &current_tui_theme;
}
