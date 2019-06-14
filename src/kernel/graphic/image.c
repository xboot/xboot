/*
 * kernel/graphic/image.c
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
#include <graphic/image.h>

extern struct image_operate_t image_operate_cairo;

struct image_t * image_alloc(int width, int height)
{
	struct image_t * img;
	void * pixels;

	if(width < 0 || height < 0)
		return NULL;

	img = malloc(sizeof(struct image_t));
	if(!img)
		return NULL;

	pixels = memalign(4, height * (width << 2));
	if(!pixels)
	{
		free(img);
		return NULL;
	}

	img->width = width;
	img->height = height;
	img->stride = width << 2;
	img->pixels = pixels;
	img->op = &image_operate_cairo;
	img->priv = img->op->create(img);
	if(!img->priv)
	{
		free(img);
		free(pixels);
		return NULL;
	}
	return img;
}

void image_free(struct image_t * img)
{
	if(img)
	{
		img->op->destroy(img->priv);
		free(img->pixels);
		free(img);
	}
}
