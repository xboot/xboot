/*
 * kernel/command/cmd-write.c
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
#include <command/command.h>

#if	defined(CONFIG_COMMAND_WRITE) && (CONFIG_COMMAND_WRITE > 0)

static int do_write(int argc, char ** argv)
{
	int fd;
	ssize_t ret;

	if(argc != 3)
	{
		printf("usage:\r\n    write <string> <file>\r\n");
		return -1;
	}

	fd = open(argv[2], O_WRONLY | O_CREAT, (S_IRUSR|S_IWUSR));
	if(fd < 0)
		return -1;

	ret = write(fd, argv[1], strlen(argv[1]));
	close(fd);
	return ret < 0 ? -1 : 0;
}

static struct command_t write_cmd = {
	.name		= "write",
	.func		= do_write,
	.desc		= "write contents to a file\r\n",
	.usage		= "write <string> <file>\r\n",
	.help		= "    write contents to a file.\r\n"
};

static __init void write_cmd_init(void)
{
	if(command_register(&write_cmd))
		LOG("Register command 'write'");
	else
		LOG("Failed to register command 'write'");
}

static __exit void write_cmd_exit(void)
{
	if(command_unregister(&write_cmd))
		LOG("Unegister command 'write'");
	else
		LOG("Failed to unregister command 'write'");
}

command_initcall(write_cmd_init);
command_exitcall(write_cmd_exit);

#endif
