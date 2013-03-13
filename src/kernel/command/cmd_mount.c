/*
 * kernel/command/cmd_mount.c
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
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <loop/loop.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>
#include <fs/fileio.h>


#if	defined(CONFIG_COMMAND_MOUNT) && (CONFIG_COMMAND_MOUNT > 0)

static void usage(void)
{
	printk("usage:\r\n    mount <-t fstype> [-o option] <dev> <dir>\r\n");
}

static int do_mount(int argc, char ** argv)
{
	char * fstype = NULL;
	char * dev = NULL, * dir = NULL;
	bool_t loop_flag = FALSE;
	bool_t ro_flag = FALSE;
	s32_t mount_flag = 0;
	struct blkdev * blk;
	struct stat st;
	char * pdev = NULL, tmp[32];
	s32_t i, index = 0;

	if(argc < 5)
	{
		usage();
		return -1;
	}

	for(i=1; i<argc; i++)
	{
		if( !strcmp((const char *)argv[i], "-t") && (argc > i+1) )
		{
			fstype = (char *)argv[i+1];
			i++;
		}
		else if( !strcmp((const char *)argv[i], "-o") && (argc > i+1) )
		{
			if(!strcmp((const char *)argv[i+1], "loop"))
				loop_flag = TRUE;
			else if(!strcmp((const char *)argv[i+1], "ro"))
				ro_flag = TRUE;
			else if(!strcmp((const char *)argv[i+1], "rw"))
				ro_flag = FALSE;
			else
			{
				printk("unrecognized the option '%s'\r\n", argv[i+1]);
				return -1;
			}
			i++;
		}
		else
		{
			if(index == 0)
				dev = (char *)argv[i];
			else if(index == 1)
				dir = (char *)argv[i];
			else if(index >= 2)
			{
				usage();
				return -1;
			}
			index++;
		}
	}

	if(!fstype || !dev || !dir)
	{
		usage();
		return -1;
	}

	if(!filesystem_search(fstype))
	{
		printk("the filesystem %s not found\r\n", fstype);
		return -1;
	}

	if(stat(dir, &st) != 0)
	{
		printk("cannot access %s: no such directory\r\n", dir);
		return -1;
	}

	if(!S_ISDIR(st.st_mode))
	{
		printk("the '%s' does not a directory\r\n", dir);
		return -1;
	}

	if(loop_flag)
	{
		pdev = dev;
		if(stat(pdev, &st) != 0)
		{
			printk("cannot access %s: no such file\r\n", pdev);
			return -1;
		}

		if(!S_ISREG(st.st_mode))
		{
			printk("it's not a regulation file\r\n", pdev);
			return -1;
		}

		if(!register_loop(pdev))
		{
			printk("register a loop block device fail\r\n");
			return -1;
		}

		blk = search_loop(pdev);
		if(!blk)
		{
			printk("special loop block device not found\r\n");
			return -1;
		}

		snprintf(tmp, 32, "/dev/%s", blk->name);
		dev = tmp;
	}

	if(stat(dev, &st) != 0)
	{
		printk("special device %s does not exist\r\n", dev);
		return -1;
	}

	if(!S_ISBLK(st.st_mode))
	{
		printk("special device %s does not a block device\r\n", dev);
		return -1;
	}

	if(loop_flag)
		mount_flag |= MOUNT_LOOP;

	if(ro_flag)
		mount_flag |= MOUNT_RDONLY;

	if(mount(dev, dir , fstype, (mount_flag & MOUNT_MASK)) != 0)
	{
		if(loop_flag)
			unregister_loop(pdev);

		printk("mount '%s' filesystem on special device '%s' fail\r\n", fstype, dev);
		return -1;
	}

	return 0;
}

static struct command mount_cmd = {
	.name		= "mount",
	.func		= do_mount,
	.desc		= "mount a file system\r\n",
	.usage		= "mount <-t fstype> [-o option] <dev> <dir>\r\n",
	.help		= "    attach the file system found on device.\r\n"
				  "    -t    used to indicate the filesystem type\r\n"
				  "    -o    'ro' for mount a read only filesystem\r\n"
				  "    -o    'rw' for mount a read and write filesystem\r\n"
				  "    -o    'loop' for mount a loop device\r\n"
};

static __init void mount_cmd_init(void)
{
	if(!command_register(&mount_cmd))
		LOG_E("register 'mount' command fail");
}

static __exit void mount_cmd_exit(void)
{
	if(!command_unregister(&mount_cmd))
		LOG_E("unregister 'mount' command fail");
}

command_initcall(mount_cmd_init);
command_exitcall(mount_cmd_exit);

#endif
