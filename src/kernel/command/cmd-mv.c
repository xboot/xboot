/*
 * kernel/command/cmd-mv.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    mv SOURCE DEST\r\n");
}

static int do_mv(int argc, char ** argv)
{
	char path[MAX_PATH];
	char * src, * dest, * p;
	struct stat st1, st2;
	s32_t rc;

	src = (char *)argv[1];
	dest = (char *)argv[2];

	if(argc != 3)
	{
		usage();
		return -1;
	}

	/* check if source exists and it's regular file. */
	if(stat((const char *)src, &st1) != 0)
	{
		printf("mv: cannot access %s: No such file or directory\r\n", src);
		return -1;
	}

	if(!S_ISREG(st1.st_mode))
	{
		printf("mv: invalid file type\r\n");
		return -1;
	}

	/* check if target is a directory. */
	rc = stat((const char *)dest, &st2);
	if(!rc && S_ISDIR(st2.st_mode))
	{
		p = strrchr(src, '/');
		p = p ? p + 1 : src;
		strlcpy(path, dest, sizeof(path));
		if(strcmp(dest, "/"))
			strlcat(path, "/", sizeof(path));
		strlcat(path, p, sizeof(path));
		dest = path;
	}

    if(rename((char *)src, (char *)dest) != 0)
    {
    	printf("mv: failed to move file %s to %s\r\n", src, dest);
    	return -1;
    }

	return 0;
}

static struct command_t cmd_mv = {
	.name	= "mv",
	.desc	= "move (rename) files",
	.usage	= usage,
	.exec	= do_mv,
};

static __init void mv_cmd_init(void)
{
	register_command(&cmd_mv);
}

static __exit void mv_cmd_exit(void)
{
	unregister_command(&cmd_mv);
}

command_initcall(mv_cmd_init);
command_exitcall(mv_cmd_exit);
