/*
 * init/init.c
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
#include <string.h>
#include <malloc.h>
#include <vsprintf.h>
#include <xml.h>
#include <time/tick.h>
#include <time/timer.h>
#include <time/delay.h>
#include <time/xtime.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <terminal/terminal.h>
#include <shell/env.h>
#include <fs/fsapi.h>
#include <init.h>


/*
 * mount root filesystem and create some directory.
 */
void do_system_rootfs(void)
{
	LOG_I("mount root filesystem");

	if(mount(NULL, "/" , "ramfs", 0) != 0)
		LOG_E("failed to mount root filesystem");

	if(chdir("/") != 0)
		LOG_E("can't change directory to '/'");

	if(mkdir("/proc", S_IRUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG_E("failed to create directory '/proc'");

	if(mount(NULL, "/proc" , "procfs", 0) != 0)
		LOG_E("failed to mount proc filesystem");

	if(mkdir("/dev", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG_E("failed to create directory '/dev'");

	if(mount(NULL, "/dev" , "devfs", 0) != 0)
		LOG_E("failed to mount dev filesystem");

	if(mkdir("/boot", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG_E("failed to create directory '/boot'");

	if(mount("/dev/ramdisk", "/boot" , "cpiofs", 0) != 0)
		LOG_E("failed to mount ramdisk");

	if(mkdir("/etc", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG_E("failed to create directory '/etc'");

	if(mkdir("/mnt", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG_E("failed to create directory '/mnt'");

	if(mkdir("/tmp", S_IRWXU|S_IRWXG|S_IRWXO) != 0)
		LOG_E("failed to create directory '/tmp'");
}

/*
 * do load system configure
 */
void do_system_cfg(void)
{
	struct machine * mach = get_machine();
	struct stdin * stdin;
	struct stdout * stdout;

	LOG_I("load system configure");

	/* load environment variable file /etc/env.xml */
	env_load("/etc/env.xml");

	/* add stdin and stdout terminal */
	if(mach)
	{
		if(mach->cfg.stdin)
		{
			stdin = mach->cfg.stdin;
			while(stdin->name)
			{
				add_terminal_stdin(stdin->name);
				stdin++;
			}
		}

		if(mach->cfg.stdout)
		{
			stdout = mach->cfg.stdout;
			while(stdout->name)
			{
				add_terminal_stdout(stdout->name);
				stdout++;
			}
		}
	}
}

/*
 * wait a moment (two seconds)
 */
void do_system_wait(void)
{
	LOG_I("wait a moment, if necessary");

	if(get_system_hz() > 0)
	{
		while(jiffies < get_system_hz() * 2);
	}
}
