/*
 * driver/audio/audio.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <audio/audio.h>

struct audio_t * search_audio(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_AUDIO);
	if(!dev)
		return NULL;
	return (struct audio_t *)dev->priv;
}

struct audio_t * search_first_audio(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_AUDIO);
	if(!dev)
		return NULL;
	return (struct audio_t *)dev->priv;
}

struct device_t * register_audio(struct audio_t * audio, struct driver_t * drv)
{
	struct device_t * dev;

	if(!audio || !audio->name)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	init_list_head(&audio->soundpool.list);
	spin_lock_init(&audio->soundpool.lock);

	dev->name = strdup(audio->name);
	dev->type = DEVICE_TYPE_AUDIO;
	dev->driver = drv;
	dev->priv = audio;
	dev->kobj = kobj_alloc_directory(dev->name);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_audio(struct audio_t * audio)
{
	struct device_t * dev;

	if(audio && audio->name)
	{
		dev = search_device(audio->name, DEVICE_TYPE_AUDIO);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

void audio_playback_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
	if(audio && audio->playback_start)
		audio->playback_start(audio, rate, fmt, ch, cb, data);
}

void audio_playback_stop(struct audio_t * audio)
{
	if(audio && audio->playback_stop)
		audio->playback_stop(audio);
}

void audio_capture_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
	if(audio && audio->capture_start)
		audio->capture_start(audio, rate, fmt, ch, cb, data);
}

void audio_capture_stop(struct audio_t * audio)
{
	if(audio && audio->capture_stop)
		audio->capture_stop(audio);
}

int audio_ioctl(struct audio_t * audio, const char * cmd, void * arg)
{
	if(audio && audio->ioctl)
		return audio->ioctl(audio, cmd, arg);
	return -1;
}

static int audio_playback_callback(void * data, void * buf, int count)
{
	struct audio_t * audio = (struct audio_t *)data;
	struct sound_t * pos, * n;
	irq_flags_t flags;
	char * pbuf = buf;
	int32_t left[240];
	int32_t right[240];
	int32_t result[240];
	int32_t * pl = left;
	int32_t * pr = right;
	int16_t * p;
	int bytes = 0;
	int sample;
	int length;
	int empty;
	int i;

	spin_lock_irqsave(&audio->soundpool.lock, flags);
	empty = list_empty_careful(&audio->soundpool.list);
	spin_unlock_irqrestore(&audio->soundpool.lock, flags);
	if(!empty)
	{
		while(count > 0)
		{
			sample = min((int)(count >> 2), 240);
			length = sample << 2;
			memset(left, 0, length);
			memset(right, 0, length);
			spin_lock_irqsave(&audio->soundpool.lock, flags);
			list_for_each_entry_safe(pos, n, &audio->soundpool.list, list)
			{
				if(pos->loop != 0)
				{
					for(i = 0; i < sample; i++)
					{
						if(pos->sample > pos->postion)
						{
							p = (int16_t *)(&pos->source[pos->postion]);
							left[i] += (p[0] * pos->lvol) >> 12;
							right[i] += (p[1] * pos->rvol) >> 12;
							pos->postion++;
						}
						else
						{
							if(pos->loop > 0)
								pos->loop--;
							if(pos->loop != 0)
							{
								pos->postion = 0;
								p = (int16_t *)(&pos->source[pos->postion]);
								left[i] += (p[0] * pos->lvol) >> 12;
								right[i] += (p[1] * pos->rvol) >> 12;
							}
						}
					}
				}
			}
			spin_unlock_irqrestore(&audio->soundpool.lock, flags);
			p = (int16_t *)result;
			for(i = 0; i < sample; i++)
			{
				*p++ = clamp(pl[i], -32768, 32767);
				*p++ = clamp(pr[i], -32768, 32767);
			}
			memcpy(pbuf, result, length);
			bytes += length;
			pbuf += length;
			count -= length;
		}
		spin_lock_irqsave(&audio->soundpool.lock, flags);
		list_for_each_entry_safe(pos, n, &audio->soundpool.list, list)
		{
			if(pos->loop == 0)
			{
				list_del(&pos->list);
				if(pos->cb)
					pos->cb(pos);
			}
		}
		spin_unlock_irqrestore(&audio->soundpool.lock, flags);
	}
	return bytes;
}

void audio_playback(struct audio_t * audio, struct sound_t * snd)
{
	struct sound_t * pos, * n;
	irq_flags_t flags;
	int found = 0;

	if(audio && snd)
	{
		spin_lock_irqsave(&audio->soundpool.lock, flags);
		list_for_each_entry_safe(pos, n, &audio->soundpool.list, list)
		{
			if(pos == snd)
			{
				found = 1;
				break;
			}
		}
		spin_unlock_irqrestore(&audio->soundpool.lock, flags);
		if(!found)
		{
			spin_lock_irqsave(&audio->soundpool.lock, flags);
			list_add_tail(&snd->list, &audio->soundpool.list);
			spin_unlock_irqrestore(&audio->soundpool.lock, flags);
			audio_playback_start(audio, AUDIO_RATE_48000, AUDIO_FORMAT_S16, 2, audio_playback_callback, audio);
		}
	}
}
