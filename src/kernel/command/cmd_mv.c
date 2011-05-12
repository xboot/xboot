/*
 * kernel/command/cmd_mv.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
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
#include <command/command.h>
#include <fs/fsapi.h>


#if	defined(CONFIG_COMMAND_MV) && (CONFIG_COMMAND_MV > 0)

static x_s32 do_mv(x_s32 argc, const x_s8 **argv)
{
	char path[MAX_PATH];
	char * src, * dest, * p;
	struct stat st1, st2;
	x_s32 rc;

	src = (char *)argv[1];
	dest = (char *)argv[2];

	if(argc != 3)
	{
		printk("usage:\r\n    mv SOURCE DEST\r\n");
		return -1;
	}

	/* check if source exists and it's regular file. */
	if(stat((const char *)src, &st1) != 0)
	{
		printk("mv: cannot access %s: No such file or directory\r\n", src);
		return -1;
	}

	if(!S_ISREG(st1.st_mode))
	{
		printk("mv: invalid file type\r\n");
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
    	printk("mv: failed to move file %s to %s\r\n", src, dest);
    	return -1;
    }

	return 0;
}

static struct command mv_cmd = {
	.name		= "mv",
	.func		= do_mv,
	.desc		= "move (rename) files\r\n",
	.usage		= "mv SOURCE DEST\r\n",
	.help		= "    Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\r\n"
};

static __init void mv_cmd_init(void)
{
	if(!command_register(&mv_cmd))
		LOG_E("register 'mv' command fail");
}

static __exit void mv_cmd_exit(void)
{
	if(!command_unregister(&mv_cmd))
		LOG_E("unregister 'mv' command fail");
}

module_init(mv_cmd_init, LEVEL_COMMAND);
module_exit(mv_cmd_exit, LEVEL_COMMAND);

#endif
