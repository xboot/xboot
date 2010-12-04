/*
 * kernel/tui/widget/widget.c
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
#include <charset.h>
#include <tui/tui.h>
#include <tui/theme.h>
#include <tui/widget/widget.h>


x_bool tui_widget_cell_putcode(struct tui_widget * widget, x_u32 cp, enum tcolor fg, enum tcolor bg, x_s32 x, x_s32 y)
{
	struct tui_cell * cell;

	if(!widget)
		return FALSE;

	cell = widget->cell;
	if(!cell)
		return FALSE;

	if(x < 0 || y < 0)
		return FALSE;

	if(x >= widget->width || y >= widget->height)
		return FALSE;

	cell = &(widget->cell[widget->width * y + x]);

	if( (cell->cp != cp) || (cell->fg != fg) || (cell->bg != bg) )
	{
		cell->cp = cp;
		cell->fg = fg;
		cell->bg = bg;
		cell->dirty = TRUE;
	}

	return TRUE;
}

x_bool tui_widget_cell_print(struct tui_widget * widget, x_s8 * str, enum tcolor fg, enum tcolor bg, x_s32 x, x_s32 y, x_s32 len)
{
	struct tui_cell * cell;
	const x_s8 * p;
	x_u32 code;

	if(!widget)
		return FALSE;

	cell = widget->cell;
	if(!cell)
		return FALSE;

	if(x < 0 || y < 0)
		return FALSE;

	if(len <= 0)
		return FALSE;

	if(x >= widget->width || y >= widget->height)
		return FALSE;

	cell = &(widget->cell[widget->width * y + x]);
	for(p = (const x_s8 *)str; utf8_to_ucs4(&code, 1, p, -1, &p) > 0; )
	{
		if(len-- <= 0)
			break;

		if( (cell->cp != code) || (cell->fg != fg) || (cell->bg != bg) )
		{
			cell->cp = code;
			cell->fg = fg;
			cell->bg = bg;
			cell->dirty = TRUE;
		}

		cell++;
	}

	return TRUE;
}

x_bool tui_widget_cell_hline(struct tui_widget * widget, x_u32 cp, enum tcolor fg, enum tcolor bg, x_s32 x0, x_s32 y0, x_s32 x)
{
	struct tui_cell * cell;
	x_s32 i;

	if(!widget)
		return FALSE;

	cell = widget->cell;
	if(!cell)
		return FALSE;

	if(x0 < 0 || y0 < 0 || x < 0)
		return FALSE;

	if(x0 >= widget->width || y0 >= widget->height)
		return FALSE;

	if(x0 + x >= widget->width)
		x = widget->width - x0;

	cell = &(widget->cell[widget->width * y0 + x0]);
	for(i = 0; i < x; i++)
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

	return TRUE;
}

x_bool tui_widget_cell_vline(struct tui_widget * widget, x_u32 cp, enum tcolor fg, enum tcolor bg, x_s32 x0, x_s32 y0, x_s32 y)
{
	struct tui_cell * cell;
	x_s32 i;

	if(!widget)
		return FALSE;

	cell = widget->cell;
	if(!cell)
		return FALSE;

	if(x0 < 0 || y0 < 0 || y < 0)
		return FALSE;

	if(x0 >= widget->width || y0 >= widget->height)
		return FALSE;

	if(y0 + y >= widget->height)
		y = widget->height - y0;

	cell = &(widget->cell[widget->width * y0 + x0]);
	for(i = 0; i < y; i++)
	{
		if( (cell->cp != cp) || (cell->fg != fg) || (cell->bg != bg) )
		{
			cell->cp = cp;
			cell->fg = fg;
			cell->bg = bg;
			cell->dirty = TRUE;
		}

		cell += widget->width;
	}

	return TRUE;
}

x_bool tui_widget_cell_rect(struct tui_widget * widget, x_u32 hline, x_u32 vline, x_u32 lt, x_u32 rt, x_u32 lb, x_u32 rb, enum tcolor fg, enum tcolor bg, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	struct tui_cell * cell;

	if(!widget)
		return FALSE;

	cell = widget->cell;
	if(!cell)
		return FALSE;

	if(x < 0 || y < 0 || w < 2 || h < 2)
		return FALSE;

	if(x >= widget->width || y >= widget->height)
		return FALSE;

	if(x + w - 1 >= widget->width)
		return FALSE;

	if(y + h - 1 >= widget->height)
		return FALSE;

	tui_widget_cell_putcode(widget, lt, fg, bg, x, y);
	tui_widget_cell_putcode(widget, rt, fg, bg, x + w - 1, y);
	tui_widget_cell_putcode(widget, lb, fg, bg, x, y + h - 1);
	tui_widget_cell_putcode(widget, rb, fg, bg, x + w - 1, y + h - 1);

	tui_widget_cell_hline(widget, hline, fg, bg, x + 1, y, w - 1 - 1);
	tui_widget_cell_hline(widget, hline, fg, bg, x + 1, y + h - 1, w - 1 - 1);
	tui_widget_cell_vline(widget, vline, fg, bg, x, y + 1, h - 1 - 1);
	tui_widget_cell_vline(widget, vline, fg, bg, x + w - 1, y + 1, h - 1 - 1);

	return TRUE;
}

x_bool tui_widget_cell_border(struct tui_widget * widget)
{
	struct tui_cell * cell;
	enum tcolor fg, bg;
	x_s32 w, h;
	x_s32 i;

	if(!widget)
		return FALSE;

	cell = widget->cell;
	if(!cell)
		return FALSE;

	w = widget->width;
	h = widget->height;
	if(w < 2 || h < 2)
		return FALSE;

	cell = &(widget->cell[w * 0 + 0]);
	for(i = 0; i < w - 1; i++)
	{
		fg = cell->fg;
		bg = cell->bg;

		cell->fg = bg;
		cell->bg = fg;
		cell->dirty = TRUE;

		cell++;
	}

	cell = &(widget->cell[w * 1 + 0]);
	for(i = 0; i < h - 1; i++)
	{
		fg = cell->fg;
		bg = cell->bg;

		cell->fg = bg;
		cell->bg = fg;
		cell->dirty = TRUE;

		cell += w;
	}

	cell = &(widget->cell[w * (h - 1) + 1]);
	for(i = 0; i < w - 1; i++)
	{
		fg = cell->fg;
		bg = cell->bg;

		cell->fg = bg;
		cell->bg = fg;
		cell->dirty = TRUE;

		cell++;
	}

	cell = &(widget->cell[w * 0 + (w - 1)]);
	for(i = 0; i < h - 1; i++)
	{
		fg = cell->fg;
		bg = cell->bg;

		cell->fg = bg;
		cell->bg = fg;
		cell->dirty = TRUE;

		cell += w;
	}

	return TRUE;
}

x_bool tui_widget_cell_clear(struct tui_widget * widget, x_u32 cp, enum tcolor fg, enum tcolor bg, x_s32 x, x_s32 y, x_s32 w, x_s32 h)
{
	struct tui_cell * cell;
	struct rect r, a, b;
	x_s32 i, j;

	if(!widget)
		return FALSE;

	cell = widget->cell;
	if(!cell)
		return FALSE;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = 0;
	b.top = 0;
	b.right =  widget->width;
	b.bottom = widget->height;

	if(rect_intersect(&r, &a, & b) == FALSE)
		return FALSE;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

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

	return TRUE;
}
