/*
 * kernel/command/cmd-umount.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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
#include <block/loop.h>
#include <xboot/list.h>
#include <xboot/initcall.h>
#include <command/command.h>
#include <fs/fileio.h>


#if	defined(CONFIG_COMMAND_UMOUNT) && (CONFIG_COMMAND_UMOUNT > 0)

static int do_umount(int argc, char ** argv)
{
	if(argc != 2)
	{
		printf("usage:\r\n    umount <dir>\r\n");
		return -1;
	}

	if(umount((const char *)argv[1]) != 0)
	{
		printf("umount '%s' fail\r\n", argv[1]);
		return -1;
	}

	return 0;
}

static struct command_t umount_cmd = {
	.name		= "umount",
	.func		= do_umount,
	.desc		= "unmount a file system\r\n",
	.usage		= "umount <dir>\r\n",
	.help		= "    detache the file system.\r\n"
};

static __init void umount_cmd_init(void)
{
	if(command_register(&umount_cmd))
		LOG("Register command 'umount'");
	else
		LOG("Failed to register command 'umount'");
}

static __exit void umount_cmd_exit(void)
{
	if(command_unregister(&umount_cmd))
		LOG("Unegister command 'umount'");
	else
		LOG("Failed to unregister command 'umount'");
}
command_initcall(umount_cmd_init);
command_exitcall(umount_cmd_exit);

#endif
