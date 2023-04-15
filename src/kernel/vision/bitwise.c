/*
 * kernel/vision/bitwise.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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
#include <vision/bitwise.h>

void vision_bitwise_and(struct vision_t * v, struct vision_t * o)
{
	if(v && o)
	{
		int w = min(vision_get_width(v), vision_get_width(o));
		int h = min(vision_get_height(v), vision_get_height(o));
		switch(v->type)
		{
		case VISION_TYPE_GRAY:
			switch(o->type)
			{
			case VISION_TYPE_GRAY:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvgray = (unsigned char *)v->datas;
					unsigned char * pogray = (unsigned char *)o->datas;
					for(int y = 0; y < h; y++, pvgray += vstride, pogray += ostride)
					{
						for(int x = 0; x < w; x++)
							pvgray[x] &= pogray[x];
					}
				}
				break;
			case VISION_TYPE_RGB:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvgray = (unsigned char *)v->datas;
					unsigned char * por = &((unsigned char *)o->datas)[v->npixel * 0];
					unsigned char * pog = &((unsigned char *)o->datas)[v->npixel * 1];
					unsigned char * pob = &((unsigned char *)o->datas)[v->npixel * 2];
					for(int y = 0; y < h; y++, pvgray += vstride, por += ostride, pog += ostride, pob += ostride)
					{
						for(int x = 0; x < w; x++)
						{
							unsigned char gray = (por[x] * 19595L + pog[x] * 38469L + pob[x] * 7472L) >> 16;
							pvgray[x] &= gray;
						}
					}
				}
				break;
			default:
				break;
			}
			break;
		case VISION_TYPE_RGB:
			switch(o->type)
			{
			case VISION_TYPE_GRAY:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvr = &((unsigned char *)v->datas)[v->npixel * 0];
					unsigned char * pvg = &((unsigned char *)v->datas)[v->npixel * 1];
					unsigned char * pvb = &((unsigned char *)v->datas)[v->npixel * 2];
					unsigned char * pogray = (unsigned char *)o->datas;
					for(int y = 0; y < h; y++, pvr += vstride, pvg += vstride, pvb += vstride, pogray += ostride)
					{
						for(int x = 0; x < w; x++)
						{
							unsigned char gray = pogray[x];
							pvr[x] &= gray;
							pvg[x] &= gray;
							pvb[x] &= gray;
						}
					}
				}
				break;
			case VISION_TYPE_RGB:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvr = &((unsigned char *)v->datas)[v->npixel * 0];
					unsigned char * pvg = &((unsigned char *)v->datas)[v->npixel * 1];
					unsigned char * pvb = &((unsigned char *)v->datas)[v->npixel * 2];
					unsigned char * por = &((unsigned char *)o->datas)[v->npixel * 0];
					unsigned char * pog = &((unsigned char *)o->datas)[v->npixel * 1];
					unsigned char * pob = &((unsigned char *)o->datas)[v->npixel * 2];
					for(int y = 0; y < h; y++, pvr += vstride, pvg += vstride, pvb += vstride, por += ostride, pog += ostride, pob += ostride)
					{
						for(int x = 0; x < w; x++)
						{
							pvr[x] &= por[x];
							pvg[x] &= pog[x];
							pvb[x] &= pob[x];
						}
					}
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void vision_bitwise_or(struct vision_t * v, struct vision_t * o)
{
	if(v && o)
	{
		int w = min(vision_get_width(v), vision_get_width(o));
		int h = min(vision_get_height(v), vision_get_height(o));
		switch(v->type)
		{
		case VISION_TYPE_GRAY:
			switch(o->type)
			{
			case VISION_TYPE_GRAY:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvgray = (unsigned char *)v->datas;
					unsigned char * pogray = (unsigned char *)o->datas;
					for(int y = 0; y < h; y++, pvgray += vstride, pogray += ostride)
					{
						for(int x = 0; x < w; x++)
							pvgray[x] |= pogray[x];
					}
				}
				break;
			case VISION_TYPE_RGB:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvgray = (unsigned char *)v->datas;
					unsigned char * por = &((unsigned char *)o->datas)[v->npixel * 0];
					unsigned char * pog = &((unsigned char *)o->datas)[v->npixel * 1];
					unsigned char * pob = &((unsigned char *)o->datas)[v->npixel * 2];
					for(int y = 0; y < h; y++, pvgray += vstride, por += ostride, pog += ostride, pob += ostride)
					{
						for(int x = 0; x < w; x++)
						{
							unsigned char gray = (por[x] * 19595L + pog[x] * 38469L + pob[x] * 7472L) >> 16;
							pvgray[x] |= gray;
						}
					}
				}
				break;
			default:
				break;
			}
			break;
		case VISION_TYPE_RGB:
			switch(o->type)
			{
			case VISION_TYPE_GRAY:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvr = &((unsigned char *)v->datas)[v->npixel * 0];
					unsigned char * pvg = &((unsigned char *)v->datas)[v->npixel * 1];
					unsigned char * pvb = &((unsigned char *)v->datas)[v->npixel * 2];
					unsigned char * pogray = (unsigned char *)o->datas;
					for(int y = 0; y < h; y++, pvr += vstride, pvg += vstride, pvb += vstride, pogray += ostride)
					{
						for(int x = 0; x < w; x++)
						{
							unsigned char gray = pogray[x];
							pvr[x] |= gray;
							pvg[x] |= gray;
							pvb[x] |= gray;
						}
					}
				}
				break;
			case VISION_TYPE_RGB:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvr = &((unsigned char *)v->datas)[v->npixel * 0];
					unsigned char * pvg = &((unsigned char *)v->datas)[v->npixel * 1];
					unsigned char * pvb = &((unsigned char *)v->datas)[v->npixel * 2];
					unsigned char * por = &((unsigned char *)o->datas)[v->npixel * 0];
					unsigned char * pog = &((unsigned char *)o->datas)[v->npixel * 1];
					unsigned char * pob = &((unsigned char *)o->datas)[v->npixel * 2];
					for(int y = 0; y < h; y++, pvr += vstride, pvg += vstride, pvb += vstride, por += ostride, pog += ostride, pob += ostride)
					{
						for(int x = 0; x < w; x++)
						{
							pvr[x] |= por[x];
							pvg[x] |= pog[x];
							pvb[x] |= pob[x];
						}
					}
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void vision_bitwise_xor(struct vision_t * v, struct vision_t * o)
{
	if(v && o)
	{
		int w = min(vision_get_width(v), vision_get_width(o));
		int h = min(vision_get_height(v), vision_get_height(o));
		switch(v->type)
		{
		case VISION_TYPE_GRAY:
			switch(o->type)
			{
			case VISION_TYPE_GRAY:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvgray = (unsigned char *)v->datas;
					unsigned char * pogray = (unsigned char *)o->datas;
					for(int y = 0; y < h; y++, pvgray += vstride, pogray += ostride)
					{
						for(int x = 0; x < w; x++)
							pvgray[x] ^= pogray[x];
					}
				}
				break;
			case VISION_TYPE_RGB:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvgray = (unsigned char *)v->datas;
					unsigned char * por = &((unsigned char *)o->datas)[v->npixel * 0];
					unsigned char * pog = &((unsigned char *)o->datas)[v->npixel * 1];
					unsigned char * pob = &((unsigned char *)o->datas)[v->npixel * 2];
					for(int y = 0; y < h; y++, pvgray += vstride, por += ostride, pog += ostride, pob += ostride)
					{
						for(int x = 0; x < w; x++)
						{
							unsigned char gray = (por[x] * 19595L + pog[x] * 38469L + pob[x] * 7472L) >> 16;
							pvgray[x] ^= gray;
						}
					}
				}
				break;
			default:
				break;
			}
			break;
		case VISION_TYPE_RGB:
			switch(o->type)
			{
			case VISION_TYPE_GRAY:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvr = &((unsigned char *)v->datas)[v->npixel * 0];
					unsigned char * pvg = &((unsigned char *)v->datas)[v->npixel * 1];
					unsigned char * pvb = &((unsigned char *)v->datas)[v->npixel * 2];
					unsigned char * pogray = (unsigned char *)o->datas;
					for(int y = 0; y < h; y++, pvr += vstride, pvg += vstride, pvb += vstride, pogray += ostride)
					{
						for(int x = 0; x < w; x++)
						{
							unsigned char gray = pogray[x];
							pvr[x] ^= gray;
							pvg[x] ^= gray;
							pvb[x] ^= gray;
						}
					}
				}
				break;
			case VISION_TYPE_RGB:
				{
					int vstride = v->width;
					int ostride = o->width;
					unsigned char * pvr = &((unsigned char *)v->datas)[v->npixel * 0];
					unsigned char * pvg = &((unsigned char *)v->datas)[v->npixel * 1];
					unsigned char * pvb = &((unsigned char *)v->datas)[v->npixel * 2];
					unsigned char * por = &((unsigned char *)o->datas)[v->npixel * 0];
					unsigned char * pog = &((unsigned char *)o->datas)[v->npixel * 1];
					unsigned char * pob = &((unsigned char *)o->datas)[v->npixel * 2];
					for(int y = 0; y < h; y++, pvr += vstride, pvg += vstride, pvb += vstride, por += ostride, pog += ostride, pob += ostride)
					{
						for(int x = 0; x < w; x++)
						{
							pvr[x] ^= por[x];
							pvg[x] ^= pog[x];
							pvb[x] ^= pob[x];
						}
					}
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void vision_bitwise_not(struct vision_t * v)
{
	if(v)
	{
		switch(v->type)
		{
		case VISION_TYPE_GRAY:
			{
				unsigned char * pgray = (unsigned char *)v->datas;
				for(int i = 0; i < v->npixel; i++, pgray++)
					*pgray = ~*pgray;
			}
			break;
		case VISION_TYPE_RGB:
			{
				unsigned char * pr = &((unsigned char *)v->datas)[v->npixel * 0];
				unsigned char * pg = &((unsigned char *)v->datas)[v->npixel * 1];
				unsigned char * pb = &((unsigned char *)v->datas)[v->npixel * 2];
				for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++)
				{
					*pr = ~*pr;
					*pg = ~*pg;
					*pb = ~*pb;
				}
			}
			break;
		default:
			break;
		}
	}
}
