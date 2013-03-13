/*
 * kernel/command/cmd_umount.c
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <loop/loop.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>
#include <fs/fileio.h>


#if	defined(CONFIG_COMMAND_UMOUNT) && (CONFIG_COMMAND_UMOUNT > 0)

static int do_umount(int argc, char ** argv)
{
	if(argc != 2)
	{
		printk("usage:\r\n    umount <dir>\r\n");
		return -1;
	}

	if(umount((const char *)argv[1]) != 0)
	{
		printk("umount '%s' fail\r\n", argv[1]);
		return -1;
	}

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

command_initcall(umount_cmd_init);
command_exitcall(umount_cmd_exit);

#endif
