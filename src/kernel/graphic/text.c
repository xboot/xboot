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

void text_init(struct text_t * txt, const char * utf8, int len, struct color_t * c, struct font_context_t * fctx, const char * family, int size)
{
	if(txt)
	{
		txt->utf8 = utf8;
		txt->len = (len < 0) ? strlen(utf8) : len;
		txt->c = c;
		txt->fctx = fctx;
		txt->family = family;
		txt->size = (size > 0) ? size : 16;
		calc_text_extent(txt);
	}
}

void text_set_text(struct text_t * txt, const char * utf8, int len)
{
	if(txt)
	{
		txt->utf8 = utf8;
		txt->len = (len < 0) ? strlen(utf8) : len;
		calc_text_extent(txt);
	}
}

void text_set_color(struct text_t * txt, struct color_t * c)
{
	if(txt)
		txt->c = c;
}

void text_set_font_family(struct text_t * txt, const char * family)
{
	if(txt)
	{
		txt->family = family;
		calc_text_extent(txt);
	}
}

void text_set_font_size(struct text_t * txt, int size)
{
	if(txt)
	{
		txt->size = (size > 0) ? size : 16;
		calc_text_extent(txt);
	}
}
