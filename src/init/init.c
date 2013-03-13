/*
 * init/init.c
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
#include <time/delay.h>
#include <time/xtime.h>
#include <xboot/menu.h>
#include <console/console.h>
#include <input/keyboard/keyboard.h>
#include <fb/fb.h>
#include <graphic/surface.h>
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

	if(mkdir("/romdisk", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG_E("failed to create directory '/romdisk'");

	if(mount("/dev/romdisk", "/romdisk" , "cpiofs", 0) != 0)
		LOG_E("failed to mount romdisk");

	if(mkdir("/etc", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG_E("failed to create directory '/etc'");

	if(mkdir("/tmp", S_IRWXU|S_IRWXG|S_IRWXO) != 0)
		LOG_E("failed to create directory '/tmp'");

	if(mkdir("/mnt", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG_E("failed to create directory '/mnt'");
}

/*
 * do load system configure
 */
void do_system_cfg(void)
{
	LOG_I("load system configure");

	/*
	 * load the setting of stdin, stdout and stderr console
	 */
	if(! console_stdio_load("/etc/console.xml"))
	{
		if(! console_stdio_load("/romdisk/etc/console.xml"))
			LOG_E("can not setting the standard console");
	}

	/*
	 * load environment variable
	 */
	if(! loadenv("/etc/environment.xml"))
	{
		if(! loadenv("/romdisk/etc/environment.xml"))
			LOG_E("can not load environment variable");
	}

	/*
	 * load menu context
	 */
	if(! menu_load("/etc/menu.xml"))
	{
		if(! menu_load("/romdisk/etc/menu.xml"))
			LOG_E("can not load menu context");
	}
}

/*
 * do load system fonts
 */
void do_system_fonts(void)
{
	char path[MAX_PATH];
	char buf[MAX_PATH];
	struct stat st;
	struct dirent * entry;
	void * dir;

	LOG_I("load system fonts");

	/*
	 * system fonts's directory path
	 */
	sprintf(path, "%s", "/romdisk/system/fonts");

	if(stat(path, &st) != 0)
		return;

	if(! S_ISDIR(st.st_mode))
		return;

	if( (dir = opendir(path)) == NULL)
    	return;

    for(;;)
    {
		if( (entry = readdir(dir)) == NULL)
		  break;

		buf[0] = 0;
		strlcpy(buf, path, sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';

		if(!strcmp(entry->d_name, "."))
		{
			continue;
		}
		else if(!strcmp(entry->d_name, ".."))
		{
			continue;
		}
		else
		{
			strlcat(buf, "/", sizeof(buf));
			strlcat(buf, entry->d_name, sizeof(buf));
		}

		if(stat(buf, &st) != 0)
			continue;

		/*
		 * FIXME
		 */
		// if(! install_font(buf))
		//	LOG_E("fail to install font: %s", buf);
    }

	closedir(dir);
}

/*
 * check battery capacity
 */
void do_system_battery(void)
{
	struct battery_info info;
	struct fb * fb;
	struct surface_t * obj[17];
	struct rect_t rect;
	char path[MAX_PATH];
	u32_t code;
	u32_t c;
	int index, oindex = -1;
	int i, count = 0;

	if(! machine_batinfo(&info))
		return;

	if(info.level >= 25)
		return;

	fb = get_default_framebuffer();
	if(!fb)
		return;

	for(i = 0; i < ARRAY_SIZE(obj); i++)
		obj[i] = NULL;

	surface_set_clip_rect(&(fb->info->surface), NULL);
	c = surface_map_color(&(fb->info->surface), get_named_color("black"));

	while(1)
	{
		machine_batinfo(&info);
		if (info.level < 0)
			info.level = 0;
		if (info.level > 100)
			info.level = 100;

		index = info.level * (ARRAY_SIZE(obj) - 1) / 100;
		if(index != oindex)
		{
			if(!obj[index])
			{
				sprintf(path, "/romdisk/system/media/image/battery/battery_%d.png", index);
				obj[index] = surface_load_from_file(path);
				if(!obj[index])
					break;
			}

			fb->swap(fb);
			rect_align(&(fb->info->surface.clip), &(obj[index]->clip), &rect, ALIGN_CENTER);
			rect_intersect(&(fb->info->surface.clip), &rect, &rect);
			surface_fill(&(fb->info->surface), &(fb->info->surface.clip), c, BLEND_MODE_REPLACE);
			surface_blit(&(fb->info->surface), &rect, obj[index], NULL, BLEND_MODE_ALPHA);
			fb->flush(fb);
		}
		oindex = index;

		if(console_stdin_getcode(&code))
		{
			if(code == KEY_POWER)
				break;
		}

		if(info.charging)
		{
			count = 0;

			if(info.level >= 100)
				break;
		}
		else
		{
			if(++count > 25)
			{
				machine_halt();
				break;
			}
		}

		mdelay(200);
	}

	for(i = 0; i < ARRAY_SIZE(obj); i++)
	{
		if(obj[i])
			surface_free(obj[i]);
	}
}

/*
 * wait a moment (two seconds)
 */
void do_system_wait(void)
{
	u32_t div, rem;
	u32_t timeout;

	if(get_system_hz() > 0)
	{
		div = jiffies / get_system_hz();
		rem = jiffies % get_system_hz();

		LOG_I("wait a moment, if necessary (%lu.%02lu)", div, rem);

		/*
		 * wait a moment for uptime until one seconds
		 */
		timeout = 0 + get_system_hz() * 1;
		while(time_before(jiffies, timeout));
	}
}
