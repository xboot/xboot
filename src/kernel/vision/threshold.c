/*
 * kernel/vision/threshold.c
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
#include <vision/threshold.h>

void vision_threshold(struct vision_t * v, int threshold, const char * type)
{
	if(v && (v->type == VISION_TYPE_GRAY))
	{
		if((threshold < 0) || (threshold > 255))
		{
			float tmp, variance = 0;
			float w0, w1, u0, u1;
			unsigned char * pgray;
			int minpos = 0;
			int maxpos = 255;
			int histogram[256];
			threshold = 128;
			memset(histogram, 0, sizeof(histogram));
			pgray = (unsigned char *)v->datas;
			for(int i = 0; i < v->npixel; i++, pgray++)
				histogram[*pgray]++;
			for(int i = 0; i < 255; i++)
			{
				if(histogram[i] != 0)
				{
					minpos = i;
					break;
				}
			}
			for(int i = 255; i > 0; i--)
			{
				if(histogram[i] != 0)
				{
					maxpos = i;
					break;
				}
			}
			for(int i = minpos; i <= maxpos; i++)
			{
				w1 = 0;
				u1 = 0;
				w0 = 0;
				u0 = 0;
				for(int j = 0; j <= i; j++)
				{
					w1 += histogram[j];
					u1 += j * histogram[j];
				}
				if(w1 == 0)
					break;
				u1 = u1 / w1;
				w1 = w1 / v->npixel;
				for(int k = i + 1; k < 255; k++)
				{
					w0 += histogram[k];
					u0 += k * histogram[k];
				}
				if(w0 == 0)
					break;
				u0 = u0 / w0;
				w0 = w0 / v->npixel;
				tmp = w0 * w1 * (u1 - u0) * (u1 - u0);
				if(variance < tmp)
				{
					variance = tmp;
					threshold = i;
				}
			}
		}
		switch(shash(type))
		{
		case 0xf4229cca: /* "binary" */
			{
				unsigned char * pgray = (unsigned char *)v->datas;
				for(int i = 0; i < v->npixel; i++, pgray++)
					*pgray = (*pgray > threshold) ? 255 : 0;
			}
			break;
		case 0xc880666f: /* "binary-invert" */
			{
				unsigned char * pgray = (unsigned char *)v->datas;
				for(int i = 0; i < v->npixel; i++, pgray++)
					*pgray = (*pgray > threshold) ? 0 : 255;
			}
			break;
		case 0x1e92b0a8: /* "tozero" */
			{
				unsigned char * pgray = (unsigned char *)v->datas;
				for(int i = 0; i < v->npixel; i++, pgray++)
				{
					if(*pgray <= threshold)
						*pgray = 0;
				}
			}
			break;
		case 0x98d3b48d: /* "tozero-invert" */
			{
				unsigned char * pgray = (unsigned char *)v->datas;
				for(int i = 0; i < v->npixel; i++, pgray++)
				{
					if(*pgray > threshold)
						*pgray = 0;
				}
			}
			break;
		case 0xe9e0dc6b: /* "truncate" */
			{
				unsigned char * pgray = (unsigned char *)v->datas;
				for(int i = 0; i < v->npixel; i++, pgray++)
				{
					if(*pgray > threshold)
						*pgray = threshold;
				}
			}
			break;
		default:
			break;
		}
	}
}
