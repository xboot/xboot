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

	/* glyph bitmap width in pixels */
	x_u32 width;

	/* glyph bitmap height in pixels */
	x_u32 height;

	/* glyph bitmap x offset in pixels */
	x_u32 offset_x;

	/* glyph bitmap y offset in pixels */
	x_u32 offset_y;

	/* Number of pixels to advance to start the next character.  */
/*	grub_uint16_t device_width;*/

	/* Row-major order, packed bits (no padding; rows can break within a byte).
	The length of the array is (width * height + 7) / 8.  Within a
	byte, the most significant bit is the first (leftmost/uppermost) pixel.
	Pixels are coded as bits, value 1 meaning of opaque pixel and 0 is
	transparent.  If the length of the array does not fit byte boundary, it
	will be padded with 0 bits to make it fit.  */
	x_u8 bitmap[0];
};

/*
 * the list of font
 */
struct font_list
{
	struct font * font;
	struct list_head entry;
};

x_bool font_load(const char * path);
struct font * font_get(const char * name);
x_bool font_remove(const char * name);

#endif /* __FONT_H__ */
