#ifndef __PIXEL_H__
#define __PIXEL_H__

#include <xboot.h>
#include <graphic/color.h>

enum pixel_format
{
	PIXEL_FORMAT_GENERIC		= 0,

	PIXEL_FORMAT_ABGR_8888		= 1,
	PIXEL_FORMAT_ARGB_8888		= 2,
	PIXEL_FORMAT_BGRA_8888		= 3,
	PIXEL_FORMAT_RGBA_8888		= 4,

	PIXEL_FORMAT_BGR_888		= 5,
	PIXEL_FORMAT_RGB_888		= 6,

	PIXEL_FORMAT_INDEX_1BIT		= 7,
};

struct pixel_info {
	/* bits per pixel */
	u8_t bits_per_pixel;

	/* bytes per pixel */
	u8_t bytes_per_pixel;

	/* how many bits are reserved for red color */
	u8_t red_mask_size;

	/* what is location of red color bits */
	u8_t red_field_pos;

	/* how many bits are reserved for green color */
	u8_t green_mask_size;

	/* what is location of green color bits */
	u8_t green_field_pos;

	/* how many bits are reserved for blue color */
	u8_t blue_mask_size;

	/* what is location of blue color bits */
	u8_t blue_field_pos;

	/* how many bits are alpha in color */
	u8_t alpha_mask_size;

	/* what is location of alpha color bits */
	u8_t alpha_field_pos;

	/* index 1-bit palette */
	struct color_t palette[2];

	/* pixel format */
	enum pixel_format fmt;
};


enum pixel_format get_pixel_format(struct pixel_info * info);
void set_pixel_info(struct pixel_info * info, enum pixel_format fmt);

u32_t map_pixel_color(struct pixel_info * info, struct color_t * col);
void unmap_pixel_color(struct pixel_info * info, u32_t c, struct color_t * col);

#endif /* __PIXEL_H__ */
