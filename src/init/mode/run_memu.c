/*
 * init/mode/run_menu.c
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
#include <xboot/list.h>
#include <xboot/menu.h>
#include <console/console.h>
#include <mode/mode.h>

#if 0
struct cell
{
	x_u32 cp;
	enum tcolor fg, bg;
	x_bool dirty;
};

struct menu_ctx
{
	x_s32 width, height;

	x_s32 clen;
	struct cell * cell;



	struct menu_list * list;
	struct menu_item * item;

	x_u32 x0, y0, x1, y1;
};

static struct menu_ctx * menu_ctx_alloc(struct console * console)
{
	struct menu_ctx * ctx;
	x_s32 w, h;

	if(!console || !console->putcode)
		return NULL;

	if(!console_getwh(get_stdout(), &w, &h))
		return NULL;

	if(w <= 0 || h <= 0)
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

	ctx->item = get_menu_indexof_item(0);

	ctx->width = w;
	ctx->height = h;

	return ctx;
}

static void menu_ctx_free(struct menu_ctx * ctx)
{
	if(!ctx)
		return;
}
#endif

/*
 * running the menu mode
 */
void run_menu_mode(void)
{
	struct menu_ctx * ctx;

//	ctx = menu_ctx_alloc(get_stdout());
//	if(!ctx)
//		return;

	do {
		xboot_set_mode(MODE_SHELL);

	} while(xboot_get_mode() == MODE_MENU);

//	menu_ctx_free(ctx);
}
