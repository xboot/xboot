/*
 * kernel/core/subsys.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

static char * trim(char * s)
{
	char * e;

	if(s)
	{
		while(isspace(*s))
			s++;
		if(*s == 0)
			return s;
		e = s + strlen(s) - 1;
		while((e > s) && isspace(*e))
			e--;
		*(e + 1) = 0;
	}
	return s;
}

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
	vfs_mount("romdisk.0", "/", "cpio", MOUNT_RDONLY);
	vfs_mount(NULL, "/sys", "sys", MOUNT_RDONLY);
	vfs_mount(NULL, "/tmp", "ram", MOUNT_RW);
	vfs_mount(NULL, "/storage", "ram", MOUNT_RW);
}

static void subsys_init_dtree(void)
{
	struct vfs_stat_t st;
	char path[VFS_MAX_PATH];
	char * json;
	int fd, n, len = 0;

	sprintf(path, "/boot/%s.json", get_machine()->name);
	if(vfs_stat(path, &st) < 0)
		return;
	if(!S_ISREG(st.st_mode))
		return;
	if(st.st_size <= 0)
		return;

	json = malloc(st.st_size + 1);
	if(!json)
		return;

	if((fd = vfs_open(path, O_RDONLY, 0)) >= 0)
	{
		for(;;)
		{
			n = vfs_read(fd, (void *)(json + len), SZ_64K);
			if(n <= 0)
				break;
			len += n;
		}
		vfs_close(fd);
		probe_device(json, len, path);
	}
	free(json);
}

static void subsys_init_private(void)
{
	char * mtab = strdup(CONFIG_MOUNT_PRIVATE_DEVICE);
	char * p, * r, * dev, * type;
	int mounted = 0;

	if(mtab)
	{
		p = mtab;
		while((r = strsep(&p, ",;|\r\n")) != NULL)
		{
			if(strchr(r, ':'))
			{
				dev = trim(strsep(&r, ":"));
				type = trim(r);
				dev = (dev && strcmp(dev, "") != 0) ? dev : NULL;
				type = (type && strcmp(type, "") != 0) ? type : NULL;
				if(dev && type)
				{
					if(vfs_mount(dev, "/private", type, MOUNT_RW) >= 0)
					{
						LOG("Mount /private using '%s' device with '%s' filesystem", dev, type);
						mounted = 1;
						break;
					}
				}
			}
		}
		free(mtab);
	}
	if(!mounted)
	{
		if(vfs_mount(NULL, "/private", "ram", MOUNT_RW) >= 0)
			LOG("mount /private with 'ram' filesystem");
	}
	vfs_mkdir("/private/application", 0755);
	vfs_mkdir("/private/userdata", 0755);
}

static __init void subsys_init(void)
{
	subsys_init_romdisk();
	subsys_init_rootfs();
	subsys_init_dtree();
	subsys_init_private();
}
subsys_initcall(subsys_init);
