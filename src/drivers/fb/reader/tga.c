/*
 * drivers/fb/reader/tga.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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
#include <malloc.h>
#include <vsprintf.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <fs/fsapi.h>
#include <fb/bitmap.h>

struct tga_header
{
	x_u8 id_length;
	x_u8 color_map_type;
	x_u8 image_type;

	/* color map specification */
	x_u16 color_map_first_index;
	x_u16 color_map_length;
	x_u8 color_map_bpp;

	/* image specification */
	x_u16 image_x_origin;
	x_u16 image_y_origin;
	x_u16 image_width;
	x_u16 image_height;
	x_u8 image_bpp;
	x_u8 image_descriptor;

} __attribute__ ((packed));

static x_bool tga_load(struct bitmap ** bitmap, const char * filename)
{
	struct stat st;
	x_s32 fd;

	if(stat(filename, &st) != 0)
		return FALSE;

	if(S_ISDIR(st.st_mode))
		return FALSE;

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return FALSE;
/*
	if( read(fd, (void *)buf, 128)

	  if (grub_file_read (file, &header, sizeof (header))
	      != sizeof (header))
	    {
	      grub_file_close (file);
	      return grub_errno;
	    }

	printk("XXX\r\n");
*/
	return TRUE;
}

/*
 * tga bitmap reader
 */
static struct bitmap_reader bitmap_reader_tga = {
	.extension		= ".tga",
	.load			= tga_load,
};

static __init void bitmap_reader_tga_init(void)
{
	if(!register_bitmap_reader(&bitmap_reader_tga))
		LOG_E("register 'tga' bitmap reader fail");
}

static __exit void bitmap_reader_tga_exit(void)
{
	if(!unregister_bitmap_reader(&bitmap_reader_tga))
		LOG_E("unregister 'tga' bitmap reader fail");
}

module_init(bitmap_reader_tga_init, LEVEL_POSTCORE);
module_exit(bitmap_reader_tga_exit, LEVEL_POSTCORE);
