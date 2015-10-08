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

struct audio_t * search_first_audio(void)
{
	struct device_t * dev;

	dev = search_first_device_with_type(DEVICE_TYPE_AUDIO);
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

	init_list_head(&audio->playback.head);
	spin_lock_init(&audio->playback.lock);
	audio->playback.sounds = 0;

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

static void audio_playback_add_sound(struct audio_t * audio, struct sound_t * sound)
{
	irq_flags_t flags;

	if(!audio || !sound)
		return;

	spin_lock_irqsave(&audio->playback.lock, flags);
	list_add_tail(&sound->entry, &audio->playback.head);
	audio->playback.sounds++;
	spin_unlock_irqrestore(&audio->playback.lock, flags);
}

static void audio_playback_del_sound(struct audio_t * audio, struct sound_t * sound)
{
	irq_flags_t flags;

	if(!audio || !sound)
		return;

	spin_lock_irqsave(&audio->playback.lock, flags);
	list_del(&sound->entry);
	audio->playback.sounds--;
	spin_unlock_irqrestore(&audio->playback.lock, flags);
}

void audio_playback_play(struct audio_t * audio, const char * filename)
{
	struct sound_t * snd;

	if(!audio || !filename)
		return;

	snd = sound_alloc(filename);
	if(!snd)
		return;

	audio_playback_add_sound(audio, snd);
}

void audio_playback_stop(struct audio_t * audio)
{
	struct sound_t * pos, * n;
	irq_flags_t flags;

	if(!audio)
		return;

	spin_lock_irqsave(&audio->playback.lock, flags);
	list_for_each_entry_safe(pos, n, &(audio->playback.head), entry)
	{
		list_del(&(pos->entry));
		sound_free(pos);
	}
	audio->playback.sounds = 0;
	spin_unlock_irqrestore(&audio->playback.lock, flags);

	if(audio->playback_stop)
		audio->playback_stop(audio);
}

#if 0
static int playback_callback(void * data, void * buf, int count)
{
	struct audio_t * audio = (struct audio_t *)data;
	int len;

	if(list_empty(&audio->playback.head))
		return 0;

	if(audio->playback.sounds > 1)
	{
		/* mixer mode */
		len = 0;
	}
	else
	{
		struct list_head * pos = (&audio->playback.head)->next;
		struct sound_t * snd = list_entry(pos, struct sound_t, entry);
		if((len = sound_read(snd, buf, count)) < count)
		{
			if(sound_tell(snd) > sound_length())
//			audio->playback_stop(audio);
			audio_playback_del_sound(audio, snd);
//			sound_seek(snd, 0);
//			len = sound_read(snd, buf, count);
		}
	}

	return len;
}

void audio_playback_start(struct audio_t * audio)
{
	if(!audio)
		return;

	if(list_empty(&audio->playback.head))
		return;

	if(audio->playback.sounds > 1)
	{
		/* mixer mode */
		audio->playback_start(audio, PCM_RATE_44100, PCM_FORMAT_BIT16, 2, playback_callback, audio);
	}
	else
	{
		struct list_head * pos = (&audio->playback.head)->next;
		struct sound_t * snd = list_entry(pos, struct sound_t, entry);
		audio->playback_start(audio, snd->rate, snd->fmt, snd->channel, playback_callback, audio);
	}
}
#endif
