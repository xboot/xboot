/*
 * drivers/audio/audio.c
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

#include <audio/pool.h>
#include <audio/audio.h>

static struct audio_t * __default_audio = NULL;

struct audio_t * get_default_audio(void)
{
	return __default_audio;
}

void set_default_audio(const char * name)
{
	struct audio_t * audio = search_audio(name);

	if(!audio)
		return;

	if(__default_audio != audio)
	{
		if(__default_audio && __default_audio->playback_stop)
			__default_audio->capture_stop(__default_audio);
		__default_audio = audio;
		audio_playback();
	}
}

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

	if(!get_default_audio())
		set_default_audio(audio->name);
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

	if(get_default_audio() == audio)
	{
		__default_audio = NULL;
		struct device_t * d = search_first_device_with_type(DEVICE_TYPE_AUDIO);
		if(d)
			set_default_audio(d->name);
	}
	return TRUE;
}

static int sound_read(struct sound_t * snd, void * buf, int count)
{
	int i, len = 0;

	if(snd && snd->read)
	{
		len = snd->read(snd, buf, count);

		if(snd->volume == 100)
		{
		}
		else if(snd->volume == 0)
		{
			memset(buf, 0, len);
		}
		else
		{
			s8_t * p8; s16_t * p16; s32_t * p32;
			s32_t v;
			switch(snd->info.fmt)
			{
			case PCM_FORMAT_BIT8:
				p8 = buf;
				for(i = 0; i < len; i++)
				{
					v = ((s32_t)(p8[i])) * snd->volume / 100;
					p8[i] = (s8_t)v;
				}
				break;

			case PCM_FORMAT_BIT16:
				p16 = buf;
				for(i = 0; i < len / 2; i++)
				{
					v = ((s32_t)be16_to_cpu(p16[i])) * snd->volume / 100;
					p16[i] = cpu_to_be16((s16_t)v);
				}
				break;

			case PCM_FORMAT_BIT24:
			case PCM_FORMAT_BIT32:
				p32 = buf;
				for(i = 0; i < len / 4; i++)
				{
					v = ((s32_t)be32_to_cpu(p32[i])) * snd->volume / 100;
					p32[i] = cpu_to_be32(v);
				}
				break;

			default:
				break;
			}
		}
	}

	return len;
}

static int audio_playback_callback(void * data, void * buf, int count)
{
	struct audio_t * audio = (struct audio_t *)data;
	struct sound_list_t * sp = &__sound_pool;
	struct list_head * pos = sp->entry.next;
	int len = 0;

	if(list_empty_careful(&sp->entry))
	{
		if(audio->playback_stop)
			audio->playback_stop(audio);
		return 0;
	}

	if(list_is_last(pos, &sp->entry))
	{
		struct sound_list_t * sl = list_entry(pos, struct sound_list_t, entry);
		struct sound_t * snd = sl->snd;
		if(snd && (sound_get_status(snd) == SOUND_STATUS_PLAY))
		{
			if((len = sound_read(snd, buf, count)) < count)
			{
				if(sound_get_position(snd) >= snd->info.length)
				{
					sound_stop(snd);
				}
			}
		}
	}

	return len;
}

static void audio_playback_start(struct audio_t * audio)
{
	struct sound_list_t * sp = &__sound_pool;
	struct list_head * pos = sp->entry.next;

	if(!audio)
		return;

	if(list_empty_careful(&sp->entry))
		return;

	/* Just one sound */
	if(list_is_last(pos, &sp->entry))
	{
		struct sound_list_t * sl = list_entry(pos, struct sound_list_t, entry);
		struct sound_t * snd = sl->snd;
		if(snd)
		{
			if(audio->playback_start)
				audio->playback_start(audio, snd->info.rate, snd->info.fmt, snd->info.channel, audio_playback_callback, audio);
		}
	}
	/* More than one sound */
	else
	{

	}
}

void audio_playback(void)
{
	audio_playback_start(get_default_audio());
}
