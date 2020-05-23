/*
 * kernel/graphic/text.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <charset.h>
#include <graphic/surface.h>
#include <graphic/font.h>
#include <graphic/text.h>

struct text_t * text_alloc(const char * utf8, struct color_t * c, struct font_context_t * fctx, const char * family, int size)
{
	struct text_t * txt;

	if(!fctx)
		return NULL;

	txt = malloc(sizeof(struct text_t));
	if(!txt)
		return NULL;

	txt->utf8 = strdup(utf8 ? utf8 : "");
	if(c)
		memcpy(&txt->c, c, sizeof(struct color_t));
	else
		color_init(&txt->c, 0xff, 0xff, 0xff, 0xff);
	txt->fctx = fctx;
	txt->family = strdup(family ? family : "roboto");
	txt->size = (size > 0) ? size : 24;
	calc_text_extent(txt);

	return txt;
}

void text_free(struct text_t * txt)
{
	if(txt)
	{
		if(txt->utf8)
			free(txt->utf8);
		if(txt->family)
			free(txt->family);
		free(txt);
	}
}

void text_set_text(struct text_t * txt, const char * utf8)
{
	if(txt)
	{
		if(txt->utf8)
			free(txt->utf8);
		txt->utf8 = strdup(utf8 ? utf8 : "");
		calc_text_extent(txt);
	}
}

void text_set_color(struct text_t * txt, struct color_t * c)
{
	if(txt)
	{
		if(c)
			memcpy(&txt->c, c, sizeof(struct color_t));
		else
			color_init(&txt->c, 0xff, 0xff, 0xff, 0xff);
	}
}

void text_set_font_family(struct text_t * txt, const char * family)
{
	if(txt)
	{
		if(txt->family)
			free(txt->family);
		txt->utf8 = strdup(family ? family : "roboto");
		calc_text_extent(txt);
	}
}

void text_set_font_size(struct text_t * txt, int size)
{
	if(txt)
	{
		txt->size = (size > 0) ? size : 24;
		calc_text_extent(txt);
	}
}
