/*
 * drivers/fb/font.c
 *
 * Copyright (c) 2007-2010 jianjun jiang <jerryjianjun@gmail.com>
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
#include <types.h>
#include <malloc.h>
#include <string.h>
#include <vsprintf.h>
#include <xboot/list.h>
#include <fb/bitmap.h>
#include <fb/fbcolor.h>
#include <fb/graphic.h>
#include <fb/font.h>

static struct font_list __font_list = {
	.entry = {
		.next	= &(__font_list.entry),
		.prev	= &(__font_list.entry),
	},
};
static struct font_list * font_list = &__font_list;

static const x_u8 unknown_glyph_data[] = {
	0xfe,	/* OOOOOOO  */
	0x82,	/* O     O  */
	0xba,	/* O OOO O  */
	0xaa,	/* O O O O  */
	0xaa,	/* O O O O  */
	0x8a,	/* O   O O  */
	0x9a,	/* O  OO O  */
	0x92,	/* O  O  O  */
	0x92,	/* O  O  O  */
	0x92,	/* O  O  O  */
	0x92,	/* O  O  O  */
	0x82,	/* O     O  */
	0x92,	/* O  O  O  */
	0x82,	/* O     O  */
	0xfe,	/* OOOOOOO  */
	0x00 	/*          */
};

static const struct font_glyph unknown_glyph = {
	.font	= NULL,
	.w		= 8,
	.h		= 16,
	.data	= (x_u8 *)(&unknown_glyph_data[0]),
};

/*
 * search font by name
 */
static struct font * search_font(const char * name)
{
	struct font_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&font_list->entry)->next; pos != (&font_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct font_list, entry);
		if(strcmp((x_s8*)list->font->name, (const x_s8 *)name) == 0)
			return list->font;
	}

	return NULL;
}

/*
 * register font into font_list
 */
static x_bool register_font(struct font * font)
{
	struct font_list * list;

	list = malloc(sizeof(struct font_list));
	if(!list || !font)
	{
		free(list);
		return FALSE;
	}

	if(!font->name || search_font(font->name))
	{
		free(list);
		return FALSE;
	}

	list->font = font;
	list_add(&list->entry, &font_list->entry);

	return TRUE;
}

/*
 * unregister font from font_list
 */
static x_bool unregister_font(struct font * font)
{
	struct font_list * list;
	struct list_head * pos;

	if(!font || !font->name)
		return FALSE;

	for(pos = (&font_list->entry)->next; pos != (&font_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct font_list, entry);
		if(list->font == font)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * get a glyph for the unicode character code in font
 */
static struct font_glyph * font_lookup_glyph(struct font * font, x_u32 code)
{
	return ( (struct font_glyph *)(&unknown_glyph) );
}

/*
 * draw the specified font glyph to framebuffer
 */
static x_bool font_draw_glyph_to_framebuffer(struct fb * fb, struct font_glyph * glyph, x_u32 c, x_u32 x, x_u32 y)
{
	struct bitmap glyph_bitmap;
	struct bitmap_info * info;

	if((glyph->w == 0) || (glyph->h == 0))
		return TRUE;

	info = &(glyph_bitmap.info);

	info->width = glyph->w;
	info->height = glyph->h;
	info->fmt =	BITMAP_FORMAT_MONOCHROME;
	info->bpp = 1;
	info->bytes_per_pixel = 0;
	info->pitch = glyph->w;

	fb_unmap_color(fb, c, &info->fg_r, &info->fg_g, &info->fg_b, &info->fg_a);
	info->bg_r = info->bg_g = info->bg_b = info->bg_a = 0x00;

	glyph_bitmap.viewport.left = 0;
	glyph_bitmap.viewport.top = 0;
	glyph_bitmap.viewport.right = glyph->w;
	glyph_bitmap.viewport.bottom = glyph->h;

	glyph_bitmap.data = glyph->data;

	return fb_blit_bitmap(fb, &glyph_bitmap, BLIT_MODE_BLEND, x, y, glyph->w, glyph->h, 0, 0);
}

/*
 * draw the specified font glyph to bitmap
 */
static x_bool font_draw_glyph_to_bitmap(struct bitmap * bitmap, struct font_glyph * glyph, x_u32 c, x_u32 x, x_u32 y)
{
	struct bitmap glyph_bitmap;
	struct bitmap_info * info;

	if((glyph->w == 0) || (glyph->h == 0))
		return TRUE;

	info = &(glyph_bitmap.info);

	info->width = glyph->w;
	info->height = glyph->h;
	info->fmt =	BITMAP_FORMAT_MONOCHROME;
	info->bpp = 1;
	info->bytes_per_pixel = 0;
	info->pitch = glyph->w;

	bitmap_unmap_color(bitmap, c, &info->fg_r, &info->fg_g, &info->fg_b, &info->fg_a);
	info->bg_r = info->bg_g = info->bg_b = info->bg_a = 0x00;

	glyph_bitmap.viewport.left = 0;
	glyph_bitmap.viewport.top = 0;
	glyph_bitmap.viewport.right = glyph->w;
	glyph_bitmap.viewport.bottom = glyph->h;

	glyph_bitmap.data = glyph->data;

	return bitmap_blit(bitmap, &glyph_bitmap, BLIT_MODE_BLEND, x, y, glyph->w, glyph->h, 0, 0);
}

/*
 * draw a utf-8 string of text on the framebuffer
 */
x_bool fb_draw_text(struct fb * fb, const char * str, struct font * font, x_u32 c, x_u32 x, x_u32 y)
{
	struct font_glyph * glyph;
	const x_u8 * p;
	x_u32 code;
	x_u32 left;

	if(!fb)
		return FALSE;

	for(p = (const x_u8 *)str, left = x; utf8_to_ucs4(&code, 1, p, -1, &p) > 0; )
	{
		glyph = font_lookup_glyph(font, code);
		if(!font_draw_glyph_to_framebuffer(fb, glyph, c, left, y))
			return FALSE;

		left += glyph->w;
	}

	return TRUE;
}

/*
 * draw a utf-8 string of text on the bitmap
 */
x_bool bitmap_draw_text(struct bitmap * bitmap, const char * str, struct font * font, x_u32 c, x_u32 x, x_u32 y)
{
	struct font_glyph * glyph;
	const x_u8 * p;
	x_u32 code;
	x_u32 left;

	if(!bitmap)
		return FALSE;

	for(p = (const x_u8 *)str, left = x; utf8_to_ucs4(&code, 1, p, -1, &p) > 0; )
	{
		glyph = font_lookup_glyph(font, code);
		if(!font_draw_glyph_to_bitmap(bitmap, glyph, c, left, y))
			return FALSE;

		left += glyph->w;
	}

	return TRUE;
}

/*
 * get a utf-8 string's metrics
 */
x_bool font_get_metrics(const char * str, struct font * font, x_u32 * w, x_u32 * h)
{
	struct font_glyph * glyph;
	const x_u8 * p;
	x_u32 code;
	x_u32 width = 0, height = 0;

	if(!w && !h)
		return FALSE;

	for(p = (const x_u8 *)str; utf8_to_ucs4(&code, 1, p, -1, &p) > 0; )
	{
		glyph = font_lookup_glyph(font, code);
		width += glyph->w;
		if(height < glyph->h)
			height = glyph->h;
	}

	if(w)
		*w = width;
	if(h)
		*h = height;

	return TRUE;
}

/*
 * load font from file path
 */
x_bool font_load(const char * path)
{
	return FALSE;
}

/*
 * get font by font name
 */
struct font * font_get(const char * name)
{
	return NULL;
}

/*
 * remove font by font name
 */
x_bool font_remove(const char * name)
{
	return FALSE;
}
