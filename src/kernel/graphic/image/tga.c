/*
 * kernel/graphic/image/tga.c
 *
 * Copyright (c) 2007-2011  jianjun jiang <jerryjianjun@gmail.com>
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

#include <graphic/image.h>

enum
{
	TGA_IMAGE_TYPE_NONE 						= 0,
	TGA_IMAGE_TYPE_UNCOMPRESSED_INDEXCOLOR 		= 1,
	TGA_IMAGE_TYPE_UNCOMPRESSED_TRUECOLOR 		= 2,
	TGA_IMAGE_TYPE_UNCOMPRESSED_BLACK_AND_WHITE = 3,
	TGA_IMAGE_TYPE_RLE_INDEXCOLOR 				= 9,
	TGA_IMAGE_TYPE_RLE_TRUECOLOR 				= 10,
	TGA_IMAGE_TYPE_RLE_BLACK_AND_WHITE 			= 11,
};

enum
{
	TGA_IMAGE_ORIGIN_RIGHT	= 0x10,
	TGA_IMAGE_ORIGIN_TOP	= 0x20
};

struct tga_header
{
	u8_t id_length;
	u8_t color_map_type;
	u8_t image_type;

	/* color map specification */
	u16_t color_map_first_index;
	u16_t color_map_length;
	u8_t color_map_bpp;

	/* image specification */
	u16_t image_x_origin;
	u16_t image_y_origin;
	u16_t image_width;
	u16_t image_height;
	u8_t image_bpp;
	u8_t image_descriptor;

} __attribute__ ((packed));

static bool_t tga_load_truecolor_r8g8b8a8(struct surface_t * surface, struct tga_header * header, s32_t fd)
{
	u32_t x, y;
	u8_t * ptr;
	u8_t tmp[4];
	u8_t bytes_per_pixel;

	bytes_per_pixel = header->image_bpp / 8;

	for(y = 0; y < header->image_height; y++)
	{
		ptr = surface->pixels;
		if((header->image_descriptor & TGA_IMAGE_ORIGIN_TOP) != 0)
			ptr += y * surface->pitch;
		else
			ptr += (header->image_height - 1 - y) * surface->pitch;

		for(x = 0; x < header->image_width; x++)
		{
			if(read(fd, &tmp[0], bytes_per_pixel) != bytes_per_pixel)
				return FALSE;

			ptr[0] = tmp[2];
			ptr[1] = tmp[1];
			ptr[2] = tmp[0];
			ptr[3] = tmp[3];

			ptr += 4;
		}
	}

	return TRUE;
}

static bool_t tga_load_truecolor_r8g8b8(struct surface_t * surface, struct tga_header * header, s32_t fd)
{
	u32_t x, y;
	u8_t * ptr;
	u8_t tmp[4];
	u8_t bytes_per_pixel;

	bytes_per_pixel = header->image_bpp / 8;

	for(y = 0; y < header->image_height; y++)
	{
		ptr = surface->pixels;
		if((header->image_descriptor & TGA_IMAGE_ORIGIN_TOP) != 0)
			ptr += y * surface->pitch;
		else
			ptr += (header->image_height - 1 - y) * surface->pitch;

		for(x = 0; x < header->image_width; x++)
		{
			if(read(fd, &tmp[0], bytes_per_pixel) != bytes_per_pixel)
				return FALSE;

			ptr[0] = tmp[2];
			ptr[1] = tmp[1];
			ptr[2] = tmp[0];

			ptr += 3;
		}
	}

	return TRUE;
}

static bool_t tga_load_truecolor_rle_r8g8b8a8(struct surface_t * surface, struct tga_header * header, s32_t fd)
{
	u32_t x, y;
	u8_t type;
	u8_t * ptr;
	u8_t tmp[4];
	u8_t bytes_per_pixel;

	bytes_per_pixel = header->image_bpp / 8;

	for(y = 0; y < header->image_height; y++)
	{
		ptr = surface->pixels;
		if((header->image_descriptor & TGA_IMAGE_ORIGIN_TOP) != 0)
			ptr += y * surface->pitch;
		else
			ptr += (header->image_height - 1 - y) * surface->pitch;

		for(x = 0; x < header->image_width;)
		{
			if(read(fd, &type, sizeof(type)) != sizeof(type))
				return FALSE;

			if(type & 0x80)
			{
				/* rle-encoded packet */
				type &= 0x7f;
				type++;

				if(read(fd, &tmp[0], bytes_per_pixel) != bytes_per_pixel)
					return FALSE;

				while(type)
				{
					if(x < header->image_width)
					{
						ptr[0] = tmp[2];
						ptr[1] = tmp[1];
						ptr[2] = tmp[0];
						ptr[3] = tmp[3];
						ptr += 4;
					}

					type--;
					x++;
				}
			}
			else
			{
				/* raw-encoded packet */
				type++;

				while(type)
				{
					if(read(fd, &tmp[0], bytes_per_pixel) != bytes_per_pixel)
						return FALSE;

					if (x < header->image_width)
					{
						ptr[0] = tmp[2];
						ptr[1] = tmp[1];
						ptr[2] = tmp[0];
						ptr[3] = tmp[3];
						ptr += 4;
					}

					type--;
					x++;
				}
			}
		}
	}

	return TRUE;
}

static bool_t tga_load_truecolor_rle_r8g8b8(struct surface_t * surface, struct tga_header * header, s32_t fd)
{
	u32_t x, y;
	u8_t type;
	u8_t * ptr;
	u8_t tmp[4];
	u8_t bytes_per_pixel;

	bytes_per_pixel = header->image_bpp / 8;

	for(y = 0; y < header->image_height; y++)
	{
		ptr = surface->pixels;
		if((header->image_descriptor & TGA_IMAGE_ORIGIN_TOP) != 0)
			ptr += y * surface->pitch;
		else
			ptr += (header->image_height - 1 - y) * surface->pitch;

		for(x = 0; x < header->image_width;)
		{
			if(read(fd, &type, sizeof(type)) != sizeof(type))
				return FALSE;

			if(type & 0x80)
			{
				/* rle-encoded packet */
				type &= 0x7f;
				type++;

				if(read(fd, &tmp[0], bytes_per_pixel) != bytes_per_pixel)
					return FALSE;

				while(type)
				{
					if(x < header->image_width)
					{
						ptr[0] = tmp[2];
						ptr[1] = tmp[1];
						ptr[2] = tmp[0];
						ptr += 3;
					}

					type--;
					x++;
				}
			}
			else
			{
				/* raw-encoded packet */
				type++;

				while(type)
				{
					if(read(fd, &tmp[0], bytes_per_pixel) != bytes_per_pixel)
						return FALSE;

					if (x < header->image_width)
					{
						ptr[0] = tmp[2];
						ptr[1] = tmp[1];
						ptr[2] = tmp[0];
						ptr += 3;
					}

					type--;
					x++;
				}
			}
		}
	}

	return TRUE;
}

static struct surface_t * tga_load(const char * filename)
{
	struct surface_t * surface;
	struct tga_header header;
	struct stat st;
	s32_t fd;
	bool_t has_alpha;

	if(stat(filename, &st) != 0)
		return NULL;

	if(S_ISDIR(st.st_mode))
		return NULL;

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return NULL;

	if(read(fd, (void*)(&header), sizeof(struct tga_header)) != sizeof(struct tga_header))
	{
		close(fd);
		return NULL;
	}

	header.color_map_first_index = cpu_to_le16( *((u16_t *)(&header.color_map_first_index)) );
	header.color_map_length = cpu_to_le16( *((u16_t *)(&header.color_map_length)) );
	header.image_x_origin = cpu_to_le16( *((u16_t *)(&header.image_x_origin)) );
	header.image_y_origin = cpu_to_le16( *((u16_t *)(&header.image_y_origin)) );
	header.image_width = cpu_to_le16( *((u16_t *)(&header.image_width)) );
	header.image_height = cpu_to_le16( *((u16_t *)(&header.image_height)) );

	if(lseek(fd, sizeof(struct tga_header) + header.id_length, VFS_SEEK_SET) < 0)
	{
		close(fd);
		return NULL;
	}

	/* check that bitmap encoding is supported */
	switch(header.image_type)
	{
	case TGA_IMAGE_TYPE_UNCOMPRESSED_TRUECOLOR:
	case TGA_IMAGE_TYPE_RLE_TRUECOLOR:
		break;

	default:
		close(fd);
		return NULL;
	}

	/* check that bitmap depth is supported */
	switch(header.image_bpp)
	{
	case 24:
		has_alpha = FALSE;
		break;

	case 32:
		has_alpha = TRUE;
		break;

	default:
		close(fd);
		return NULL;
	}

	if(has_alpha)
	{
		surface = surface_alloc(0, header.image_width, header.image_height, PIXEL_FORMAT_ABGR_8888);
		if(!surface)
		{
			close(fd);
			return NULL;
		}

		switch(header.image_type)
		{
		case TGA_IMAGE_TYPE_UNCOMPRESSED_TRUECOLOR:
			if(!tga_load_truecolor_r8g8b8a8(surface, &header, fd))
			{
				surface_free(surface);
				close(fd);
				return NULL;
			}
			break;

		case TGA_IMAGE_TYPE_RLE_TRUECOLOR:
			if(!tga_load_truecolor_rle_r8g8b8a8(surface, &header, fd))
			{
				surface_free(surface);
				close(fd);
				return NULL;
			}
			break;

		default:
			surface_free(surface);
			close(fd);
			return NULL;
		}
	}
	else
	{
		surface = surface_alloc(0, header.image_width, header.image_height, PIXEL_FORMAT_BGR_888);
		if(!surface)
		{
			close(fd);
			return NULL;
		}

		switch(header.image_type)
		{
		case TGA_IMAGE_TYPE_UNCOMPRESSED_TRUECOLOR:
			if(!tga_load_truecolor_r8g8b8(surface, &header, fd))
			{
				surface_free(surface);
				close(fd);
				return NULL;
			}
			break;

		case TGA_IMAGE_TYPE_RLE_TRUECOLOR:
			if(!tga_load_truecolor_rle_r8g8b8(surface, &header, fd))
			{
				surface_free(surface);
				close(fd);
				return NULL;
			}
			break;

		default:
			surface_free(surface);
			close(fd);
			return NULL;
		}
	}

	close(fd);
	return surface;
}

static struct image_loader image_loader_tga = {
	.extension		= ".tga",
	.load			= tga_load,
};

static __init void image_loader_tga_init(void)
{
	if(!register_image_loader(&image_loader_tga))
		LOG_E("register 'tga' bitmap reader fail");
}

static __exit void image_loader_tga_exit(void)
{
	if(!unregister_image_loader(&image_loader_tga))
		LOG_E("unregister 'tga' bitmap reader fail");
}

module_init(image_loader_tga_init, LEVEL_POSTCORE);
module_exit(image_loader_tga_exit, LEVEL_POSTCORE);
