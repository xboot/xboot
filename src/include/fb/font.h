#ifndef __FONT_H__
#define __FONT_H__

#include <configs.h>
#include <default.h>

/*
 * the structure of font
 */
struct font
{
	/* file name */
	const char * name;

	/* file opened id */
	x_s32 file;
};

/*
 * the structure of font_glyph
 */
struct font_glyph
{
	/* reference to the font this glyph belongs to */
	struct font * font;

	/* width */
	x_u32 w;

	/* height*/
	x_u32 h;

	/* the bitmap data */
	x_u8 * data;
};

/*
 * the list of font
 */
struct font_list
{
	struct font * font;
	struct list_head entry;
};

x_bool fb_draw_text(struct fb * fb, const char * str, struct font * font, x_u32 c, x_u32 x, x_u32 y);
x_bool bitmap_draw_text(struct bitmap * bitmap, const char * str, struct font * font, x_u32 c, x_u32 x, x_u32 y);
x_bool font_get_metrics(const char * str, struct font * font, x_u32 * w, x_u32 * h);

x_bool font_load(const char * path);
struct font * font_get(const char * name);
x_bool font_remove(const char * name);

#endif /* __FONT_H__ */
