/*
 * driver/led/ledtrigger.c
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
#include <led/ledtrigger.h>

static ssize_t ledtrigger_write_activity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct ledtrigger_t * trigger = (struct ledtrigger_t *)kobj->priv;

	ledtrigger_activity(trigger);
	return size;
}

struct ledtrigger_t * search_ledtrigger(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_LEDTRIGGER);
	if(!dev)
		return NULL;
	return (struct ledtrigger_t *)dev->priv;
}

bool_t register_ledtrigger(struct device_t ** device, struct ledtrigger_t * trigger)
{
	struct device_t * dev;

	if(!trigger || !trigger->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(trigger->name);
	dev->type = DEVICE_TYPE_LEDTRIGGER;
	dev->priv = trigger;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "activity", NULL, ledtrigger_write_activity, trigger);

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

bool_t unregister_ledtrigger(struct ledtrigger_t * trigger)
{
	struct device_t * dev;

	if(!trigger || !trigger->name)
		return FALSE;

	dev = search_device(trigger->name, DEVICE_TYPE_LEDTRIGGER);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void ledtrigger_activity(struct ledtrigger_t * trigger)
{
	if(trigger && trigger->activity)
		trigger->activity(trigger);
}
