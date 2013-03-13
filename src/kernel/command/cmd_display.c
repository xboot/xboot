/*
 * kernel/command/cmd_display.c
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
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>
#include <fs/fileio.h>
#include <fb/fb.h>

#if	defined(CONFIG_COMMAND_DISPLAY) && (CONFIG_COMMAND_DISPLAY > 0)

static int display(int argc, char ** argv)
{
	struct fb * fb;
	struct surface_t * surface;
	struct rect_t rect;
	struct stat st;
	u32_t c;

	if(argc != 3)
	{
		printk("usage:\r\n    display <dev> <file>\r\n");
		return -1;
	}

	fb = search_framebuffer((char *)argv[1]);
	if(!fb)
	{
		printk("display: '%s' does not a valid framebuffer\r\n", (char *)argv[1]);
		return -1;
	}

	if(stat((const char *)argv[2], &st) != 0)
	{
		printk("display: cannot access %s: No such file or directory\r\n", (char *)argv[2]);
		return -1;
	}

	if(S_ISDIR(st.st_mode))
	{
		printk("display: the file %s is a directory\r\n", (char *)argv[2]);
		return -1;
	}

	surface = surface_load_from_file((const char *)argv[2]);
	if(!surface)
	{
		printk("display: cannot load file %s\r\n", argv[2]);
		return -1;
	}

	surface_set_clip_rect(&(fb->info->surface), NULL);
	surface_set_clip_rect(surface, NULL);
	rect_align(&(fb->info->surface.clip), &(surface->clip), &rect, ALIGN_CENTER);

	if (!rect_intersect(&(fb->info->surface.clip), &rect, &rect))
	{
		surface_free(surface);
		return -1;
	}

	c = surface_map_color(&(fb->info->surface), get_named_color("black"));
	surface_fill(&(fb->info->surface), &(fb->info->surface.clip), c, BLEND_MODE_REPLACE);

	surface_blit(&(fb->info->surface), &rect, surface, &(surface->clip), BLEND_MODE_REPLACE);
	surface_free(surface);

	return 0;
}

static struct command display_cmd = {
	.name		= "display",
	.func		= display,
	.desc		= "display a picture on the screen\r\n",
	.usage		= "display <dev> <file>\r\n",
	.help		= "    show images on the screen\r\n"
};

static __init void display_cmd_init(void)
{
	if(!command_register(&display_cmd))
		LOG_E("register 'display' command fail");
}

static __exit void display_cmd_exit(void)
{
	if(!command_unregister(&display_cmd))
		LOG_E("unregister 'display' command fail");
}

command_initcall(display_cmd_init);
command_exitcall(display_cmd_exit);

#endif
