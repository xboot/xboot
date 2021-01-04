/*
 * kernel/vision/vision.c
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

struct vision_t * vision_alloc(enum vision_type_t type, int width, int height)
{
	struct vision_t * v;
	void * datas;
	size_t ndata;
	int npixel;

	if((width <= 0) || (height <= 0))
		return NULL;

	v = malloc(sizeof(struct vision_t));
	if(!v)
		return NULL;

	npixel = width * height;
	ndata = npixel * vision_type_get_bytes(type) * vision_type_get_channels(type);
	datas = malloc(ndata);
	if(!datas)
	{
		free(v);
		return NULL;
	}

	v->type = type;
	v->width = width;
	v->height = height;
	v->npixel = npixel;
	v->datas = datas;
	v->ndata = ndata;
	return v;
}

void vision_free(struct vision_t * v)
{
	if(v)
	{
		if(v->datas)
			free(v->datas);
		free(v);
	}
}

struct vision_t * vision_clone(struct vision_t * v, int x, int y, int w, int h)
{
	if(v)
	{
		if((w <= 0) || (h <= 0))
		{
			struct vision_t * o = vision_alloc(v->type, v->width, v->height);
			if(o)
			{
				memcpy(o->datas, v->datas, o->ndata);
				return o;
			}
		}
		else
		{
			int x1, y1, x2, y2;
			x1 = max(0, x);
			x2 = min(v->width, x + w);
			if(x1 <= x2)
			{
				y1 = max(0, y);
				y2 = min(v->height, y + h);
				if(y1 <= y2)
				{
					int width = x2 - x1;
					int height = y2 - y1;
					struct vision_t * o = vision_alloc(v->type, width, height);
					if(o)
					{
						switch(o->type)
						{
						case VISION_TYPE_GRAY:
							{
								int ostride = o->width;
								int vstride = v->width;
								int len = ostride * vision_type_get_bytes(VISION_TYPE_GRAY);
								unsigned char * po = (unsigned char *)o->datas;
								unsigned char * pv = &((unsigned char *)v->datas)[y1 * v->width + x1];
								for(int i = 0; i < height; i++, po += ostride, pv += vstride)
									memcpy(po, pv, len);
							}
							break;
						case VISION_TYPE_RGB:
							{
								int ostride = o->width;
								int vstride = v->width;
								int len = ostride * vision_type_get_bytes(VISION_TYPE_RGB);
								unsigned char * po, * pv;

								po = &((unsigned char *)o->datas)[o->npixel * 0];
								pv = &((unsigned char *)v->datas)[v->npixel * 0 + y1 * v->width + x1];
								for(int i = 0; i < height; i++, po += ostride, pv += vstride)
									memcpy(po, pv, len);

								po = &((unsigned char *)o->datas)[o->npixel * 1];
								pv = &((unsigned char *)v->datas)[v->npixel * 1 + y1 * v->width + x1];
								for(int i = 0; i < height; i++, po += ostride, pv += vstride)
									memcpy(po, pv, len);

								po = &((unsigned char *)o->datas)[o->npixel * 2];
								pv = &((unsigned char *)v->datas)[v->npixel * 2 + y1 * v->width + x1];
								for(int i = 0; i < height; i++, po += ostride, pv += vstride)
									memcpy(po, pv, len);
							}
							break;
						case VISION_TYPE_HSV:
							{
								int ostride = o->width;
								int vstride = v->width;
								int len = ostride * vision_type_get_bytes(VISION_TYPE_HSV);
								float * po, * pv;

								po = &((float *)o->datas)[o->npixel * 0];
								pv = &((float *)v->datas)[v->npixel * 0 + y1 * v->width + x1];
								for(int i = 0; i < height; i++, po += ostride, pv += vstride)
									memcpy(po, pv, len);

								po = &((float *)o->datas)[o->npixel * 1];
								pv = &((float *)v->datas)[v->npixel * 1 + y1 * v->width + x1];
								for(int i = 0; i < height; i++, po += ostride, pv += vstride)
									memcpy(po, pv, len);

								po = &((float *)o->datas)[o->npixel * 2];
								pv = &((float *)v->datas)[v->npixel * 2 + y1 * v->width + x1];
								for(int i = 0; i < height; i++, po += ostride, pv += vstride)
									memcpy(po, pv, len);
							}
							break;
						default:
							break;
						}
						return o;
					}
				}
			}
		}
	}
	return NULL;
}

void vision_convert(struct vision_t * v, enum vision_type_t type)
{
	if(v)
	{
		switch(v->type)
		{
		case VISION_TYPE_GRAY:
			switch(type)
			{
			case VISION_TYPE_GRAY:
				break;
			case VISION_TYPE_RGB:
				{
					size_t ndata = v->npixel * vision_type_get_bytes(VISION_TYPE_RGB) * vision_type_get_channels(VISION_TYPE_RGB);
					void * datas = malloc(ndata);
					if(datas)
					{
						unsigned char * pr = &((unsigned char *)datas)[v->npixel * 0];
						unsigned char * pg = &((unsigned char *)datas)[v->npixel * 1];
						unsigned char * pb = &((unsigned char *)datas)[v->npixel * 2];
						unsigned char * pgray = (unsigned char *)v->datas;
						for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++, pgray++)
							*pb = *pg = *pr = *pgray;
						if(v->datas)
							free(v->datas);
						v->datas = datas;
						v->ndata = ndata;
						v->type = VISION_TYPE_RGB;
					}
				}
				break;
			case VISION_TYPE_HSV:
				{
					size_t ndata = v->npixel * vision_type_get_bytes(VISION_TYPE_HSV) * vision_type_get_channels(VISION_TYPE_HSV);
					void * datas = malloc(ndata);
					if(datas)
					{
						float * ph = &((float *)datas)[v->npixel * 0];
						float * ps = &((float *)datas)[v->npixel * 1];
						float * pv = &((float *)datas)[v->npixel * 2];
						unsigned char * pgray = (unsigned char *)v->datas;
						struct color_t c;
						for(int i = 0; i < v->npixel; i++, ph++, ps++, pv++, pgray++)
						{
							c.r = c.g = c.b = *pgray;
							color_get_hsva(&c, ph, ps, pv, NULL);
						}
						if(v->datas)
							free(v->datas);
						v->datas = datas;
						v->ndata = ndata;
						v->type = VISION_TYPE_HSV;
					}
				}
				break;
			default:
				break;
			}
			break;
		case VISION_TYPE_RGB:
			switch(type)
			{
			case VISION_TYPE_GRAY:
				{
					unsigned char * pr = &((unsigned char *)v->datas)[v->npixel * 0];
					unsigned char * pg = &((unsigned char *)v->datas)[v->npixel * 1];
					unsigned char * pb = &((unsigned char *)v->datas)[v->npixel * 2];
					for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++)
						*pr = ((*pr) * 19595L + (*pg) * 38469L + (*pb) * 7472L) >> 16;
					v->type = VISION_TYPE_GRAY;
				}
				break;
			case VISION_TYPE_RGB:
				break;
			case VISION_TYPE_HSV:
				{
					size_t ndata = v->npixel * vision_type_get_bytes(VISION_TYPE_HSV) * vision_type_get_channels(VISION_TYPE_HSV);
					void * datas = malloc(ndata);
					if(datas)
					{
						float * ph = &((float *)datas)[v->npixel * 0];
						float * ps = &((float *)datas)[v->npixel * 1];
						float * pv = &((float *)datas)[v->npixel * 2];
						unsigned char * pr = &((unsigned char *)v->datas)[v->npixel * 0];
						unsigned char * pg = &((unsigned char *)v->datas)[v->npixel * 1];
						unsigned char * pb = &((unsigned char *)v->datas)[v->npixel * 2];
						struct color_t c;
						for(int i = 0; i < v->npixel; i++, ph++, ps++, pv++, pr++, pg++, pb++)
						{
							c.r = *pr;
							c.g = *pg;
							c.b = *pb;
							color_get_hsva(&c, ph, ps, pv, NULL);
						}
						if(v->datas)
							free(v->datas);
						v->datas = datas;
						v->ndata = ndata;
						v->type = VISION_TYPE_HSV;
					}
				}
				break;
			default:
				break;
			}
			break;
		case VISION_TYPE_HSV:
			switch(type)
			{
			case VISION_TYPE_GRAY:
				{
					unsigned char * pgray = (unsigned char *)v->datas;
					float * ph = &((float *)v->datas)[v->npixel * 0];
					float * ps = &((float *)v->datas)[v->npixel * 1];
					float * pv = &((float *)v->datas)[v->npixel * 2];
					struct color_t c;
					for(int i = 0; i < v->npixel; i++, pgray++, ph++, ps++, pv++)
					{
						color_set_hsva(&c, *ph, *ps, *pv, 1.0f);
						*pgray = (c.r * 19595L + c.g * 38469L + c.b * 7472L) >> 16;
					}
					v->type = VISION_TYPE_GRAY;
				}
				break;
			case VISION_TYPE_RGB:
				{
					size_t ndata = v->npixel * vision_type_get_bytes(VISION_TYPE_RGB) * vision_type_get_channels(VISION_TYPE_RGB);
					void * datas = malloc(ndata);
					if(datas)
					{
						unsigned char * pr = &((unsigned char *)datas)[v->npixel * 0];
						unsigned char * pg = &((unsigned char *)datas)[v->npixel * 1];
						unsigned char * pb = &((unsigned char *)datas)[v->npixel * 2];
						float * ph = &((float *)v->datas)[v->npixel * 0];
						float * ps = &((float *)v->datas)[v->npixel * 1];
						float * pv = &((float *)v->datas)[v->npixel * 2];
						struct color_t c;
						for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++, ph++, ps++, pv++)
						{
							color_set_hsva(&c, *ph, *ps, *pv, 1.0f);
							*pr = c.r;
							*pg = c.g;
							*pb = c.b;
						}
						if(v->datas)
							free(v->datas);
						v->datas = datas;
						v->ndata = ndata;
						v->type = VISION_TYPE_RGB;
					}
				}
				break;
			case VISION_TYPE_HSV:
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

void vision_clear(struct vision_t * v)
{
	if(v)
		memset(v->datas, 0, v->npixel * vision_type_get_bytes(v->type) * vision_type_get_channels(v->type));
}

void vision_apply_surface(struct vision_t * v, struct surface_t * s)
{
	if(v && s)
	{
		int w = min(vision_get_width(v), surface_get_width(s));
		int h = min(vision_get_height(v), surface_get_height(s));
		switch(v->type)
		{
		case VISION_TYPE_GRAY:
			{
				unsigned char * pgray, * qgray = &((unsigned char *)v->datas)[v->npixel * 0];
				unsigned char * p, * q = surface_get_pixels(s);
				int vstride = v->width;
				int sstride = s->stride;
				int x, y;
				for(y = 0; y < h; y++, qgray += vstride, q += sstride)
				{
					for(x = 0, pgray = qgray, p = q; x < w; x++, pgray++, p += 4)
					{
						if(p[3] != 0)
						{
							if(p[3] == 255)
								*pgray = (p[2] * 19595L + p[1] * 38469L + p[0] * 7472L) >> 16;
							else
								*pgray = ((p[2] * 19595L + p[1] * 38469L + p[0] * 7472L) >> 16) * 255 / p[3];
						}
						else
							*pgray = 0;
					}
				}
			}
			break;
		case VISION_TYPE_RGB:
			{
				unsigned char * pr, * qr = &((unsigned char *)v->datas)[v->npixel * 0];
				unsigned char * pg, * qg = &((unsigned char *)v->datas)[v->npixel * 1];
				unsigned char * pb, * qb = &((unsigned char *)v->datas)[v->npixel * 2];
				unsigned char * p, * q = surface_get_pixels(s);
				int vstride = v->width;
				int sstride = s->stride;
				int x, y;
				for(y = 0; y < h; y++, qr += vstride, qg += vstride, qb += vstride, q += sstride)
				{
					for(x = 0, pr = qr, pg = qg, pb = qb, p = q; x < w; x++, pr++, pg++, pb++, p += 4)
					{
						if(p[3] != 0)
						{
							if(p[3] == 255)
							{
								*pr = p[2];
								*pg = p[1];
								*pb = p[0];
							}
							else
							{
								*pr = p[2] * 255 / p[3];
								*pg = p[1] * 255 / p[3];
								*pb = p[0] * 255 / p[3];
							}
						}
						else
						{
							*pr = 0;
							*pg = 0;
							*pb = 0;
						}
					}
				}
			}
			break;
		case VISION_TYPE_HSV:
			{
				float * ph, * qh = &((float *)v->datas)[v->npixel * 0];
				float * ps, * qs = &((float *)v->datas)[v->npixel * 1];
				float * pv, * qv = &((float *)v->datas)[v->npixel * 2];
				unsigned char * p, * q = surface_get_pixels(s);
				int vstride = v->width;
				int sstride = s->stride;
				int x, y;
				struct color_t c;
				for(y = 0; y < h; y++, qh += vstride, qs += vstride, qv += vstride, q += sstride)
				{
					for(x = 0, ph = qh, ps = qs, pv = qv, p = q; x < w; x++, ph++, ps++, pv++, p += 4)
					{
						if(p[3] != 0)
						{
							if(p[3] == 255)
							{
								c.r = p[2];
								c.g = p[1];
								c.b = p[0];
							}
							else
							{
								c.r = p[2] * 255 / p[3];
								c.g = p[1] * 255 / p[3];
								c.b = p[0] * 255 / p[3];
							}
							color_get_hsva(&c, ph, ps, pv, NULL);
						}
						else
						{
							*ph = 0.0f;
							*ps = 0.0f;
							*pv = 0.0f;
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

void surface_apply_vision(struct surface_t * s, struct vision_t * v)
{
	if(s && v)
	{
		int w = min(surface_get_width(s), vision_get_width(v));
		int h = min(surface_get_height(s), vision_get_height(v));
		switch(v->type)
		{
		case VISION_TYPE_GRAY:
			{
				unsigned char * pgray, * qgray = &((unsigned char *)v->datas)[v->npixel * 0];
				unsigned char * p, * q = surface_get_pixels(s);
				int vstride = v->width;
				int sstride = s->stride;
				int x, y;
				for(y = 0; y < h; y++, qgray += vstride, q += sstride)
				{
					for(x = 0, pgray = qgray, p = q; x < w; x++, pgray++, p += 4)
					{
						p[3] = 255;
						p[2] = p[1] = p[0] = *pgray;
					}
				}
			}
			break;
		case VISION_TYPE_RGB:
			{
				unsigned char * pr, * qr = &((unsigned char *)v->datas)[v->npixel * 0];
				unsigned char * pg, * qg = &((unsigned char *)v->datas)[v->npixel * 1];
				unsigned char * pb, * qb = &((unsigned char *)v->datas)[v->npixel * 2];
				unsigned char * p, * q = surface_get_pixels(s);
				int vstride = v->width;
				int sstride = s->stride;
				int x, y;
				for(y = 0; y < h; y++, qr += vstride, qg += vstride, qb += vstride, q += sstride)
				{
					for(x = 0, pr = qr, pg = qg, pb = qb, p = q; x < w; x++, pr++, pg++, pb++, p += 4)
					{
						p[3] = 255;
						p[2] = *pr;
						p[1] = *pg;
						p[0] = *pb;
					}
				}
			}
			break;
		case VISION_TYPE_HSV:
			{
				float * ph, * qh = &((float *)v->datas)[v->npixel * 0];
				float * ps, * qs = &((float *)v->datas)[v->npixel * 1];
				float * pv, * qv = &((float *)v->datas)[v->npixel * 2];
				unsigned char * p, * q = surface_get_pixels(s);
				int vstride = v->width;
				int sstride = s->stride;
				int x, y;
				struct color_t c;
				for(y = 0; y < h; y++, qh += vstride, qs += vstride, qv += vstride, q += sstride)
				{
					for(x = 0, ph = qh, ps = qs, pv = qv, p = q; x < w; x++, ph++, ps++, pv++, p += 4)
					{
						color_set_hsva(&c, *ph, *ps, *pv, 1.0f);
						p[3] = 255;
						p[2] = c.r;
						p[1] = c.g;
						p[0] = c.b;
					}
				}
			}
			break;
		default:
			break;
		}
	}
}
