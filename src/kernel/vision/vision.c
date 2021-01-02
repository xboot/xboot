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
	float * datas;
	int npixel;
	size_t ndata;

	if((width <= 0) || (height <= 0))
		return NULL;

	v = malloc(sizeof(struct vision_t));
	if(!v)
		return NULL;

	npixel = width * height;
	ndata = ((v->type >> 8) & 0xff) * npixel * sizeof(float);
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

void vision_convert(struct vision_t * v, enum vision_type_t type)
{
	if(v)
	{
		switch(type)
		{
		case VISION_TYPE_GRAY:
			switch(v->type)
			{
			case VISION_TYPE_GRAY:
				break;
			case VISION_TYPE_RGB:
				{
					float * pr = &v->datas[v->npixel * 0];
					float * pg = &v->datas[v->npixel * 1];
					float * pb = &v->datas[v->npixel * 2];
					for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++)
						*pr = (*pr) * 0.299f + (*pg) * 0.587f + (*pb) * 0.114f;
					v->type = VISION_TYPE_GRAY;
				}
				break;
			case VISION_TYPE_HSV:
				{
					float * ph = &v->datas[v->npixel * 0];
					float * pv = &v->datas[v->npixel * 2];
					for(int i = 0; i < v->npixel; i++, ph++, pv++)
						*ph = *pv;
					v->type = VISION_TYPE_GRAY;
				}
				break;
			default:
				break;
			}
			break;
		case VISION_TYPE_RGB:
			switch(v->type)
			{
			case VISION_TYPE_GRAY:
				{
					size_t ndata = 3 * v->npixel * sizeof(float);
					if(v->ndata < ndata)
					{
						v->datas = realloc(v->datas, ndata);
						v->ndata = ndata;
					}
					float * pr = &v->datas[v->npixel * 0];
					float * pg = &v->datas[v->npixel * 1];
					float * pb = &v->datas[v->npixel * 2];
					for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++)
						*pb = *pg = *pr;
					v->type = VISION_TYPE_RGB;
				}
				break;
			case VISION_TYPE_RGB:
				break;
			case VISION_TYPE_HSV:
				{
					float * ph = &v->datas[v->npixel * 0];
					float * ps = &v->datas[v->npixel * 1];
					float * pv = &v->datas[v->npixel * 2];
					float hv, sv, vv;
					float p, q, t, f;
					for(int i = 0; i < v->npixel; i++, ph++, ps++, pv++)
					{
						sv = *ps;
						vv = *pv;
						if(sv <= 0.0f)
							*ph = *ps = vv;
						else
						{
							hv = *ph / (60.0f / 360.0f);
							i = (int)hv;
							f = hv - (float)i;
							p = vv * (1.0f - sv);
							q = vv * (1.0f - (sv * f));
							t = vv * (1.0f - sv * (1.0f - f));
							switch(i)
							{
							case 0: *ph = vv; *ps = t; *pv = p; break;
							case 1: *ph = q; *ps = vv; *pv = p; break;
							case 2: *ph = p; *ps = vv; *pv = t; break;
							case 3: *ph = p; *ps = q; *pv = vv; break;
							case 4: *ph = t; *ps = p; *pv = vv; break;
							case 5: *ph = vv; *ps = p; *pv = q; break;
							default: *ph = vv; *ps = t; *pv = p; break;
							}
						}
					}
					v->type = VISION_TYPE_RGB;
				}
				break;
			default:
				break;
			}
			break;
		case VISION_TYPE_HSV:
			switch(v->type)
			{
			case VISION_TYPE_GRAY:
				{
					size_t ndata = 3 * v->npixel * sizeof(float);
					if(v->ndata < ndata)
					{
						v->datas = realloc(v->datas, ndata);
						v->ndata = ndata;
					}
					float * ph = &v->datas[v->npixel * 0];
					float * ps = &v->datas[v->npixel * 1];
					float * pv = &v->datas[v->npixel * 2];
					for(int i = 0; i < v->npixel; i++, ph++, ps++, pv++)
					{
						*ps = 0;
						*pv = *ph;
						*ph = 0;
					}
					v->type = VISION_TYPE_HSV;
				}
				break;
			case VISION_TYPE_RGB:
				{
					float * pr = &v->datas[v->npixel * 0];
					float * pg = &v->datas[v->npixel * 1];
					float * pb = &v->datas[v->npixel * 2];
					float r, g, b;
					float k = 0.0f;
					float chroma;
					float t;
					for(int i = 0; i < v->npixel; i++, pr++, pg++, pb++)
					{
						r = *pr;
						g = *pg;
						b = *pb;
						if(g < b)
						{
							t = g;
							g = b;
							b = t;
							k = -1.f;
						}
						if(r < g)
						{
							t = r;
							r = g;
							g = t;
							k = -2.f / 6.0f - k;
						}
						chroma = r - ((g < b) ? g : b);
						*pr = fabsf(k + (g - b) / (6.0f * chroma + 1e-20f));
						*pg = chroma / (r + 1e-20f);
						*pb = r;
					}
					v->type = VISION_TYPE_HSV;
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
		memset(v->datas, 0, v->ndata);
}

void vision_set_pixel(struct vision_t * v, int x, int y, int c, float px)
{
	if(v && (x < v->width) && (y < v->height) && (c < vision_get_channel(v)))
		v->datas[v->npixel * c + y * v->width + x] = px;
}

float vision_get_pixel(struct vision_t * v, int x, int y, int c)
{
	if(v && (x < v->width) && (y < v->height) && (c < vision_get_channel(v)))
		return v->datas[v->npixel * c + y * v->width + x];
	return 0;
}

struct vision_t * vision_alloc_from_surface(struct surface_t * s)
{
	struct vision_t * v;

	if(s && (v = vision_alloc(VISION_TYPE_RGB, surface_get_width(s), surface_get_height(s))))
	{
		unsigned char * p = surface_get_pixels(s);
		float * pr = &v->datas[v->npixel * 0];
		float * pg = &v->datas[v->npixel * 1];
		float * pb = &v->datas[v->npixel * 2];
		for(int i = 0; i < v->npixel; i++, p += 4, pr++, pg++, pb++)
		{
			if(p[3] != 0)
			{
				*pr = (float)p[2] / (float)p[3];
				*pg = (float)p[1] / (float)p[3];
				*pb = (float)p[0] / (float)p[3];
			}
			else
			{
				*pr = 0.0f;
				*pg = 0.0f;
				*pb = 0.0f;
			}
		}
		return v;
	}
	return NULL;
}

void vision_copyto_surface(struct vision_t * v, struct surface_t * s)
{
	if(v && s && (v->width == s->width) && (v->height == s->height))
	{
		switch(v->type)
		{
		case VISION_TYPE_GRAY:
			{
				unsigned char * p = surface_get_pixels(s);
				float * pgray = &v->datas[v->npixel * 0];
				for(int i = 0; i < v->npixel; i++, p += 4, pgray++)
				{
					p[3] = 255;
					p[2] = p[1] = p[0] = (unsigned char)(*pgray * 255.0f);
				}
			}
			break;
		case VISION_TYPE_RGB:
			{
				unsigned char * p = surface_get_pixels(s);
				float * pr = &v->datas[v->npixel * 0];
				float * pg = &v->datas[v->npixel * 1];
				float * pb = &v->datas[v->npixel * 2];
				for(int i = 0; i < v->npixel; i++, p += 4, pr++, pg++, pb++)
				{
					p[3] = 255;
					p[2] = (unsigned char)(*pr * 255.0f);
					p[1] = (unsigned char)(*pg * 255.0f);
					p[0] = (unsigned char)(*pb * 255.0f);
				}
			}
			break;
		case VISION_TYPE_HSV:
			{
				unsigned char * p = surface_get_pixels(s);
				float * ph = &v->datas[v->npixel * 0];
				float * ps = &v->datas[v->npixel * 1];
				float * pv = &v->datas[v->npixel * 2];
				struct color_t c;
				for(int i = 0; i < v->npixel; p += 4, ph++, ps++, pv++)
				{
					color_set_hsva(&c, *ph, *ps, *pv, 1.0f);
					p[3] = c.a;
					p[2] = c.r;
					p[1] = c.g;
					p[0] = c.b;
				}
			}
			break;
		default:
			break;
		}
	}
}
