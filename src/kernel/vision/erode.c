/*
 * kernel/vision/erode.c
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
#include <vision/erode.h>

void vision_erode(struct vision_t * v, int times)
{
	if(v && (v->type == VISION_TYPE_GRAY))
	{
		int width = vision_get_width(v);
		int height = vision_get_height(v);
		int ndata = vision_get_npixel(v);
		unsigned char * l, * t, * r, * b;
		unsigned char * lt, * rt, * lb, * rb;
		unsigned char * p, * q;
		unsigned char gray;
		void * datas;
		int m, n;

		datas = malloc(ndata);
		if(datas)
		{
			while(times-- > 0)
			{
				p = vision_get_datas(v);
				q = datas;
				memcpy(q, p, ndata);
				for(int y = 0; y < height; y++)
				{
					for(int x = 0; x < width; x++, p++, q++)
					{
						if(q[3] != 0)
						{
							m = (x - 1 > 0 ? 1 : x) << 2;
							n = (x + 1 < width ? 1 : width - x - 1) << 2;
							l = q - m;
							t = q - ((y - 1 > 0 ? 1 : y) * width);
							r = q + n;
							b = q + ((y + 1 < height ? 1 : height - y - 1) * width);
							lt = t - m;
							rt = t + n;
							lb = b - m;
							rb = b + n;
							gray = q[0];
							if(l[0] < gray)
								gray = l[0];
							if(t[0] < gray)
								gray = t[0];
							if(r[0] < gray)
								gray = r[0];
							if(b[0] < gray)
								gray = b[0];
							if(lt[0] < gray)
								gray = lt[0];
							if(rt[0] < gray)
								gray = rt[0];
							if(lb[0] < gray)
								gray = lb[0];
							if(rb[0] < gray)
								gray = rb[0];
							p[0] = gray;
						}
					}
				}
			}
			free(datas);
		}
	}
}
