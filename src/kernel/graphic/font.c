/*
 * kernel/graphic/font.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <string.h>
#include <hmap.h>
#include <graphic/font.h>
#include <ft2build.h>
#include FT_FREETYPE_H

struct font_context_t {
	FT_Library lib;
	struct hmap_t * map;
} __font_context;

static struct font_t * load_font_file(struct font_context_t * ctx, const char * family)
{
	struct font_t * font;
	FT_Face face;

	font = malloc(sizeof(struct font_t));
	if(!font)
		return NULL;

	if(FT_New_Face(ctx->lib, family, 0, &face))
	{
		free(font);
		return NULL;
	}
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);

	font->family = strdup(face->family_name);
	font->style = strdup(face->style_name);
	font->face = face;
	return font;
}

struct font_t * search_font(const char * family)
{
	struct font_context_t * ctx = &__font_context;
	struct font_t * font = hmap_search(ctx->map, family);
	if(!font)
		font = hmap_search(ctx->map, "sans-serif");
	return font;
}

void do_loadfont(void)
{
	struct font_context_t * ctx = &__font_context;
	struct font_t * font;

	FT_Init_FreeType(&ctx->lib);
	ctx->map = hmap_alloc(0);

	font = load_font_file(ctx, "/framework/assets/fonts/Roboto-Regular.ttf");
	if(font)
		hmap_add(ctx->map, "sans-serif", font);
}
