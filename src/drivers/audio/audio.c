/*
 * drivers/audio/audio.c
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

#include <audio/audio.h>

static void audio_suspend(struct device_t * dev)
{
	struct audio_t * audio;

	if(!dev || dev->type != DEVICE_TYPE_AUDIO)
		return;

	audio = (struct audio_t *)(dev->driver);
	if(!audio)
		return;

	if(audio->suspend)
		audio->suspend(audio);
}

static void audio_resume(struct device_t * dev)
{
	struct audio_t * audio;

	if(!dev || dev->type != DEVICE_TYPE_AUDIO)
		return;

	audio = (struct audio_t *)(dev->driver);
	if(!audio)
		return;

	if(audio->resume)
		audio->resume(audio);
}

struct audio_t * search_audio(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_AUDIO);
	if(!dev)
		return NULL;

	return (struct audio_t *)dev->driver;
}

bool_t register_audio(struct audio_t * audio)
{
	struct device_t * dev;

	if(!audio || !audio->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(audio->name);
	dev->type = DEVICE_TYPE_AUDIO;
	dev->suspend = audio_suspend;
	dev->resume = audio_resume;
	dev->driver = audio;
	dev->kobj = kobj_alloc_directory(dev->name);

	if(audio->init)
		(audio->init)(audio);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	return TRUE;
}

bool_t unregister_audio(struct audio_t * audio)
{
	struct device_t * dev;
	struct audio_t * driver;

	if(!audio || !audio->name)
		return FALSE;

	dev = search_device_with_type(audio->name, DEVICE_TYPE_AUDIO);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	driver = (struct audio_t *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)(audio);

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}
