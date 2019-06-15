/*
 * kernel/graphic/surface.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <graphic/surface.h>

extern struct surface_operate_t surface_operate_cairo;

struct surface_t * surface_alloc(int width, int height)
{
	struct surface_t * s;
	void * pixels;

	if(width < 0 || height < 0)
		return NULL;

	s = malloc(sizeof(struct surface_t));
	if(!s)
		return NULL;

	pixels = memalign(4, height * (width << 2));
	if(!pixels)
	{
		free(s);
		return NULL;
	}

	s->width = width;
	s->height = height;
	s->stride = width << 2;
	s->pixels = pixels;
	s->op = &surface_operate_cairo;
	s->priv = s->op->create(s);
	if(!s->priv)
	{
		free(s);
		free(pixels);
		return NULL;
	}
	return s;
}

void surface_free(struct surface_t * s)
{
	if(s)
	{
		s->op->destroy(s->priv);
		free(s->pixels);
		free(s);
	}
}
