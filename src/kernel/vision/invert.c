/*
 * kernel/vision/invert.c
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
#include <vision/vision.h>

void vision_invert(struct vision_t * v)
{
	if(v)
	{
		switch(v->type)
		{
		case VISION_TYPE_GRAY_F32:
			{
				float * pgray = &((float *)v->datas)[v->npixel * 0];
				for(int i = 0; i < v->npixel; i++, pgray++)
					*pgray = 1.0f - *pgray;
			}
			break;
		case VISION_TYPE_RGB_F32:
			{
				float * pr = &((float *)v->datas)[v->npixel * 0];
				float * pg = &((float *)v->datas)[v->npixel * 1];
				float * pb = &((float *)v->datas)[v->npixel * 2];
				for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++)
				{
					*pr = 1.0f - *pr;
					*pg = 1.0f - *pg;
					*pb = 1.0f - *pb;
				}
			}
			break;
		case VISION_TYPE_HSV_F32:
			{
				float * ph = &((float *)v->datas)[v->npixel * 0];
				float * ps = &((float *)v->datas)[v->npixel * 1];
				float * pv = &((float *)v->datas)[v->npixel * 2];
				for(int i = 0; i < v->npixel; ph++, ps++, pv++)
				{
					*ph = 1.0f - *ph;
					*ps = 1.0f - *ps;
					*pv = 1.0f - *pv;
				}
			}
			break;
		default:
			break;
		}
	}
}
