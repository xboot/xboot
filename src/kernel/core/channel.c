/*
 * kernel/core/channel.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <xboot/channel.h>

#define min(x,y) ({			\
	typeof(x) _x = (x);		\
	typeof(y) _y = (y);		\
	(void)(&_x == &_y);		\
	_x < _y ? _x : _y; })

#define max(x,y) ({			\
	typeof(x) _x = (x);		\
	typeof(y) _y = (y);		\
	(void)(&_x == &_y);		\
	_x > _y ? _x : _y; })

struct channel_t * channel_alloc(unsigned int size)
{
	struct channel_t * c;

	c = malloc(sizeof(struct channel_t));
	if(!c)
		return NULL;

	if(size & (size - 1))
		size = roundup_pow_of_two(size);

	c->buffer = malloc(size);
	if(!c->buffer)
	{
		free(c);
		return NULL;
	}
	c->size = size;
	c->in = 0;
	c->out = 0;
	init_list_head(&c->swait);
	init_list_head(&c->rwait);
	spin_lock_init(&c->lock);

	return c;
}

void channel_free(struct channel_t * c)
{
	if(c)
	{
		free(c->buffer);
		free(c);
	}
}

static inline int channel_isempty(struct channel_t * c)
{
	int ret;

	spin_lock(&c->lock);
	ret = (c->in - c->out <= 0) ? 1 : 0;
	spin_unlock(&c->lock);

	return ret;
}

static inline int channel_isfull(struct channel_t * c)
{
	int ret;

	spin_lock(&c->lock);
	ret = (c->in - c->out >= c->size) ? 1 : 0;
	spin_unlock(&c->lock);

	return ret;
}

static inline unsigned int __channel_put(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	unsigned int l;

	spin_lock(&c->lock);
	len = min(len, c->size - c->in + c->out);
	smp_mb();
	l = min(len, c->size - (c->in & (c->size - 1)));
	memcpy(c->buffer + (c->in & (c->size - 1)), buf, l);
	memcpy(c->buffer, buf + l, len - l);
	smp_wmb();
	c->in += len;
	spin_unlock(&c->lock);

	return len;
}

static inline unsigned int __channel_get(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	unsigned int l;

	spin_lock(&c->lock);
	len = min(len, c->in - c->out);
	smp_rmb();
	l = min(len, c->size - (c->out & (c->size - 1)));
	memcpy(buf, c->buffer + (c->out & (c->size - 1)), l);
	memcpy(buf + l, c->buffer, len - l);
	smp_mb();
	c->out += len;
	spin_unlock(&c->lock);

	return len;
}

static inline unsigned int channel_put(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	struct task_t * self;
	struct task_t * pos, * n;
	unsigned int l;

	if(channel_isfull(c))
	{
		self = task_self();
		spin_lock(&c->lock);
		if(!list_empty(&c->rwait))
		{
			list_for_each_entry_safe(pos, n, &c->rwait, rlist)
			{
				list_del_init(&pos->rlist);
				task_resume(pos);
			}
		}
		if(list_empty_careful(&self->slist))
		{
			list_add_tail(&self->slist, &c->swait);
		}
		spin_unlock(&c->lock);
		task_suspend(self);
		l = __channel_put(c, buf, len);
	}
	else if(channel_isempty(c))
	{
		l = __channel_put(c, buf, len);
		spin_lock(&c->lock);
		if(!list_empty(&c->rwait))
		{
			list_for_each_entry_safe(pos, n, &c->rwait, rlist)
			{
				list_del_init(&pos->rlist);
				task_resume(pos);
			}
		}
		if(!list_empty(&c->swait))
		{
			list_for_each_entry_safe(pos, n, &c->swait, slist)
			{
				list_del_init(&pos->slist);
				task_resume(pos);
			}
		}
		spin_unlock(&c->lock);
	}
	else
	{
		l = __channel_put(c, buf, len);
		spin_lock(&c->lock);
		if(!list_empty(&c->rwait))
		{
			list_for_each_entry_safe(pos, n, &c->rwait, rlist)
			{
				list_del_init(&pos->rlist);
				task_resume(pos);
			}
		}
		spin_unlock(&c->lock);
	}
	return l;
}

static inline unsigned int channel_get(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	struct task_t * self;
	struct task_t * pos, * n;
	unsigned int l;

	if(channel_isempty(c))
	{
		self = task_self();
		spin_lock(&c->lock);
		if(!list_empty(&c->swait))
		{
			list_for_each_entry_safe(pos, n, &c->swait, slist)
			{
				list_del_init(&pos->slist);
				task_resume(pos);
			}
		}
		if(list_empty_careful(&self->rlist))
		{
			list_add_tail(&self->rlist, &c->rwait);
		}
		spin_unlock(&c->lock);
		task_suspend(self);
		l = __channel_get(c, buf, len);
	}
	else if(channel_isfull(c))
	{
		l = __channel_get(c, buf, len);
		spin_lock(&c->lock);
		if(!list_empty(&c->rwait))
		{
			list_for_each_entry_safe(pos, n, &c->rwait, rlist)
			{
				list_del_init(&pos->rlist);
				task_resume(pos);
			}
		}
		if(!list_empty(&c->swait))
		{
			list_for_each_entry_safe(pos, n, &c->swait, slist)
			{
				list_del_init(&pos->slist);
				task_resume(pos);
			}
		}
		spin_unlock(&c->lock);
	}
	else
	{
		l = __channel_get(c, buf, len);
		spin_lock(&c->lock);
		if(!list_empty(&c->swait))
		{
			list_for_each_entry_safe(pos, n, &c->swait, slist)
			{
				list_del_init(&pos->slist);
				task_resume(pos);
			}
		}
		spin_unlock(&c->lock);
	}
	return l;
}

void channel_send(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	unsigned int l = 0;

	if(c && buf)
	{
		do {
			l += channel_put(c, buf + l, len - l);
			task_yield();
		} while(l < len);
	}
}

void channel_recv(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	unsigned int l = 0;

	if(c && buf)
	{
		do {
			l += channel_get(c, buf + l, len - l);
			task_yield();
		} while(l < len);
	}
}
