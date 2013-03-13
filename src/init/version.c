/*
 * init/version.c
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
#include <stdio.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <console/console.h>
#include <version.h>

/*
 * print xboot banner
 */
void xboot_banner(struct console * console)
{
	console_print(console, "xboot version: %ld.%ld%ld (%s - %s) for %s\r\n", XBOOT_MAJOY, XBOOT_MINIOR, XBOOT_PATCH, __DATE__, __TIME__, __MACH__);
}

/*
 * xboot's character logo.
 *         _
 *        | |                  _        __0
 *   _  _ | |___ _____ _____ _| |_    _  /\_
 *  \ \/ /|  _  |  _  |  _  |_   _|    \/\
 *   )  ( | |_| | |_| | |_| | | |________/_____
 *  /_/\_\|_____|_____|_____| |________________|
 *
 */
static const char xboot[6][64] = {	"        _",
									"       | |                  _        __0",
									"  _  _ | |___ _____ _____ _| |_    _  /\\_",
									" \\ \\/ /|  _  |  _  |  _  |_   _|    \\/\\",
									"  )  ( | |_| | |_| | |_| | | |________/_____",
									" /_/\\_\\|_____|_____|_____| |________________|" };

/*
 * print xboot's character logo.
 */
void xboot_char_logo(struct console * console, u32_t x0, u32_t y0)
{
	u32_t i, len;
	s32_t w, h;
	char buf[64];

	if(!console)
		return;

	if(!console_getwh(console, &w, &h))
		return;

	if(x0 + 1 > w || y0 + 1 > h)
		return;

	for(i = 0; i < 6; i++)
	{
		len = strlen(&xboot[i][0]);
		if( len <= w - x0 )
			sprintf(buf, "%s", &xboot[i][0]);
		else
		{
			strncpy(buf, &xboot[i][0], w - x0);
			buf[w - x0] = 0;
		}
		console_gotoxy(console, x0, y0 + i);
		console_print(console, (const char *)buf);
	}
}
