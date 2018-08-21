/*
 * driver/watchdog/watchdog.c
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
#include <watchdog/watchdog.h>

static ssize_t watchdog_read_timeout(struct kobj_t * kobj, void * buf, size_t size)
{
	struct watchdog_t * wdg = (struct watchdog_t *)kobj->priv;
	int timeout;

	timeout = watchdog_get_timeout(wdg);
	return sprintf(buf, "%d", timeout);
}

static ssize_t watchdog_write_timeout(struct kobj_t * kobj, void * buf, size_t size)
{
	struct watchdog_t * wdg = (struct watchdog_t *)kobj->priv;
	int timeout = strtol(buf, NULL, 0);

	watchdog_set_timeout(wdg, timeout);
	return size;
}

struct watchdog_t * search_watchdog(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_WATCHDOG);
	if(!dev)
		return NULL;
	return (struct watchdog_t *)dev->priv;
}

struct watchdog_t * search_first_watchdog(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_WATCHDOG);
	if(!dev)
		return NULL;
	return (struct watchdog_t *)dev->priv;
}

bool_t register_watchdog(struct device_t ** device,struct watchdog_t * wdg)
{
	struct device_t * dev;

	if(!wdg || !wdg->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(wdg->name);
	dev->type = DEVICE_TYPE_WATCHDOG;
	dev->priv = wdg;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "timeout", watchdog_read_timeout, watchdog_write_timeout, wdg);

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

bool_t unregister_watchdog(struct watchdog_t * wdg)
{
	struct device_t * dev;

	if(!wdg || !wdg->name)
		return FALSE;

	dev = search_device(wdg->name, DEVICE_TYPE_WATCHDOG);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void watchdog_set_timeout(struct watchdog_t * wdg, int timeout)
{
	if(wdg && wdg->set)
	{
		if(timeout < 0)
			timeout = 0;
		wdg->set(wdg, timeout);
	}
}

int watchdog_get_timeout(struct watchdog_t * wdg)
{
	if(wdg && wdg->get)
		return wdg->get(wdg);
	return 0;
}
