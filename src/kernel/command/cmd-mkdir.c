/*
 * kernel/command/cmd-mkdir.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
	printf("    mkdir [-p] DIRECTORY...\r\n\r\n");
}

static s32_t build(s8_t * path)
{
	struct stat st;
	s8_t * p;

	p = path;
	if(p[0] == '/')
		++p;

	for( ; ; ++p)
	{
		if(p[0] == '\0' || (p[0] == '/' && p[1] == '\0'))
			break;
		if(p[0] != '/')
			continue;
		*p = '\0';
		if( stat((const char *)path, &st) != 0 )
		{
			if(mkdir((const char *)path, (S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) != 0)
			{
				return -1;
			}
		}
		*p = '/';
	}
	return 0;
}

static int do_mkdir(int argc, char ** argv)
{
	s32_t c = 0;
	s8_t ** v;
	bool_t pflag = FALSE;
	s32_t ret = 0;
	s32_t i;

	if( (v = malloc(sizeof(s8_t *) * argc)) == NULL)
		return -1;

	for(i=1; i<argc; i++)
	{
		if( !strcmp((const char *)argv[i], "-p") )
			pflag = TRUE;
		else
			v[c++] = (s8_t *)argv[i];
	}

	if(c == 0)
	{
		printf("usage:\r\n    mkdir [-p] DIRECTORY...\r\n");
		free(v);
		return (-1);
	}

	for(i=0; i<c; i++)
	{
		if( pflag && build(v[i]) )
		{
			ret = -1;
			continue;
		}

		if(mkdir((const char*)v[i], (S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) != 0)
		{
			ret = -1;
			printf("mkdir: failed to create directory %s\r\n", v[i]);
		}
	}

	free(v);
	return ret;
}

static struct command_t cmd_mkdir = {
	.name	= "mkdir",
	.desc	= "make directories",
	.usage	= usage,
	.exec	= do_mkdir,
};

static __init void mkdir_cmd_init(void)
{
	register_command(&cmd_mkdir);
}

static __exit void mkdir_cmd_exit(void)
{
	unregister_command(&cmd_mkdir);
}

command_initcall(mkdir_cmd_init);
command_exitcall(mkdir_cmd_exit);
