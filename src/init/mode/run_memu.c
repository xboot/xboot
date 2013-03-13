/*
 * init/mode/run_menu.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
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

#include <xboot.h>
#include <xboot/list.h>
#include <xboot/menu.h>
#include <console/console.h>
#include <shell/exec.h>
#include <mode/mode.h>


struct menu_ctx
{
	struct menu_list * list;
	u32_t index;
	u32_t total;
	u32_t win1, win2;

	struct console * in;
	struct console * out;
	struct console * err;

	s32_t width, height;
	enum tcolor fg, bg;

	u32_t x0, y0, x1, y1;
	bool_t cursor;
};

static void menu_ctx_paint(struct menu_ctx * ctx)
{
	struct menu_list * list;
	struct list_head * pos;
	s32_t i;

	if(ctx->win2 - ctx->win1 != ctx->y1 - ctx->y0 - 1)
	{
		ctx->win1 = 0;
		ctx->win2 = ctx->y1 - ctx->y0 - 1;
	}

	if(ctx->index > ctx->win2)
	{
		ctx->win2 = ctx->index;
		ctx->win1 = ctx->index + 1 - (ctx->y1 - ctx->y0);
	}

	if(ctx->index < ctx->win1)
	{
		ctx->win1 = ctx->index;
		ctx->win2 = ctx->index + ctx->y1 - ctx->y0 - 1;
	}

	console_setcolor(ctx->out, ctx->fg, ctx->bg);
	console_gotoxy(ctx->out, ctx->x1 - 1, ctx->y0 + 1);
	if(ctx->win1 > 0)
		console_putcode(ctx->out, UNICODE_UP);
	else
		console_putcode(ctx->out, UNICODE_SPACE);

	console_gotoxy(ctx->out, ctx->x1 - 1, ctx->y1);
	if(ctx->win2 + 1 < ctx->total)
		console_putcode(ctx->out, UNICODE_DOWN);
	else
		console_putcode(ctx->out, UNICODE_SPACE);

	for(i = 0, pos = (&ctx->list->entry)->next; pos != (&ctx->list->entry); pos = pos->next, i++)
	{
		if( (i >= ctx->win1) && (i <= ctx->win2) )
		{
			list = list_entry(pos, struct menu_list, entry);

			if(i == ctx->index)
				console_setcolor(ctx->out, ctx->bg, ctx->fg);
			else
				console_setcolor(ctx->out, ctx->fg, ctx->bg);

			console_gotoxy(ctx->out, ctx->x0 + 2, ctx->y0 + i + 1 - ctx->win1);
			console_print(ctx->out, (const char *)"%s%*s", list->item->title, (ctx->x1 - ctx->x0) - utf8_width(list->item->title) - 3, " ");
		}
	}
}

static struct menu_ctx * menu_ctx_alloc(void)
{
	struct console * cin = get_console_stdin();
	struct console * cout = get_console_stdout();
	struct console * cerr = get_console_stderr();
	struct menu_ctx * ctx;
	s32_t w, h;

	if(!cin || !cout || !cerr)
		return NULL;

	if(!console_getwh(cout, &w, &h))
		return NULL;

	ctx = malloc(sizeof(struct menu_ctx));
	if(!ctx)
		return NULL;

	ctx->list = get_menu_list();
	if(!ctx->list)
	{
		free(ctx);
		return NULL;
	}

	ctx->index = 0;
	ctx->total = get_menu_total_items();
	if(ctx->total == 0)
		ctx->total = 1;

	ctx->win1 = 0;
	ctx->win2 = 0;

	ctx->cursor = console_getcursor(cout);
	if(!console_getcolor(cout, &ctx->fg, &ctx->bg))
	{
		free(ctx);
		return NULL;
	}

	console_setcolor(cout, ctx->fg, ctx->bg);
	console_setcursor(cout, FALSE);
	console_cls(cout);

	ctx->in = cin;
	ctx->out = cout;
	ctx->err = cerr;

	ctx->width = w;
	ctx->height = h;

	ctx->x0 = 1;
	ctx->y0 = 2;
	ctx->x1 = ctx->width - 2;
	if(ctx->width >= 44 && ctx->height > 12)
	{
		xboot_char_logo(ctx->out, (ctx->width - 44) / 2, ctx->height - 7);
		ctx->y1 = ctx->height - 10;
	}
	else
	{
		ctx->y1 = ctx->height - 2;
	}

	console_gotoxy(ctx->out, 1, 0);
	xboot_banner(ctx->out);

	console_rect(ctx->out,
					UNICODE_HLINE, UNICODE_VLINE,
					UNICODE_LEFTTOP, UNICODE_RIGHTTOP,
					UNICODE_LEFTBOTTOM, UNICODE_RIGHTBOTTOM,
					ctx->x0, ctx->y0, ctx->x1, ctx->y1);

	menu_ctx_paint(ctx);

	return ctx;
}

static void menu_ctx_free(struct menu_ctx * ctx)
{
	if(ctx)
	{
		console_setcolor(ctx->out, ctx->fg, ctx->bg);
		console_cls(ctx->out);
		console_setcursor(ctx->out, ctx->cursor);
		free(ctx);
	}
}

/*
 * running the menu mode
 */
void run_menu_mode(void)
{
	struct menu_item * item = NULL;
	struct menu_ctx * ctx;
	u32_t code;
	bool_t running = TRUE;

	ctx = menu_ctx_alloc();
	if(!ctx)
		return;

	do {
		if(ctx->in->getcode(ctx->in, &code))
		{
			switch(code)
			{
			case 0x2:	/* left */
				break;

			case 0x10:	/* up */
				ctx->index = (ctx->index + ctx->total - 1) % ctx->total;
				menu_ctx_paint(ctx);
				break;

			case 0x6:	/* right */
				break;

			case 0xe:	/* down */
				ctx->index = (ctx->index + ctx->total + 1) % ctx->total;
				menu_ctx_paint(ctx);
				break;

			case 0xa:	/* lf */
			case 0xd:	/* cr */
				item = get_menu_indexof_item(ctx->index);
				if(item && item->title && item->command)
					running = FALSE;
				break;

			default:
				break;
			}
		}
	} while((xboot_get_mode() == MODE_MENU) && (running == TRUE));

	menu_ctx_free(ctx);

	if(item && item->title && item->command)
		exec_cmdline(item->command);
}
