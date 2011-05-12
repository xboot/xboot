#ifndef __FONT_H__
#define __FONT_H__

#include <configs.h>
#include <default.h>
#include <fb/fb.h>

/*
 * the structure of font_glyph
 */
struct font_glyph
{
	/* ucs-4 code */
	u32_t code;

	/* width */
	u32_t w;

	/* height*/
	u32_t h;

	/* the bitmap data */
	u8_t * data;
};

/*
 * the list of font_glyph
 */
struct font_glyph_list
{
	struct font_glyph * glyph;
	struct hlist_node node;
};

/*
 * the structure of font
 */
struct font
{
	/* font name */
	char * name;

	/* font_glyph hash table */
	struct hlist_head * table;

	/* hash size */
	u32_t size;
};

/*
 * the list of font
 */
struct font_list
{
	struct font * font;
	struct list_head entry;
};

/*
 * the struct of font reader
 */
struct font_reader
{
	/* file extension for this font type with dot */
	const char * extension;

	/* load font function */
	bool_t (*load)(struct font ** font, const char * filename);
};

/*
 * the list of font reader
 */
struct font_reader_list
{
	struct font_reader * reader;
	struct list_head entry;
};


bool_t register_font_reader(struct font_reader * reader);
bool_t unregister_font_reader(struct font_reader * reader);

bool_t font_create(struct font ** font, const char * name, u32_t size);
bool_t add_font_glyph(struct font * font, struct font_glyph * glyph);
bool_t font_destory(struct font * font);


struct font * get_font(const char * name);
bool_t install_font(const char * path);
bool_t uninstall_font(const char * name);

bool_t fb_draw_text(struct fb * fb, const char * str, struct font * font, u32_t c, u32_t x, u32_t y);
bool_t bitmap_draw_text(struct bitmap * bitmap, const char * str, struct font * font, u32_t c, u32_t x, u32_t y);
bool_t font_get_metrics(const char * str, struct font * font, u32_t * w, u32_t * h);

#endif /* __FONT_H__ */
