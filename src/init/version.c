/*
 * init/version.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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
#include <xboot.h>
#include <vsprintf.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <terminal/curses.h>
#include <terminal/terminal.h>
#include <version.h>

/*
 * print xboot banner
 */
void xboot_banner(struct terminal * term)
{
	x_s8 buf[128];

	if(!term)
		return;

	sprintf(buf, (const x_s8 *)"xboot version: %d.%d%d ("__DATE__ " - " __TIME__ ") for " __MACH__"\r\n", XBOOT_MAJOY, XBOOT_MINIOR, XBOOT_PATCH);

	terminal_write(term, (x_u8 *)buf, strlen(buf));
}

/* xboot's character logo.
 *         _
 *        | |                  _        __0
 *   _  _ | |___ _____ _____ _| |_    _  /\_
 *  \ \/ /|  _  |  _  |  _  |_   _|    \/\
 *   )  ( | |_| | |_| | |_| | | |________/_____
 *  /_/\_\|_____|_____|_____| |________________|
 *
 */
static const char xboot[6][64] = {	"        _\r\n",
									"       | |                  _        __0\r\n",
									"  _  _ | |___ _____ _____ _| |_    _  /\\_\r\n",
									" \\ \\/ /|  _  |  _  |  _  |_   _|    \\/\\\r\n",
									"  )  ( | |_| | |_| | |_| | | |________/_____\r\n",
									" /_/\\_\\|_____|_____|_____| |________________|\r\n" };

/*
 * display xboot's character logo.
 */
void xboot_char_logo(struct terminal * term, x_u32 x0, x_u32 y0)
{
	x_u32 i, len;
	x_s32 width, height;
	x_s8 line_buf[64];

	if(!term || !term->getwh(term, &width, &height))
		return;

	if(x0 + 1 > width || y0 + 1 > height)
		return;

	for(i = 0; i < 6; i++)
	{
		len = strlen((x_s8*)&xboot[i][0]);
		if( len <= width-x0 )
			sprintf(line_buf, (x_s8*)("%s"), &xboot[i][0]);
		else
		{
			strncpy(line_buf, (x_s8*)&xboot[i][0], width-x0);
			line_buf[width-x0] = 0;
		}
		terminal_setxy(term, x0, y0 + i);
		terminal_write(term, (x_u8 *)line_buf, strlen(line_buf));
	}
}
