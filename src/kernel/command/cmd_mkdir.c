/*
 * kernel/command/cmd_mkdir.c
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
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>
#include <fs/fsapi.h>


#if	defined(CONFIG_COMMAND_MKDIR) && (CONFIG_COMMAND_MKDIR > 0)

static x_s32 build(x_s8 * path)
{
	struct stat st;
	x_s8 * p;

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

static x_s32 do_mkdir(x_s32 argc, const x_s8 **argv)
{
	x_s32 c = 0;
	x_s8 ** v;
	x_bool pflag = FALSE;
	x_s32 ret = 0;
	x_s32 i;

	if( (v = malloc(sizeof(x_s8 *) * argc)) == NULL)
		return -1;

	for(i=1; i<argc; i++)
	{
		if( !strcmp(argv[i],(x_s8*)"-p") )
			pflag = TRUE;
		else
			v[c++] = (x_s8 *)argv[i];
	}

	if(c == 0)
	{
		printk("usage:\r\n    mkdir [-p] DIRECTORY...\r\n");
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
			printk("mkdir: failed to create directory %s\r\n", v[i]);
		}
	}

	free(v);
	return ret;
}

static struct command mkdir_cmd = {
	.name		= "mkdir",
	.func		= do_mkdir,
	.desc		= "make directories\r\n",
	.usage		= "mkdir [-p] DIRECTORY...\r\n",
	.help		= "    create the DIRECTORY, if they do not already exist.\r\n"
};

static __init void mkdir_cmd_init(void)
{
	if(!command_register(&mkdir_cmd))
		LOG_E("register 'mkdir' command fail");
}

static __exit void mkdir_cmd_exit(void)
{
	if(!command_unregister(&mkdir_cmd))
		LOG_E("unregister 'mkdir' command fail");
}

module_init(mkdir_cmd_init, LEVEL_COMMAND);
module_exit(mkdir_cmd_exit, LEVEL_COMMAND);

#endif
