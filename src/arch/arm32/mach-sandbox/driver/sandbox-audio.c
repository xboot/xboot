/*
 * driver/sandbox-audio.c
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

#include <xboot.h>
#include <sandbox-audio.h>

struct sandbox_audio_private_data_t
{
	audio_callback_t playcb;
	void * playcbdata;
};

static void audio_callback_playback(void * data, void * buf, int count)
{
	struct sandbox_audio_private_data_t * dat = (struct sandbox_audio_private_data_t *)data;
	int len;

	if((len = dat->playcb(dat->playcbdata, buf, count)) < count)
	{
		memset((char *)buf + len, 0, count - len);
		sandbox_sdl_audio_stop();
	}
}

static void audio_init(struct audio_t * audio)
{
	sandbox_sdl_audio_init();
}

static void audio_exit(struct audio_t * audio)
{
	sandbox_sdl_audio_exit();
}

static void audio_playback_open(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
	struct sandbox_audio_private_data_t * dat = (struct sandbox_audio_private_data_t *)audio->priv;
	int sample;

	dat->playcb = cb;
	dat->playcbdata = data;

	/* 50ms */
	sample = __fls(rate / 20);
	sample = (sample != 0) ? (1 << sample) : 8192;
	sandbox_sdl_audio_open(fmt, rate, ch, sample, audio_callback_playback, dat);
}

static void audio_playback_start(struct audio_t * audio)
{
	sandbox_sdl_audio_start();
}

static void audio_playback_stop(struct audio_t * audio)
{
	sandbox_sdl_audio_stop();
}

static void audio_playback_close(struct audio_t * audio)
{
	sandbox_sdl_audio_close();
}

static void audio_capture_open(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
}

static void audio_capture_start(struct audio_t * audio)
{
}

static void audio_capture_stop(struct audio_t * audio)
{
}

static void audio_capture_close(struct audio_t * audio)
{
}

static void audio_suspend(struct audio_t * audio)
{
}

static void audio_resume(struct audio_t * audio)
{
}

static bool_t sandbox_register_audio(struct resource_t * res)
{
	struct sandbox_audio_data_t * rdat = (struct sandbox_audio_data_t *)res->data;
	struct sandbox_audio_private_data_t * dat;
	struct audio_t * audio;
	char name[64];

	dat = malloc(sizeof(struct sandbox_audio_private_data_t));
	if(!dat)
		return FALSE;

	audio = malloc(sizeof(struct audio_t));
	if(!audio)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	audio->name = strdup(name);
	audio->init = audio_init,
	audio->exit = audio_exit,
	audio->playback_open = audio_playback_open;
	audio->playback_start = audio_playback_start;
	audio->playback_stop = audio_playback_stop;
	audio->playback_close = audio_playback_close;
	audio->capture_open = audio_capture_open;
	audio->capture_start = audio_capture_start;
	audio->capture_stop = audio_capture_stop;
	audio->capture_close = audio_capture_close;
	audio->suspend = audio_suspend,
	audio->resume = audio_resume,
	audio->priv = dat;

	if(register_audio(audio))
		return TRUE;

	free(audio->name);
	free(audio);
	return FALSE;
}

static bool_t sandbox_unregister_audio(struct resource_t * res)
{
	struct audio_t * audio;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	audio = search_audio(name);
	if(!audio)
		return FALSE;

	if(!unregister_audio(audio))
		return FALSE;

	free(audio->priv);
	free(audio->name);
	free(audio);
	return TRUE;
}

static __init void sandbox_audio_init(void)
{
	resource_for_each_with_name("sandbox-audio", sandbox_register_audio);
}

static __exit void sandbox_audio_exit(void)
{
	resource_for_each_with_name("sandbox-audio", sandbox_unregister_audio);
}

device_initcall(sandbox_audio_init);
device_exitcall(sandbox_audio_exit);
