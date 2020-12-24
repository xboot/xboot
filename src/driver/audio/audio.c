/*
 * driver/audio/audio.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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

void audio_playback_start(struct audio_t * audio, enum pcm_rate_t rate, enum pcm_format_t fmt, int ch, audio_callback_t cb, void * data)
{
	if(audio && audio->playback_start)
		audio->playback_start(audio, rate, fmt, ch, cb, data);
}

void audio_playback_stop(struct audio_t * audio)
{
	if(audio && audio->playback_stop)
		audio->playback_stop(audio);
}

void audio_capture_start(struct audio_t * audio, enum pcm_rate_t rate, enum pcm_format_t fmt, int ch, audio_callback_t cb, void * data)
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
