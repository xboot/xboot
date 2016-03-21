/*
 * drivers/vibrator/vibrator.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <vibrator/vibrator.h>

static void vibrator_suspend(struct device_t * dev)
{
	struct vibrator_t * vib;

	if(!dev || dev->type != DEVICE_TYPE_VIBRATOR)
		return;

	vib = (struct vibrator_t *)(dev->driver);
	if(!vib)
		return;

	if(vib->suspend)
		vib->suspend(vib);
}

static void vibrator_resume(struct device_t * dev)
{
	struct vibrator_t * vib;

	if(!dev || dev->type != DEVICE_TYPE_VIBRATOR)
		return;

	vib = (struct vibrator_t *)(dev->driver);
	if(!vib)
		return;

	if(vib->resume)
		vib->resume(vib);
}

static ssize_t vibrator_read_state(struct kobj_t * kobj, void * buf, size_t size)
{
	struct vibrator_t * vib = (struct vibrator_t *)kobj->priv;
	return sprintf(buf, "%d", vibrator_get_state(vib));
}

static ssize_t vibrator_write_state(struct kobj_t * kobj, void * buf, size_t size)
{
	struct vibrator_t * vib = (struct vibrator_t *)kobj->priv;
	vibrator_set_state(vib, strtol(buf, NULL, 0));
	return size;
}

struct vibrator_t * search_vibrator(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_VIBRATOR);
	if(!dev)
		return NULL;

	return (struct vibrator_t *)dev->driver;
}

struct vibrator_t * search_first_vibrator(void)
{
	struct device_t * dev;

	dev = search_first_device_with_type(DEVICE_TYPE_VIBRATOR);
	if(!dev)
		return NULL;

	return (struct vibrator_t *)dev->driver;
}

bool_t register_vibrator(struct vibrator_t * vib)
{
	struct device_t * dev;

	if(!vib || !vib->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(vib->name);
	dev->type = DEVICE_TYPE_VIBRATOR;
	dev->suspend = vibrator_suspend;
	dev->resume = vibrator_resume;
	dev->driver = vib;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "state", vibrator_read_state, vibrator_write_state, vib);

	if(vib->init)
		(vib->init)(vib);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	return TRUE;
}

bool_t unregister_vibrator(struct vibrator_t * vib)
{
	struct device_t * dev;
	struct vibrator_t * driver;

	if(!vib || !vib->name)
		return FALSE;

	dev = search_device_with_type(vib->name, DEVICE_TYPE_VIBRATOR);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	driver = (struct vibrator_t *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)(driver);

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void vibrator_set_state(struct vibrator_t * vib, int state)
{
	if(vib && vib->set)
		vib->set(vib, (state > 0) ? 1 : 0);
}

int vibrator_get_state(struct vibrator_t * vib)
{
	if(vib && vib->get)
		return vib->get(vib);
	return 0;
}

void vibrator_vibrate(struct vibrator_t * vib, int state, int ms)
{
	if(vib && vib->vibrate)
	{
		vib->vibrate(vib, (state > 0) ? 1 : 0, (ms > 0) ? ms : 0);
	}
}

void vibrator_play(struct vibrator_t * vib, const char * morse)
{

}

