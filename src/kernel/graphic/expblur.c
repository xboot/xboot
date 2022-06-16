/*
 * kernel/graphic/expblur.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <graphic/expblur.h>

static inline void blurinner(unsigned char * p, int * zr, int * zg, int * zb, int * za, int alpha)
{
	int r, g, b;
	unsigned char a;

	b = p[0];
	g = p[1];
	r = p[2];
	a = p[3];
	*zb += (alpha * ((b << 7) - *zb)) >> 16;
	*zg += (alpha * ((g << 7) - *zg)) >> 16;
	*zr += (alpha * ((r << 7) - *zr)) >> 16;
	*za += (alpha * ((a << 7) - *za)) >> 16;
	p[0] = *zb >> 7;
	p[1] = *zg >> 7;
	p[2] = *zr >> 7;
	p[3] = *za >> 7;
}

static inline void blurrow(unsigned char * pixel, int width, int height, int x, int y, int w, int index, int alpha)
{
	unsigned char * p = pixel + (y + index) * (width << 2) + (x << 2);
	int zr, zg, zb, za;
	int i;

	zb = p[0] << 7;
	zg = p[1] << 7;
	zr = p[2] << 7;
	za = p[3] << 7;
	for(i = 0; i < w; i++, p += 4)
		blurinner(p, &zr, &zg, &zb, &za, alpha);
	for(i = w - 2, p -= 8; i >= 0; i--, p -= 4)
		blurinner(p, &zr, &zg, &zb, &za, alpha);
}

static inline void blurcol(unsigned char * pixel, int width, int height, int x, int y, int h, int index, int alpha)
{
	int stride = width << 2;
	unsigned char * p = pixel + (y + 1) * stride + ((x + index) << 2);
	int zr, zg, zb, za;
	int i;

	zb = p[0] << 7;
	zg = p[1] << 7;
	zr = p[2] << 7;
	za = p[3] << 7;
	for(i = 1; i < h; i++, p += stride)
		blurinner(p, &zr, &zg, &zb, &za, alpha);
	for(i = h - 2, p -= (stride << 1); i >= 0; i--, p -= stride)
		blurinner(p, &zr, &zg, &zb, &za, alpha);
}

void expblur(unsigned char * pixel, int width, int height, int x, int y, int w, int h, int radius)
{
	int alpha = (int)((1 << 16) * (1.0 - expf(-2.3 / (radius + 1.0))));
	int i;

	for(i = 0; i < h; i++)
		blurrow(pixel, width, height, x, y, w, i, alpha);
	for(i = 0; i < w; i++)
		blurcol(pixel, width, height, x, y, h, i, alpha);
}
