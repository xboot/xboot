/*
 * kernel/command/cmd_umount.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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
#include <loop/loop.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <shell/command.h>
#include <fs/fsapi.h>


#if	defined(CONFIG_COMMAND_MOUNT) && (CONFIG_COMMAND_MOUNT > 0)

static x_s32 do_umount(x_s32 argc, const x_s8 **argv)
{
	return 0;
}

static struct command umount_cmd = {
	.name		= "umount",
	.func		= do_umount,
	.desc		= "unmount a file system\r\n",
	.usage		= "umount <dir>\r\n",
	.help		= "    detache the file system.\r\n"
};

static __init void umount_cmd_init(void)
{
	if(!command_register(&umount_cmd))
		LOG_E("register 'umount' command fail");
}

static __exit void umount_cmd_exit(void)
{
	if(!command_unregister(&umount_cmd))
		LOG_E("unregister 'umount' command fail");
}

module_init(umount_cmd_init, LEVEL_COMMAND);
module_exit(umount_cmd_exit, LEVEL_COMMAND);

#endif
