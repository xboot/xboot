/*
 * driver/audio-sandbox.c
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
#include <sandbox.h>

struct audio_sandbox_pdata_t
{
	void * pctx;
	void * cctx;
};

static void audio_sandbox_playback_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
	struct audio_sandbox_pdata_t * pdat = (struct audio_sandbox_pdata_t *)audio->priv;
	if(pdat->pctx)
	{
		if(!sandbox_audio_playback_status(pdat->pctx))
		{
			sandbox_audio_playback_stop(pdat->pctx);
			pdat->pctx = sandbox_audio_playback_start(rate, fmt, ch, cb, data);
		}
	}
	else
	{
		pdat->pctx = sandbox_audio_playback_start(rate, fmt, ch, cb, data);
	}
}

static void audio_sandbox_playback_stop(struct audio_t * audio)
{
	struct audio_sandbox_pdata_t * pdat = (struct audio_sandbox_pdata_t *)audio->priv;
	if(pdat->pctx)
	{
		sandbox_audio_playback_stop(pdat->pctx);
		pdat->pctx = NULL;
	}
}

static void audio_sandbox_capture_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
	struct audio_sandbox_pdata_t * pdat = (struct audio_sandbox_pdata_t *)audio->priv;
	if(pdat->cctx)
	{
		if(!sandbox_audio_capture_status(pdat->pctx))
		{
			sandbox_audio_capture_stop(pdat->cctx);
			pdat->cctx = sandbox_audio_capture_start(rate, fmt, ch, cb, data);
		}
	}
	else
	{
		pdat->cctx = sandbox_audio_capture_start(rate, fmt, ch, cb, data);
	}
}

static void audio_sandbox_capture_stop(struct audio_t * audio)
{
	struct audio_sandbox_pdata_t * pdat = (struct audio_sandbox_pdata_t *)audio->priv;
	if(pdat->cctx)
	{
		sandbox_audio_capture_stop(pdat->cctx);
		pdat->cctx = NULL;
	}
}

static int audio_sandbox_ioctl(struct audio_t * audio, const char * cmd, void * arg)
{
	int * p = arg;

	switch(shash(cmd))
	{
	case 0x892b3889: /* "audio-set-playback-volume" */
		if(p)
		{
			sandbox_audio_set_playback_volume(clamp(p[0], 0, 1000));
			return 0;
		}
		break;
	case 0x3eee6d7d: /* "audio-get-playback-volume" */
		if(p)
		{
			p[0] = sandbox_audio_get_playback_volume();
			return 0;
		}
		break;
	case 0x6dab0056: /* "audio-set-capture-volume" */
		if(p)
		{
			sandbox_audio_set_capture_volume(clamp(p[0], 0, 1000));
			return 0;
		}
		break;
	case 0x44a166ca: /* "audio-get-capture-volume" */
		if(p)
		{
			p[0] = sandbox_audio_get_capture_volume();
			return 0;
		}
		break;
	default:
		break;
	}
	return -1;
}

static struct device_t * audio_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
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

	pdat->pctx = NULL;
	pdat->cctx = NULL;

	audio->name = alloc_device_name(dt_read_name(n), -1);
	audio->playback_start = audio_sandbox_playback_start;
	audio->playback_stop = audio_sandbox_playback_stop;
	audio->capture_start = audio_sandbox_capture_start;
	audio->capture_stop = audio_sandbox_capture_stop;
	audio->ioctl = audio_sandbox_ioctl;
	audio->priv = pdat;

	if(!(dev = register_audio(audio, drv)))
	{
		if(pdat->pctx)
			sandbox_audio_playback_stop(pdat->pctx);
		if(pdat->cctx)
			sandbox_audio_capture_stop(pdat->cctx);
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
		return NULL;
	}
	return dev;
}

static void audio_sandbox_remove(struct device_t * dev)
{
	struct audio_t * audio = (struct audio_t *)dev->priv;
	struct audio_sandbox_pdata_t * pdat = (struct audio_sandbox_pdata_t *)audio->priv;

	if(audio)
	{
		unregister_audio(audio);
		if(pdat->pctx)
			sandbox_audio_playback_stop(pdat->pctx);
		if(pdat->cctx)
			sandbox_audio_capture_stop(pdat->cctx);
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
	}
}

static void audio_sandbox_suspend(struct device_t * dev)
{
}

static void audio_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t audio_sandbox = {
	.name		= "audio-sandbox",
	.probe		= audio_sandbox_probe,
	.remove		= audio_sandbox_remove,
	.suspend	= audio_sandbox_suspend,
	.resume		= audio_sandbox_resume,
};

static __init void audio_sandbox_driver_init(void)
{
	register_driver(&audio_sandbox);
}

static __exit void audio_sandbox_driver_exit(void)
{
	unregister_driver(&audio_sandbox);
}

driver_initcall(audio_sandbox_driver_init);
driver_exitcall(audio_sandbox_driver_exit);
