/*
 * kernel/command/cmd_sync.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jerryjianjun@gmail.com>
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
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <fs/fsapi.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_SYNC) && (CONFIG_COMMAND_SYNC > 0)

static s32_t do_sync(s32_t argc, const s8_t **argv)
{
	sync();

	return 0;
}

static struct command sync_cmd = {
	.name		= "sync",
	.func		= do_sync,
	.desc		= "flush file system buffers\r\n",
	.usage		= "sync\r\n",
	.help		= "    force changed blocks to disk, update the super block.\r\n"
};

static __init void sync_cmd_init(void)
{
	if(!command_register(&sync_cmd))
		LOG_E("register 'sync' command fail");
}

static __exit void sync_cmd_exit(void)
{
	if(!command_unregister(&sync_cmd))
		LOG_E("unregister 'sync' command fail");
}

module_init(sync_cmd_init, LEVEL_COMMAND);
module_exit(sync_cmd_exit, LEVEL_COMMAND);

#endif
