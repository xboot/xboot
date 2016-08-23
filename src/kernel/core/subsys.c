/*
 * kernel/core/subsys.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

extern unsigned char __romdisk_start;
extern unsigned char __romdisk_end;

static void subsys_init_romdisk(void)
{
	char json[256];
	int length;

	length = sprintf(json,
		"{\"romdisk@0\":{\"address\":\"%lld\",\"size\":\"%lld\"}}",
		(unsigned long long)(&__romdisk_start),
		(unsigned long long)(&__romdisk_end - &__romdisk_start));
	probe_device(json, length);
}

static void subsys_init_rootfs(void)
{
	mount(NULL, "/" , "ramfs", 0);

	chdir("/");
	mkdir("/sys", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	mkdir("/mnt", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	mkdir("/app", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	mkdir("/romdisk", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

	mount(NULL, "/sys", "sysfs", 0);
	mount("romdisk.0", "/romdisk", "cpiofs", 0);
}

static void subsys_init_dt(void)
{
	char path[64];
	char * json;
	int fd, n, len = 0;

	json = malloc(SZ_1M);
	if(!json)
		return;

	sprintf(path, "/romdisk/%s.json", get_machine()->name);
	if((fd = open(path, O_RDONLY, (S_IRUSR | S_IRGRP | S_IROTH))) > 0)
	{
	    for(;;)
	    {
	        n = read(fd, (void *)(json + len), SZ_512K);
	        if(n <= 0)
	        	break;
			len += n;
	    }
	    close(fd);
	    probe_device(json, len);
	}

	free(json);
}

static __init void subsys_init(void)
{
	subsys_init_romdisk();
	subsys_init_rootfs();

	subsys_init_timer();
	subsys_init_keeper();
	subsys_init_dt();
}
subsys_initcall(subsys_init);
