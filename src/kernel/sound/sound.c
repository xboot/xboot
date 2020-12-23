/*
 * kernel/sound/sound.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <sound/sound.h>

struct sound_t * sound_alloc(size_t length, void * priv)
{
	struct sound_t * s;
	void * datas;

	if((length <= 0) || ((length & 0x3) != 0))
		return NULL;

	s = malloc(sizeof(struct sound_t));
	if(!s)
		return NULL;

	datas = memalign(4, length);
	if(!datas)
	{
		free(s);
		return NULL;
	}

	init_list_head(&s->list);
	s->datas = datas;
	s->length = length;
	s->postion = 0;
	s->state = SOUND_STATE_STOPPED;
	s->lgain = 255;
	s->rgain = 255;
	s->loop = 0;
	s->priv = priv;

	return s;
}

void sound_free(struct sound_t * s)
{
	if(s)
	{
		free(s->datas);
		free(s);
	}
}
