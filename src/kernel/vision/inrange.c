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

void vision_inrange_gray(struct vision_t * v, float l, float h)
{
	if(v && (v->type == VISION_TYPE_GRAY_F32))
	{
		float * pgray = &((float *)v->datas)[v->npixel * 0];
		for(int i = 0; i < v->npixel; i++, pgray++)
		{
			if((*pgray < l) || (*pgray > h))
				*pgray = 0.0f;
		}
	}
}

void vision_inrange_rgb(struct vision_t * v, float lr, float lg, float lb, float hr, float hg, float hb)
{
	if(v && (v->type == VISION_TYPE_RGB_F32))
	{
		float * pr = &((float *)v->datas)[v->npixel * 0];
		float * pg = &((float *)v->datas)[v->npixel * 1];
		float * pb = &((float *)v->datas)[v->npixel * 2];
		for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++)
		{
			if((*pr < lr) || (*pr > hr) || (*pg < lg) || (*pg > hg) || (*pb < lb) || (*pb > hb))
			{
				*pr = 0.0f;
				*pg = 0.0f;
				*pb = 0.0f;
			}
		}
	}
}

void vision_inrange_hsv(struct vision_t * v, float lh, float ls, float lv, float hh, float hs, float hv)
{
	if(v && (v->type == VISION_TYPE_HSV_F32))
	{
		float * ph = &((float *)v->datas)[v->npixel * 0];
		float * ps = &((float *)v->datas)[v->npixel * 1];
		float * pv = &((float *)v->datas)[v->npixel * 2];
		for(int i = 0; i < v->npixel; i++, ph++, ps++, pv++)
		{
			if((*ph < lh) || (*ph > hh) || (*ps < ls) || (*ps > hs) || (*pv < lv) || (*pv > hv))
			{
				*ph = 0.0f;
				*ps = 0.0f;
				*pv = 0.0f;
			}
		}
	}
}
