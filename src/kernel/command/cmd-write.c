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

static int write_file(const char * filename, const char * value)
{
	int fd;
	size_t len;
	ssize_t ret;

	fd = open(filename, O_WRONLY | O_CREAT, 0600);
	if(fd < 0)
		return -1;

	len = strlen(value);
	do {
		ret = write(fd, value, len);
	} while(ret < 0);

	close(fd);
	return ret;
}

static int do_write(int argc, char ** argv)
{
	int i;

	if(argc != 3)
	{
		printf("usage:\r\n    write <string> <file>\r\n");
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		if(write_one_file(argv[i]) != 0)
			return -1;
	}
	return 0;
}

static struct command_t write_cmd = {
	.name		= "write",
	.func		= do_write,
	.desc		= "write contents to a file\r\n",
	.usage		= "write <file> ...\r\n",
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
