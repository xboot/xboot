/*
 * kernel/command/cmd-cat.c
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

#if	defined(CONFIG_COMMAND_CAT) && (CONFIG_COMMAND_CAT > 0)

static int cat_one_file(const char * filename)
{
    struct stat st;
	char buf[SZ_4K];
    ssize_t i, n;
    int fd;

	if(stat(filename, &st) != 0)
	{
		printf("cat: %s: No such file or directory\r\n", filename);
		return -1;
	}

	if(S_ISDIR(st.st_mode))
	{
		printf("cat: %s: Is a directory\r\n", filename);
		return -1;
	}

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
	{
		printf("cat: %s: Can not open\r\n", filename);
		return -1;
	}

	while((n = read(fd, buf, sizeof(buf))) > 0)
	{
		for(i = 0; i < n; i++)
			putchar(buf[i]);
	}

	close(fd);
	return 0;
}

static int cat(int argc, char ** argv)
{
	int i;

	if(argc == 1)
	{
		printf("usage:\r\n    cat <file> ...\r\n");
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		if(cat_one_file(argv[i]) != 0)
			return -1;
	}

	printf("\r\n");
	return 0;
}

static struct command_t cat_cmd = {
	.name		= "cat",
	.func		= cat,
	.desc		= "show the contents of a file\r\n",
	.usage		= "cat <file> ...\r\n",
	.help		= "    show the contents of a file.\r\n"
};

static __init void cat_cmd_init(void)
{
	if(command_register(&cat_cmd))
		LOG("Register command 'cat'");
	else
		LOG("Failed to register command 'cat'");
}

static __exit void cat_cmd_exit(void)
{
	if(command_unregister(&cat_cmd))
		LOG("Unegister command 'cat'");
	else
		LOG("Failed to unregister command 'cat'");
}

command_initcall(cat_cmd_init);
command_exitcall(cat_cmd_exit);

#endif
