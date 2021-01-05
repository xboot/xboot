/*
 * kernel/vision/dither.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <vision/dither.h>

void vision_dither(struct vision_t * v)
{
	if(v && (v->type == VISION_TYPE_GRAY))
	{
		int width = vision_get_width(v);
		int height = vision_get_height(v);
		int w = width - 1;
		int h = height - 1;
		unsigned char * p, * q;
		int o, n, e;
		int x, y;

		for(y = 0, p = vision_get_datas(v); y < height; y++)
		{
			for(x = 0; x < width; x++, p++)
			{
				o = p[0];
				n = o > 127 ? 255 : 0;
				e = o - n;
				p[0] = n;
				if(x < w)
				{
					q = p + 1;
					o = clamp(q[0] + ((e * 7) >> 4), 0, 255);
					q[0] = o;
				}
				if(y < h)
				{
					if(x > 0)
					{
						q = p + width - 1;
						o = clamp(q[0] + ((e * 3) >> 4), 0, 255);
						q[0] = o;
					}
					q = p + width;
					o = clamp(q[0] + ((e * 5) >> 4), 0, 255);
					q[0] = o;
					if(x < w)
					{
						q = p + width + 1;
						o = clamp(q[0] + (e >> 4), 0, 255);
						q[0] = o;
					}
				}
			}
		}
	}
}
