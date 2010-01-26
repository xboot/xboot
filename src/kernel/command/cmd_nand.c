/*
 * kernel/command/cmd_nand.c
 *
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <terminal/terminal.h>
#include <terminal/curses.h>
#include <mtd/nand/nfc.h>
#include <mtd/nand/nand.h>
#include <shell/command.h>

#if	defined(CONFIG_COMMAND_NAND) && (CONFIG_COMMAND_NAND > 0)

extern struct nand_list * nand_list;

static void usage(void)
{
	printk("usage:\r\n    nand <device>\r\n");
}

static x_s32 nand(x_s32 argc, const x_s8 **argv)
{
	if(argc < 2)
	{
		return 0;
	}

	return 0;
}

static struct command nand_cmd = {
	.name		= "nand",
	.func		= nand,
	.desc		= "nand flash tool\r\n",
	.usage		= "nand\r\n",
	.help		= "    nand chip manager tool\r\n"
};

static __init void nand_cmd_init(void)
{
	if(!command_register(&nand_cmd))
		LOG_E("register 'nand' command fail");
}

static __exit void nand_cmd_exit(void)
{
	if(!command_unregister(&nand_cmd))
		LOG_E("unregister 'nand' command fail");
}

module_init(nand_cmd_init, LEVEL_COMMAND);
module_exit(nand_cmd_exit, LEVEL_COMMAND);

#endif
