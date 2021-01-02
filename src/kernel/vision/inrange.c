/*
 * kernel/vision/inrange.c
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

void vision_inrange_gray(struct vision_t * v, unsigned char l, unsigned char h)
{
	if(v && (v->type == VISION_TYPE_GRAY))
	{
		unsigned char * pgray = (unsigned char *)v->datas;
		for(int i = 0; i < v->npixel; i++, pgray++)
		{
			if((*pgray < l) || (*pgray > h))
				*pgray = 0;
		}
	}
}

void vision_inrange_rgb(struct vision_t * v, unsigned char * lrgb, unsigned char * hrgb)
{
	if(v && (v->type == VISION_TYPE_RGB))
	{
		unsigned char * pr = &((unsigned char *)v->datas)[v->npixel * 0];
		unsigned char * pg = &((unsigned char *)v->datas)[v->npixel * 1];
		unsigned char * pb = &((unsigned char *)v->datas)[v->npixel * 2];
		for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++)
		{
			if((*pr < lrgb[0]) || (*pr > hrgb[0]) || (*pg < lrgb[1]) || (*pg > hrgb[1]) || (*pb < lrgb[2]) || (*pb > hrgb[2]))
			{
				*pr = 0;
				*pg = 0;
				*pb = 0;
			}
		}
	}
}

void vision_inrange_hsv(struct vision_t * v, float * lhsv, float * hhsv)
{
	if(v && (v->type == VISION_TYPE_HSV))
	{
		float * ph = &((float *)v->datas)[v->npixel * 0];
		float * ps = &((float *)v->datas)[v->npixel * 1];
		float * pv = &((float *)v->datas)[v->npixel * 2];
		for(int i = 0; i < v->npixel; i++, ph++, ps++, pv++)
		{
			if((*ph < lhsv[0]) || (*ph > hhsv[0]) || (*ps < lhsv[1]) || (*ps > hhsv[1]) || (*pv < lhsv[2]) || (*pv > hhsv[2]))
			{
				*ph = 0.0f;
				*ps = 0.0f;
				*pv = 0.0f;
			}
		}
	}
}
