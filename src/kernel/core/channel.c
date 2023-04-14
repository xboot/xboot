/*
 * kernel/core/channel.c
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
#include <xboot/channel.h>

struct channel_t * channel_alloc(unsigned int size)
{
	struct channel_t * c;

	c = malloc(sizeof(struct channel_t));
	if(!c)
		return NULL;

	if(size < 16)
		size = 16;
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
	return (c->in - c->out <= 0) ? 1 : 0;
}

static inline int channel_isfull(struct channel_t * c)
{
	return (c->in - c->out >= c->size) ? 1 : 0;
}

static inline unsigned int __channel_put(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	unsigned int l;

	len = min(len, c->size - c->in + c->out);
	smp_mb();
	l = min(len, c->size - (c->in & (c->size - 1)));
	memcpy(c->buffer + (c->in & (c->size - 1)), buf, l);
	memcpy(c->buffer, buf + l, len - l);
	smp_wmb();
	c->in += len;

	return len;
}

static inline unsigned int __channel_get(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	unsigned int l;

	len = min(len, c->in - c->out);
	smp_rmb();
	l = min(len, c->size - (c->out & (c->size - 1)));
	memcpy(buf, c->buffer + (c->out & (c->size - 1)), l);
	memcpy(buf + l, c->buffer, len - l);
	smp_mb();
	c->out += len;

	return len;
}

static inline unsigned int channel_put(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	struct task_t * self = task_self();
	unsigned int l;

	spin_lock(&c->lock);
	if(channel_isfull(c))
	{
		l = 0;
		task_dynice_increase(self);
	}
	else
	{
		l = __channel_put(c, buf, len);
		task_dynice_restore(self);
	}
	spin_unlock(&c->lock);

	return l;
}

static inline unsigned int channel_get(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	struct task_t * self = task_self();
	unsigned int l;

	spin_lock(&c->lock);
	if(channel_isempty(c))
	{
		l = 0;
		task_dynice_increase(self);
	}
	else
	{
		l = __channel_get(c, buf, len);
		task_dynice_restore(self);
	}
	spin_unlock(&c->lock);

	return l;
}

void channel_send(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	unsigned int l;

	if(c && buf)
	{
		while(len > 0)
		{
			l = channel_put(c, buf, len);
			buf += l;
			len -= l;
			task_yield();
		}
	}
}

void channel_recv(struct channel_t * c, unsigned char * buf, unsigned int len)
{
	unsigned int l;

	if(c && buf)
	{
		while(len > 0)
		{
			l = channel_get(c, buf, len);
			buf += l;
			len -= l;
			task_yield();
		}
	}
}
