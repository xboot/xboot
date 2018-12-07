/*
 * kernel/core/subsys.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
		"{\"romdisk@0\":{\"address\":\"%ld\",\"size\":\"%ld\"}}",
		(unsigned long)(&__romdisk_start),
		(unsigned long)(&__romdisk_end - &__romdisk_start));
	probe_device(json, length, NULL);
}

static void subsys_init_rootfs(void)
{
	mount("romdisk.0", "/", "cpiofs", 0); chdir("/");
	mount(NULL, "/sys", "sysfs", 0);
	mount(NULL, "/storage" , "ramfs", 0);
	mount(NULL, "/private" , "ramfs", 0);
	mkdir("/private/application", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	mkdir("/private/userdata", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
}

static void subsys_init_dt(void)
{
	char path[64];
	char * json;
	int fd, n, len = 0;

	json = malloc(SZ_1M);
	if(!json)
		return;

	sprintf(path, "/boot/%s.json", get_machine()->name);
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
		probe_device(json, len, path);
	}
	free(json);
}

static __init void subsys_init(void)
{
	subsys_init_romdisk();
	subsys_init_rootfs();
	subsys_init_dt();
}
subsys_initcall(subsys_init);
