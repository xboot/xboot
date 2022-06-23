/*
 * kernel/graphic/filter.c
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
#include <graphic/surface.h>

void surface_filter_gray(struct surface_t * s)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	unsigned char gray;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			gray = (p[2] * 19595L + p[1] * 38469L + p[0] * 7472L) >> 16;
			p[0] = gray;
			p[1] = gray;
			p[2] = gray;
		}
	}
}

void surface_filter_sepia(struct surface_t * s)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int r, g, b;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			b = (p[2] * 17826L + p[1] * 34996L + p[0] * 8585L) >> 16;
			g = (p[2] * 22872L + p[1] * 44958L + p[0] * 11010L) >> 16;
			r = (p[2] * 25756L + p[1] * 50397L + p[0] * 12386L) >> 16;
			p[0] = min(b, 255);
			p[1] = min(g, 255);
			p[2] = min(r, 255);
		}
	}
}

void surface_filter_invert(struct surface_t * s)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			p[0] = p[3] - p[0];
			p[1] = p[3] - p[1];
			p[2] = p[3] - p[2];
		}
	}
}

void surface_filter_coloring(struct surface_t * s, struct color_t * c)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	unsigned char r = c->r;
	unsigned char g = c->g;
	unsigned char b = c->b;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] == 255)
		{
			p[0] = b;
			p[1] = g;
			p[2] = r;
		}
		else
		{
			p[0] = idiv255(b * p[3]);
			p[1] = idiv255(g * p[3]);
			p[2] = idiv255(r * p[3]);
		}
	}
}

void surface_filter_hue(struct surface_t * s, int angle)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	float av = angle * M_PI / 180.0;
	float cv = cosf(av);
	float sv = sinf(av);
	int r, g, b;
	int tr, tg, tb;
	int m[9];

	m[0] = (0.213 + cv * 0.787 - sv * 0.213) * 65536;
	m[1] = (0.715 - cv * 0.715 - sv * 0.715) * 65536;
	m[2] = (0.072 - cv * 0.072 + sv * 0.928) * 65536;
	m[3] = (0.213 - cv * 0.213 + sv * 0.143) * 65536;
	m[4] = (0.715 + cv * 0.285 + sv * 0.140) * 65536;
	m[5] = (0.072 - cv * 0.072 - sv * 0.283) * 65536;
	m[6] = (0.213 - cv * 0.213 - sv * 0.787) * 65536;
	m[7] = (0.715 - cv * 0.715 + sv * 0.715) * 65536;
	m[8] = (0.072 + cv * 0.928 + sv * 0.072) * 65536;
	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				tb = (m[6] * r + m[7] * g + m[8] * b) >> 16;
				tg = (m[3] * r + m[4] * g + m[5] * b) >> 16;
				tr = (m[0] * r + m[1] * g + m[2] * b) >> 16;
				p[0] = clamp(tb, 0, 255);
				p[1] = clamp(tg, 0, 255);
				p[2] = clamp(tr, 0, 255);
			}
			else
			{
				b = p[0] * 255 / p[3];
				g = p[1] * 255 / p[3];
				r = p[2] * 255 / p[3];
				tb = (m[6] * r + m[7] * g + m[8] * b) >> 16;
				tg = (m[3] * r + m[4] * g + m[5] * b) >> 16;
				tr = (m[0] * r + m[1] * g + m[2] * b) >> 16;
				p[0] = clamp(idiv255(tb * p[3]), 0, 255);
				p[1] = clamp(idiv255(tg * p[3]), 0, 255);
				p[2] = clamp(idiv255(tr * p[3]), 0, 255);
			}
		}
	}
}

void surface_filter_saturate(struct surface_t * s, int saturate)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int v = clamp(saturate, -100, 100) * 128 / 100;
	int r, g, b, vmin, vmax;
	int alpha, delta, value, lv, sv;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				vmin = min(min(r, g), b);
				vmax = max(max(r, g), b);
				delta = vmax - vmin;
				value = vmax + vmin;
				if(delta == 0)
					continue;
				lv = value >> 1;
				sv = lv < 128 ? (delta << 7) / value : (delta << 7) / (510 - value);
				if(v >= 0)
				{
					alpha = (v + sv >= 128) ? sv : 128 - v;
					if(alpha != 0)
						alpha = 128 * 128 / alpha - 128;
				}
				else
				{
					alpha = v;
				}
				r = r + ((r - lv) * alpha >> 7);
				g = g + ((g - lv) * alpha >> 7);
				b = b + ((b - lv) * alpha >> 7);
				p[0] = clamp(b, 0, 255);
				p[1] = clamp(g, 0, 255);
				p[2] = clamp(r, 0, 255);
			}
			else
			{
				b = p[0] * 255 / p[3];
				g = p[1] * 255 / p[3];
				r = p[2] * 255 / p[3];
				vmin = min(min(r, g), b);
				vmax = max(max(r, g), b);
				delta = vmax - vmin;
				value = vmax + vmin;
				if(delta == 0)
					continue;
				lv = value >> 1;
				sv = lv < 128 ? (delta << 7) / value : (delta << 7) / (510 - value);
				if(v >= 0)
				{
					alpha = (v + sv >= 128) ? sv : 128 - v;
					if(alpha != 0)
						alpha = 128 * 128 / alpha - 128;
				}
				else
				{
					alpha = v;
				}
				r = r + ((r - lv) * alpha >> 7);
				g = g + ((g - lv) * alpha >> 7);
				b = b + ((b - lv) * alpha >> 7);
				p[0] = clamp(idiv255(b * p[3]), 0, 255);
				p[1] = clamp(idiv255(g * p[3]), 0, 255);
				p[2] = clamp(idiv255(r * p[3]), 0, 255);
			}
		}
	}
}

void surface_filter_brightness(struct surface_t * s, int brightness)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int t, v = clamp(brightness, -100, 100) * 255 / 100;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				p[0] = clamp(p[0] + v, 0, 255);
				p[1] = clamp(p[1] + v, 0, 255);
				p[2] = clamp(p[2] + v, 0, 255);
			}
			else
			{
				t = idiv255(v * p[3]);
				p[0] = clamp(p[0] + t, 0, 255);
				p[1] = clamp(p[1] + t, 0, 255);
				p[2] = clamp(p[2] + t, 0, 255);
			}
		}
	}
}

void surface_filter_contrast(struct surface_t * s, int contrast)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int v = clamp(contrast, -100, 100) * 128 / 100;
	int r, g, b;
	int tr, tg, tb;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				tb = ((b << 7) + (b - 128) * v) >> 7;
				tg = ((g << 7) + (g - 128) * v) >> 7;
				tr = ((r << 7) + (r - 128) * v) >> 7;
				p[0] = clamp(tb, 0, 255);
				p[1] = clamp(tg, 0, 255);
				p[2] = clamp(tr, 0, 255);
			}
			else
			{
				b = p[0] * 255 / p[3];
				g = p[1] * 255 / p[3];
				r = p[2] * 255 / p[3];
				tb = ((b << 7) + (b - 128) * v) >> 7;
				tg = ((g << 7) + (g - 128) * v) >> 7;
				tr = ((r << 7) + (r - 128) * v) >> 7;
				p[0] = clamp(idiv255(tb * p[3]), 0, 255);
				p[1] = clamp(idiv255(tg * p[3]), 0, 255);
				p[2] = clamp(idiv255(tr * p[3]), 0, 255);
			}
		}
	}
}

void surface_filter_opacity(struct surface_t * s, int alpha)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int v = clamp(alpha, 0, 100) * 256 / 100;

	switch(v)
	{
	case 0:
		memset(s->pixels, 0, s->pixlen);
		break;
	case 256:
		break;
	default:
		for(i = 0; i < len; i++, p += 4)
		{
			if(p[3] != 0)
			{
				p[0] = (p[0] * v) >> 8;
				p[1] = (p[1] * v) >> 8;
				p[2] = (p[2] * v) >> 8;
				p[3] = (p[3] * v) >> 8;
			}
		}
		break;
	}
}

void surface_filter_haldclut(struct surface_t * s, struct surface_t * clut, const char * type)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, * q = surface_get_pixels(s);
	int cw = surface_get_width(clut);
	int ch = surface_get_height(clut);
	unsigned char * t, * cp, * cq = surface_get_pixels(clut);
	double sum[9];
	double dr, dg, db, xdr, xdg, xdb;
	int ri, gi, bi;
	int x, y, v;
	int level, level2, level_1, level_2;

	if(cw == ch)
	{
		switch(cw)
		{
		case 8:    level =  2 *  2; break;
		case 27:   level =  3 *  3; break;
		case 64:   level =  4 *  4; break;
		case 125:  level =  5 *  5; break;
		case 216:  level =  6 *  6; break;
		case 343:  level =  7 *  7; break;
		case 512:  level =  8 *  8; break;
		case 729:  level =  9 *  9; break;
		case 1000: level = 10 * 10; break;
		case 1331: level = 11 * 11; break;
		case 1728: level = 12 * 12; break;
		case 2197: level = 13 * 13; break;
		case 2744: level = 14 * 14; break;
		case 3375: level = 15 * 15; break;
		case 4096: level = 16 * 16; break;
		default:
			return;
		}
		level2 = level * level;
		level_1 = level - 1;
		level_2 = level - 2;
		switch(shash(type))
		{
		case 0x09fa48d7: /* "nearest" */
			for(y = 0; y < height; y++, q += stride)
			{
				for(x = 0, p = q; x < width; x++, p += 4)
				{
					if(p[3] != 0)
					{
						if(p[3] == 255)
						{
							bi = idiv255(p[0] * level_1);
							if(bi > level_2)
								bi = level_2;
							gi = idiv255(p[1] * level_1);
							if(gi > level_2)
								gi = level_2;
							ri = idiv255(p[2] * level_1);
							if(ri > level_2)
								ri = level_2;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							p[0] = cp[0];
							p[1] = cp[1];
							p[2] = cp[2];
						}
						else
						{
							bi = p[0] * level_1 / p[3];
							if(bi > level_2)
								bi = level_2;
							gi = p[1] * level_1 / p[3];
							if(gi > level_2)
								gi = level_2;
							ri = p[2] * level_1 / p[3];
							if(ri > level_2)
								ri = level_2;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							p[0] = idiv255(cp[0] * p[3]);
							p[1] = idiv255(cp[1] * p[3]);
							p[2] = idiv255(cp[2] * p[3]);
						}
					}
				}
			}
			break;
		case 0x860ab38f: /* "trilinear" */
			for(y = 0; y < height; y++, q += stride)
			{
				for(x = 0, p = q; x < width; x++, p += 4)
				{
					if(p[3] != 0)
					{
						if(p[3] == 255)
						{
							bi = idiv255(p[0] * level_1);
							if(bi > level_2)
								bi = level_2;
							gi = idiv255(p[1] * level_1);
							if(gi > level_2)
								gi = level_2;
							ri = idiv255(p[2] * level_1);
							if(ri > level_2)
								ri = level_2;
							db = (double)p[0] * level_1 / 255 - bi;
							dg = (double)p[1] * level_1 / 255 - gi;
							dr = (double)p[2] * level_1 / 255 - ri;
							xdb = 1 - db;
							xdg = 1 - dg;
							xdr = 1 - dr;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							t = cp;
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + (level << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[6] = sum[0] * xdg + sum[3] * dg;
							sum[7] = sum[1] * xdg + sum[4] * dg;
							sum[8] = sum[2] * xdg + sum[5] * dg;
							t = cp + (level2 << 2);
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + ((level2 + level) << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[0] = sum[0] * xdg + sum[3] * dg;
							sum[1] = sum[1] * xdg + sum[4] * dg;
							sum[2] = sum[2] * xdg + sum[5] * dg;
							v = sum[6] * xdb + sum[0] * db;
							p[0] = clamp(v, 0, 255);
							v = sum[7] * xdb + sum[1] * db;
							p[1] = clamp(v, 0, 255);
							v = sum[8] * xdb + sum[2] * db;
							p[2] = clamp(v, 0, 255);
						}
						else
						{
							bi = p[0] * level_1 / p[3];
							if(bi > level_2)
								bi = level_2;
							gi = p[1] * level_1 / p[3];
							if(gi > level_2)
								gi = level_2;
							ri = p[2] * level_1 / p[3];
							if(ri > level_2)
								ri = level_2;
							db = (double)p[0] * level_1 / p[3] - bi;
							dg = (double)p[1] * level_1 / p[3] - gi;
							dr = (double)p[2] * level_1 / p[3] - ri;
							xdb = 1 - db;
							xdg = 1 - dg;
							xdr = 1 - dr;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							t = cp;
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + (level << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[6] = sum[0] * xdg + sum[3] * dg;
							sum[7] = sum[1] * xdg + sum[4] * dg;
							sum[8] = sum[2] * xdg + sum[5] * dg;
							t = cp + (level2 << 2);
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + ((level2 + level) << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[0] = sum[0] * xdg + sum[3] * dg;
							sum[1] = sum[1] * xdg + sum[4] * dg;
							sum[2] = sum[2] * xdg + sum[5] * dg;
							v = (sum[6] * xdb + sum[0] * db) * p[3] / 255;
							p[0] = clamp(v, 0, 255);
							v = (sum[7] * xdb + sum[1] * db) * p[3] / 255;
							p[1] = clamp(v, 0, 255);
							v = (sum[8] * xdb + sum[2] * db) * p[3] / 255;
							p[2] = clamp(v, 0, 255);
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

void surface_filter_blur(struct surface_t * s, int radius)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);

	if(radius > 0)
		blur(surface_get_pixels(s), width, height, 0, 0, width, height, radius);
}
