/*
 * driver/stepper/stepper.c
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
#include <stepper/stepper.h>

static ssize_t stepper_write_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct stepper_t * m = (struct stepper_t *)kobj->priv;
	stepper_enable(m);
	return size;
}

static ssize_t stepper_write_disable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct stepper_t * m = (struct stepper_t *)kobj->priv;
	stepper_disable(m);
	return size;
}

static ssize_t stepper_write_move(struct kobj_t * kobj, void * buf, size_t size)
{
	struct stepper_t * m = (struct stepper_t *)kobj->priv;
	stepper_move(m, strtol(buf, NULL, 0), 0);
	return size;
}

static ssize_t stepper_read_busying(struct kobj_t * kobj, void * buf, size_t size)
{
	struct stepper_t * m = (struct stepper_t *)kobj->priv;
	return sprintf(buf, "%d", stepper_busying(m) ? 1 : 0);
}

struct stepper_t * search_stepper(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_STEPPER);
	if(!dev)
		return NULL;
	return (struct stepper_t *)dev->priv;
}

bool_t register_stepper(struct device_t ** device, struct stepper_t * m)
{
	struct device_t * dev;

	if(!m || !m->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(m->name);
	dev->type = DEVICE_TYPE_STEPPER;
	dev->priv = m;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "enable", NULL, stepper_write_enable, m);
	kobj_add_regular(dev->kobj, "disable", NULL, stepper_write_disable, m);
	kobj_add_regular(dev->kobj, "move", NULL, stepper_write_move, m);
	kobj_add_regular(dev->kobj, "busying", stepper_read_busying, NULL, m);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_stepper(struct stepper_t * m)
{
	struct device_t * dev;

	if(!m || !m->name)
		return FALSE;

	dev = search_device(m->name, DEVICE_TYPE_STEPPER);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void stepper_enable(struct stepper_t * m)
{
	if(m && m->enable)
		m->enable(m);
}

void stepper_disable(struct stepper_t * m)
{
	if(m && m->disable)
		m->disable(m);
}

void stepper_move(struct stepper_t * m, int step, int speed)
{
	if(m && m->move && (step != 0))
		m->move(m, step, speed);
}

int stepper_busying(struct stepper_t * m)
{
	if(m && m->busying)
		return m->busying(m);
	return 0;
}

