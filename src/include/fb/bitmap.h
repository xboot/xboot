#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <configs.h>
#include <default.h>

/*
 * defined blit formats
 */
enum bitmap_format
{
	BITMAP_FORMAT_RGBA_GENERIC,
	BITMAP_FORMAT_RGB_GENERIC,

	BITMAP_FORMAT_RGBA_8888,
	BITMAP_FORMAT_BGRA_8888,

	BITMAP_FORMAT_RGB_888,
	BITMAP_FORMAT_BGR_888,

	BITMAP_FORMAT_RGB_565,
	BITMAP_FORMAT_BGR_565,

	BITMAP_FORMAT_MONOCHROME,
};

/*
 * the gimp's c source format picture
 */
struct picture
{
	/* width of the picture*/
	u32_t width;

	/* height of the picture*/
	u32_t height;

	/* bytes per pixel */
	u32_t bytes_per_pixel;

	/* the pixel data */
	u8_t * data;
};

struct bitmap_info
{
	/* width of the bitmap */
	u32_t width;

	/* height of the bitmap */
	u32_t height;

	/* bits per pixel */
	u32_t bpp;

	/* bytes per pixel */
	u32_t bytes_per_pixel;

	/* pitch of one scan line, how many bytes there are for scan line */
	u32_t pitch;

	/* how many bits are reserved for red color */
	u32_t red_mask_size;

	/* what is location of red color bits */
	u32_t red_field_pos;

	/* how many bits are reserved for green color */
	u32_t green_mask_size;

	/* what is location of green color bits */
	u32_t green_field_pos;

	/* how many bits are reserved for blue color */
	u32_t blue_mask_size;

	/* what is location of blue color bits */
	u32_t blue_field_pos;

	/* how many bits are alpha in color */
	u32_t alpha_mask_size;

	/* what is location of alpha color bits */
	u32_t alpha_field_pos;

	/* optimization hint how binary data is coded */
	enum bitmap_format fmt;

	/* for monochrome bitmaps, the foreground and background color */
	u8_t fg_r, fg_g, fg_b, fg_a;
	u8_t bg_r, bg_g, bg_b, bg_a;
};

struct bitmap
{
	/* bitmap information description */
	struct bitmap_info info;

	/* bitmap's view port */
	struct {
		s32_t	left;
		s32_t	top;
		s32_t 	right;
		s32_t	bottom;
	} viewport;

	/* if the data in heap (malloc method), the it will be set */
	bool_t allocated;

	/* pointer to bitmap data formatted according to info */
	void * data;
};

/*
 * the struct of bitmap reader
 */
struct bitmap_reader
{
	/* file extension for this bitmap type with dot */
	const char * extension;

	/* load bitmap function */
	bool_t (*load)(struct bitmap ** bitmap, const char * filename);
};

/*
 * the list of bitmap reader
 */
struct bitmap_reader_list
{
	struct bitmap_reader * reader;
	struct list_head entry;
};

bool_t register_bitmap_reader(struct bitmap_reader * reader);
bool_t unregister_bitmap_reader(struct bitmap_reader * reader);

enum bitmap_format get_bitmap_format(struct bitmap_info * info);
bool_t bitmap_create(struct bitmap ** bitmap, u32_t width, u32_t height, enum bitmap_format fmt);
bool_t bitmap_load_from_picture(struct bitmap ** bitmap, struct picture * picture);
bool_t bitmap_load_from_file(struct bitmap ** bitmap, const char * filename);
bool_t bitmap_destroy(struct bitmap * bitmap);

#endif /* __BITMAP_H__ */
