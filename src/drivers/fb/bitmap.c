/*
 * drivers/fb/bitmap.c
 *
 * Copyright (c) 2007-2010 jianjun jiang <jerryjianjun@gmail.com>
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
#include <malloc.h>
#include <string.h>
#include <xboot/list.h>
#include <fb/bitmap.h>

/* the list of bitmap reader */
static struct bitmap_reader_list __bitmap_reader_list = {
	.entry = {
		.next	= &(__bitmap_reader_list.entry),
		.prev	= &(__bitmap_reader_list.entry),
	},
};
static struct bitmap_reader_list * bitmap_reader_list = &__bitmap_reader_list;

/*
 * match extension to filename
 */
static x_bool match_extension(const char * filename, const char * ext)
{
	x_s32 pos, ext_len;

	pos = strlen((const x_s8 *)filename);
	ext_len = strlen((const x_s8 *)ext);

	if( (!pos) || (!ext_len) || (ext_len > pos) )
		return FALSE;

	pos -= ext_len;

	return strcmp((const x_s8 *)(filename + pos), (const x_s8 *)ext) == 0;
}

/*
 * search bitmap reader by extension
 */
static struct bitmap_reader * search_bitmap_reader(const char * extension)
{
	struct bitmap_reader_list * list;
	struct list_head * pos;

	if(!extension)
		return NULL;

	for(pos = (&bitmap_reader_list->entry)->next; pos != (&bitmap_reader_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct bitmap_reader_list, entry);
		if(strcmp((x_s8*)list->reader->extension, (const x_s8 *)extension) == 0)
			return list->reader;
	}

	return NULL;
}

/*
 * register a bitmap reader into bitmap_reader_list
 */
x_bool register_bitmap_reader(struct bitmap_reader * reader)
{
	struct bitmap_reader_list * list;

	list = malloc(sizeof(struct bitmap_reader_list));
	if(!list || !reader)
	{
		free(list);
		return FALSE;
	}

	if(!reader->extension || search_bitmap_reader(reader->extension))
	{
		free(list);
		return FALSE;
	}

	list->reader = reader;
	list_add(&list->entry, &bitmap_reader_list->entry);

	return TRUE;
}

/*
 * unregister bitmap reader from bitmap_reader_list
 */
x_bool unregister_bitmap_reader(struct bitmap_reader * reader)
{
	struct bitmap_reader_list * list;
	struct list_head * pos;

	if(!reader || !reader->extension)
		return FALSE;

	for(pos = (&bitmap_reader_list->entry)->next; pos != (&bitmap_reader_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct bitmap_reader_list, entry);
		if(list->reader == reader)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * determine bitmap format
 */
enum bitmap_format get_bitmap_format(struct bitmap_info * info)
{
	/* check if we have any known 32 bit modes */
	if(info->bpp == 32)
	{
		if((info->red_mask_size == 8)
			&& (info->red_field_pos == 16)
			&& (info->green_mask_size == 8)
			&& (info->green_field_pos == 8)
			&& (info->blue_mask_size == 8)
			&& (info->blue_field_pos == 0))
		{
			return BITMAP_FORMAT_BGRA_8888;
		}
		else if((info->red_mask_size == 8)
			&& (info->red_field_pos == 0)
			&& (info->green_mask_size == 8)
			&& (info->green_field_pos == 8)
			&& (info->blue_mask_size == 8)
			&& (info->blue_field_pos == 16))
		{
			return BITMAP_FORMAT_RGBA_8888;
		}
	}

	/* check if we have any known 24 bit modes */
	else if(info->bpp == 24)
	{
		if((info->red_mask_size == 8)
			&& (info->red_field_pos == 16)
			&& (info->green_mask_size == 8)
			&& (info->green_field_pos == 8)
			&& (info->blue_mask_size == 8)
			&& (info->blue_field_pos == 0))
		{
			return BITMAP_FORMAT_BGR_888;
		}
		else if ((info->red_mask_size == 8)
			&& (info->red_field_pos == 0)
			&& (info->green_mask_size == 8)
			&& (info->green_field_pos == 8)
			&& (info->blue_mask_size == 8)
			&& (info->blue_field_pos == 16))
		{
			return BITMAP_FORMAT_RGB_888;
		}
	}

	/* check if we have any known 16 bit modes */
	else if(info->bpp == 16)
	{
		if ((info->red_mask_size == 5)
			&& (info->red_field_pos == 11)
			&& (info->green_mask_size == 6)
			&& (info->green_field_pos == 5)
			&& (info->blue_mask_size == 5)
			&& (info->blue_field_pos == 0))
		{
			return BITMAP_FORMAT_BGR_565;
		}
		else if ((info->red_mask_size == 5)
			&& (info->red_field_pos == 0)
			&& (info->green_mask_size == 6)
			&& (info->green_field_pos == 5)
			&& (info->blue_mask_size == 5)
			&& (info->blue_field_pos == 11))
		{
			return BITMAP_FORMAT_RGB_565;
		}
	}

	/* check if we have any known 1 bit modes */
	else if(info->bpp == 1)
	{
		return BITMAP_FORMAT_MONOCHROME;
	}

	if(info->alpha_mask_size > 0)
		return BITMAP_FORMAT_RGBA_GENERIC;
	else
		return BITMAP_FORMAT_RGB_GENERIC;
}

/*
 * create new bitmap, saves created bitmap on success to * bitmap
 *
 * just support BITMAP_FORMAT_RGBA_8888 and BITMAP_FORMAT_RGB_888
 */
x_bool bitmap_create(struct bitmap ** bitmap, x_u32 width, x_u32 height, enum bitmap_format fmt)
{
	struct bitmap_info * info;
	x_u32 size;

	if(!bitmap)
		return FALSE;

	*bitmap = NULL;

	if(width <= 0 || height <= 0)
		return FALSE;

	*bitmap = (struct bitmap *)malloc(sizeof(struct bitmap));
	if( !(*bitmap) )
		return FALSE;

	info = &((*bitmap)->info);

	info->width = width;
	info->height = height;
	info->fmt = fmt;

	switch(fmt)
	{
	case BITMAP_FORMAT_RGBA_8888:
		info->bpp = 32;
        info->bytes_per_pixel = 4;
        info->pitch = width * 4;
        info->red_mask_size = 8;
        info->red_field_pos = 0;
        info->green_mask_size = 8;
        info->green_field_pos = 8;
        info->blue_mask_size = 8;
        info->blue_field_pos = 16;
        info->alpha_mask_size = 8;
        info->alpha_field_pos = 24;
        info->fg_r = info->fg_g = info->fg_b = info->fg_a = 0xff;
        info->bg_r = info->bg_g = info->bg_b = info->bg_a = 0x00;
		break;

	case BITMAP_FORMAT_RGB_888:
		info->bpp = 24;
        info->bytes_per_pixel = 3;
        info->pitch = width * 3;
        info->red_mask_size = 8;
        info->red_field_pos = 0;
        info->green_mask_size = 8;
        info->green_field_pos = 8;
        info->blue_mask_size = 8;
        info->blue_field_pos = 16;
        info->alpha_mask_size = 0;
        info->alpha_field_pos = 0;
        info->fg_r = info->fg_g = info->fg_b = info->fg_a = 0xff;
        info->bg_r = info->bg_g = info->bg_b = info->bg_a = 0x00;
        break;

	default:
		free(*bitmap);
        *bitmap = NULL;

        return FALSE;
	}

	(*bitmap)->viewport.left = 0;
	(*bitmap)->viewport.top = 0;
	(*bitmap)->viewport.right = width;
	(*bitmap)->viewport.bottom = height;

	size = (width * info->bytes_per_pixel) * height;
	(*bitmap)->data = malloc(size);

	if(!(*bitmap)->data)
	{
		free(*bitmap);
        *bitmap = NULL;
        return FALSE;
    }
	(*bitmap)->allocated = TRUE;

	return TRUE;
}

/*
 * load bitmap from memory picture (the gimp's c source format)
 */
x_bool bitmap_load_from_picture(struct bitmap ** bitmap, struct picture * picture)
{
	struct bitmap_info * info;

	if(!bitmap)
		return FALSE;
	*bitmap = NULL;

	if(!picture)
		return FALSE;

	if(picture->width <= 0 || picture->height <= 0)
		return FALSE;

	if( (picture->bytes_per_pixel != 3) && (picture->bytes_per_pixel != 4) )
		return FALSE;

	if(!picture->data)
		return FALSE;

	*bitmap = (struct bitmap *)malloc(sizeof(struct bitmap));
	if( !(*bitmap) )
		return FALSE;

	info = &((*bitmap)->info);
	info->width = picture->width;
	info->height = picture->height;

	if(picture->bytes_per_pixel == 3)
	{
		info->fmt = BITMAP_FORMAT_RGB_888;
		info->bpp = 24;
        info->bytes_per_pixel = 3;
        info->pitch = picture->width * 3;
        info->red_mask_size = 8;
        info->red_field_pos = 0;
        info->green_mask_size = 8;
        info->green_field_pos = 8;
        info->blue_mask_size = 8;
        info->blue_field_pos = 16;
        info->alpha_mask_size = 0;
        info->alpha_field_pos = 0;
        info->fg_r = info->fg_g = info->fg_b = info->fg_a = 0xff;
        info->bg_r = info->bg_g = info->bg_b = info->bg_a = 0x00;
	}
	else if(picture->bytes_per_pixel == 4)
	{
		info->fmt = BITMAP_FORMAT_RGBA_8888;
		info->bpp = 32;
        info->bytes_per_pixel = 4;
        info->pitch = picture->width * 4;
        info->red_mask_size = 8;
        info->red_field_pos = 0;
        info->green_mask_size = 8;
        info->green_field_pos = 8;
        info->blue_mask_size = 8;
        info->blue_field_pos = 16;
        info->alpha_mask_size = 8;
        info->alpha_field_pos = 24;
        info->fg_r = info->fg_g = info->fg_b = info->fg_a = 0xff;
        info->bg_r = info->bg_g = info->bg_b = info->bg_a = 0x00;
	}
	else
	{
		free(*bitmap);
        *bitmap = NULL;

        return FALSE;
	}

	(*bitmap)->viewport.left = 0;
	(*bitmap)->viewport.top = 0;
	(*bitmap)->viewport.right = picture->width;
	(*bitmap)->viewport.bottom = picture->height;

	(*bitmap)->data = picture->data;
	(*bitmap)->allocated = FALSE;

	return TRUE;
}

/*
 * load bitmap from file using registered bitmap reader
 */
x_bool bitmap_load_from_file(struct bitmap ** bitmap, const char * filename)
{
	struct bitmap_reader_list * list;
	struct list_head * pos;

	if(!bitmap)
		return FALSE;

	*bitmap = NULL;

	for(pos = (&bitmap_reader_list->entry)->next; pos != (&bitmap_reader_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct bitmap_reader_list, entry);
		if(list->reader->load)
		{
			if(match_extension(filename, list->reader->extension))
				return list->reader->load(bitmap, filename);
		}
	}

	return FALSE;
}

/*
 * frees all resources allocated by bitmap
 */
x_bool bitmap_destroy(struct bitmap * bitmap)
{
	if(!bitmap)
		return FALSE;

	if(bitmap->allocated)
		free(bitmap->data);
	free(bitmap);

	return TRUE;
}
