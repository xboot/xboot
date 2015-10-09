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

struct audio_state_t {
	struct {
		struct list_head head;
		spinlock_t lock;
		int sounds;
	} playback;
};

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
	struct audio_state_t * state;

	if(!audio || !audio->name)
		return FALSE;

	state = malloc(sizeof(struct audio_state_t));
	if(!state)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
	{
		free(state);
		return FALSE;
	}

	init_list_head(&state->playback.head);
	spin_lock_init(&state->playback.lock);
	state->playback.sounds = 0;
	audio->state = state;

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
		free(state);
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
	free(driver->state);
	free(dev->name);
	free(dev);
	return TRUE;
}

void audio_playback_add_sound(struct audio_t * audio, struct sound_t * sound)
{
	struct audio_state_t * state;
	irq_flags_t flags;

	if(!audio || !sound)
		return;

	state = (struct audio_state_t *)audio->state;
	spin_lock_irqsave(&state->playback.lock, flags);
	list_add_tail(&sound->entry, &state->playback.head);
	sound_set_position(sound, 0);
	state->playback.sounds++;
	spin_unlock_irqrestore(&state->playback.lock, flags);
}

void audio_playback_del_sound(struct audio_t * audio, struct sound_t * sound)
{
	struct audio_state_t * state;
	irq_flags_t flags;

	if(!audio || !sound)
		return;

	state = (struct audio_state_t *)audio->state;
	spin_lock_irqsave(&state->playback.lock, flags);
	list_del(&sound->entry);
	sound_set_position(sound, 0);
	state->playback.sounds--;
	spin_unlock_irqrestore(&state->playback.lock, flags);
}

void audio_playback_clr_sound(struct audio_t * audio)
{
	struct sound_t * pos, * n;
	struct audio_state_t * state;
	irq_flags_t flags;

	if(!audio)
		return;

	state = (struct audio_state_t *)audio->state;
	spin_lock_irqsave(&state->playback.lock, flags);
	list_for_each_entry_safe(pos, n, &(state->playback.head), entry)
	{
		list_del(&(pos->entry));
		sound_set_position(pos, 0);
	}
	state->playback.sounds = 0;
	spin_unlock_irqrestore(&state->playback.lock, flags);
}

static int normal_playback_callback(void * data, void * buf, int count)
{
	struct audio_t * audio = (struct audio_t *)data;
	struct audio_state_t * state = (struct audio_state_t *)audio->state;
	int len = 0;

	if(list_empty(&state->playback.head))
		return 0;

	if(state->playback.sounds == 1)
	{
		struct list_head * pos = (&state->playback.head)->next;
		struct sound_t * snd = list_entry(pos, struct sound_t, entry);
		if((len = sound_read(snd, buf, count)) < count)
		{
			if(sound_get_position(snd) >= sound_length(snd))
				audio_playback_del_sound(audio, snd);
		}
	}

	return len;
}

void audio_playback_start(struct audio_t * audio)
{
	struct audio_state_t * state;

	if(!audio)
		return;
	state = (struct audio_state_t *)audio->state;

	if(list_empty(&state->playback.head))
	{
		if(audio->playback_stop)
			audio->playback_stop(audio);
		return;
	}

	if(state->playback.sounds == 1)
	{
		struct list_head * pos = (&state->playback.head)->next;
		struct sound_t * snd = list_entry(pos, struct sound_t, entry);
		if(snd)
		{
			if(audio->playback_stop)
				audio->playback_stop(audio);
			if(audio->playback_start)
				audio->playback_start(audio, snd->rate, snd->fmt, snd->channel, normal_playback_callback, audio);
		}
	}
	else if(state->playback.sounds > 1)
	{
	}
}

void audio_playback_stop(struct audio_t * audio)
{
	if(!audio)
		return;

	audio_playback_clr_sound(audio);
	if(audio->playback_stop)
		audio->playback_stop(audio);
}
