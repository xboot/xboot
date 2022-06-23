/*
 * kernel/vision/gamma.c
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
#include <vision/gamma.h>

void vision_gamma(struct vision_t * v, float gamma)
{
	if(v)
	{
		if(v->type == VISION_TYPE_HSV)
		{
			float * pv = &((float *)v->datas)[v->npixel * 2];
			for(int i = 0; i < v->npixel; i++, pv++)
			{
				*pv = powf(*pv, gamma);
			}
		}
		else
		{
			unsigned char lut[256];
			for(int i = 0; i < 256; i++)
			{
				int t = powf((float)(i / 255.0), gamma) * 255.0;
				lut[i] = clamp(t, 0, 255);
			}
			switch(v->type)
			{
			case VISION_TYPE_GRAY:
				{
					unsigned char * pgray = (unsigned char *)v->datas;
					for(int i = 0; i < v->npixel; i++, pgray++)
						*pgray = lut[*pgray];
				}
				break;
			case VISION_TYPE_RGB:
				{
					unsigned char * pr = &((unsigned char *)v->datas)[v->npixel * 0];
					unsigned char * pg = &((unsigned char *)v->datas)[v->npixel * 1];
					unsigned char * pb = &((unsigned char *)v->datas)[v->npixel * 2];
					for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++)
					{
						*pr = lut[*pr];
						*pg = lut[*pg];
						*pb = lut[*pb];
					}
				}
				break;
			default:
				break;
			}
		}
	}
}
