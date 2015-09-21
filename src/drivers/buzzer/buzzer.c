/*
 * drivers/buzzer/buzzer.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

#include <buzzer/buzzer.h>

static void buzzer_suspend(struct device_t * dev)
{
	struct buzzer_t * buzzer;

	if(!dev || dev->type != DEVICE_TYPE_BUZZER)
		return;

	buzzer = (struct buzzer_t *)(dev->driver);
	if(!buzzer)
		return;

	if(buzzer->suspend)
		buzzer->suspend(buzzer);
}

static void buzzer_resume(struct device_t * dev)
{
	struct buzzer_t * buzzer;

	if(!dev || dev->type != DEVICE_TYPE_BUZZER)
		return;

	buzzer = (struct buzzer_t *)(dev->driver);
	if(!buzzer)
		return;

	if(buzzer->resume)
		buzzer->resume(buzzer);
}

static ssize_t buzzer_read_frequency(struct kobj_t * kobj, void * buf, size_t size)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)kobj->priv;
	int frequency;

	frequency = buzzer_get_frequency(buzzer);
	return sprintf(buf, "%d", frequency);
}

static ssize_t buzzer_write_frequency(struct kobj_t * kobj, void * buf, size_t size)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)kobj->priv;
	int frequency = strtol(buf, NULL, 0);

	buzzer_set_frequency(buzzer, frequency);
	return size;
}

struct buzzer_t * search_buzzer(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_BUZZER);
	if(!dev)
		return NULL;

	return (struct buzzer_t *)dev->driver;
}

bool_t register_buzzer(struct buzzer_t * buzzer)
{
	struct device_t * dev;

	if(!buzzer || !buzzer->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(buzzer->name);
	dev->type = DEVICE_TYPE_BUZZER;
	dev->suspend = buzzer_suspend;
	dev->resume = buzzer_resume;
	dev->driver = buzzer;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "frequency", buzzer_read_frequency, buzzer_write_frequency, buzzer);

	if(buzzer->init)
		(buzzer->init)(buzzer);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	return TRUE;
}

bool_t unregister_buzzer(struct buzzer_t * buzzer)
{
	struct device_t * dev;
	struct buzzer_t * driver;

	if(!buzzer || !buzzer->name)
		return FALSE;

	dev = search_device_with_type(buzzer->name, DEVICE_TYPE_BUZZER);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	driver = (struct buzzer_t *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)(buzzer);

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void buzzer_set_frequency(struct buzzer_t * buzzer, int frequency)
{
	if(buzzer && buzzer->set)
	{
		if(frequency < 0)
			frequency = 0;
		buzzer->set(buzzer, frequency);
	}
}

int buzzer_get_frequency(struct buzzer_t * buzzer)
{
	if(buzzer && buzzer->get)
		return buzzer->get(buzzer);
	return 0;
}

void buzzer_beep(struct buzzer_t * buzzer, int frequency, int millisecond)
{
	if(buzzer && buzzer->beep)
	{
		if(frequency < 0)
			frequency = 0;
		if(millisecond < 1)
			frequency = 1;
		buzzer->beep(buzzer, frequency, millisecond);
	}
}
