/*
 * driver/audio-sandbox-sdl.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <sandbox.h>

struct audio_sandbox_pdata_t
{
	audio_callback_t playcb;
	void * playcbdata;
};

static void sandbox_audio_sdl_playback_callback(void * data, void * buf, int count)
{
	struct audio_t * audio = (struct audio_t *)data;
	struct audio_sandbox_pdata_t * pdat = (struct audio_sandbox_pdata_t *)audio->priv;
	int len;

	if((len = pdat->playcb(pdat->playcbdata, buf, count)) < count)
	{
		memset((char *)buf + len, 0, count - len);
	}
}

static void sandbox_audio_sdl_playback_start(struct audio_t * audio, enum pcm_rate_t rate, enum pcm_format_t fmt, int ch, audio_callback_t cb, void * data)
{
	struct audio_sandbox_pdata_t * pdat = (struct audio_sandbox_pdata_t *)audio->priv;
	int sample;

	pdat->playcb = cb;
	pdat->playcbdata = data;

	/* 50ms */
	sample = __fls(rate / 20);
	sample = (sample != 0) ? (1 << sample) : 8192;
	sandbox_audio_sdl_close();
	sandbox_audio_sdl_open(fmt, rate, ch, sample, sandbox_audio_sdl_playback_callback, audio);
	sandbox_audio_sdl_start();
}

static void sandbox_audio_sdl_playback_stop(struct audio_t * audio)
{
	sandbox_audio_sdl_stop();
}

static void sandbox_audio_sdl_capture_start(struct audio_t * audio, enum pcm_rate_t rate, enum pcm_format_t fmt, int ch, audio_callback_t cb, void * data)
{
}

static void sandbox_audio_sdl_capture_stop(struct audio_t * audio)
{
}

static struct device_t * audio_sandbox_sdl_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct audio_sandbox_pdata_t * pdat;
	struct audio_t * audio;
	struct device_t * dev;

	pdat = malloc(sizeof(struct audio_sandbox_pdata_t));
	if(!pdat)
		return NULL;

	audio = malloc(sizeof(struct audio_t));
	if(!audio)
	{
		free(pdat);
		return NULL;
	}

	pdat->playcb = NULL;
	pdat->playcbdata = NULL;

	audio->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	audio->playback_start = sandbox_audio_sdl_playback_start;
	audio->playback_stop = sandbox_audio_sdl_playback_stop;
	audio->capture_start = sandbox_audio_sdl_capture_start;
	audio->capture_stop = sandbox_audio_sdl_capture_stop;
	audio->priv = pdat;

	if(!register_audio(&dev, audio))
	{
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void audio_sandbox_sdl_remove(struct device_t * dev)
{
	struct audio_t * audio = (struct audio_t *)dev->priv;

	if(audio && unregister_audio(audio))
	{
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
	}
}

static void audio_sandbox_sdl_suspend(struct device_t * dev)
{
}

static void audio_sandbox_sdl_resume(struct device_t * dev)
{
}

static struct driver_t audio_sandbox_sdl = {
	.name		= "audio-sandbox-sdl",
	.probe		= audio_sandbox_sdl_probe,
	.remove		= audio_sandbox_sdl_remove,
	.suspend	= audio_sandbox_sdl_suspend,
	.resume		= audio_sandbox_sdl_resume,
};

static __init void audio_sandbox_sdl_driver_init(void)
{
	register_driver(&audio_sandbox_sdl);
}

static __exit void audio_sandbox_sdl_driver_exit(void)
{
	unregister_driver(&audio_sandbox_sdl);
}

driver_initcall(audio_sandbox_sdl_driver_init);
driver_exitcall(audio_sandbox_sdl_driver_exit);
