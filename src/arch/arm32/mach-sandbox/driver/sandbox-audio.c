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
	struct fifo_t * playfifo;
	struct fifo_t * capfifo;
	struct sandbox_audio_data_t * rdat;
};

static void audio_callback_playback(void * data, void * buf, int count)
{
	struct sandbox_audio_private_data_t * dat = (struct sandbox_audio_private_data_t *)data;
	ssize_t len = fifo_get(dat->playfifo, (u8_t *)buf, count);
	if(len < count)
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

static void audio_open(struct audio_t * audio, enum audio_format_t fmt, enum audio_rate_t rate, int ch)
{
	struct sandbox_audio_private_data_t * dat = (struct sandbox_audio_private_data_t *)audio->priv;
	struct sandbox_audio_data_t * rdat = (struct sandbox_audio_data_t *)dat->rdat;
	int bps = fmt * rate * ch;
	int sample;

	dat->playfifo = fifo_alloc(bps * rdat->maxidle / 8);
	if(!dat->playfifo)
		return;
	dat->capfifo = fifo_alloc(bps * rdat->maxidle / 8);
	if(!dat->capfifo)
		return;

	/* 50ms */
	sample = __fls(rate / 20);
	sample = (sample != 0) ? (1 << sample) : 8192;
	sandbox_sdl_audio_open(fmt, rate, ch, sample, audio_callback_playback, dat);
}

static void audio_close(struct audio_t * audio)
{
	struct sandbox_audio_private_data_t * dat = (struct sandbox_audio_private_data_t *)audio->priv;

	sandbox_sdl_audio_close();
	if(dat->playfifo)
		fifo_free(dat->playfifo);
	if(dat->capfifo)
		fifo_free(dat->capfifo);
}

static ssize_t audio_playback(struct audio_t * audio, const u8_t * buf, size_t count)
{
	struct sandbox_audio_private_data_t * dat = (struct sandbox_audio_private_data_t *)audio->priv;
	ssize_t len = fifo_put(dat->playfifo, (u8_t *)buf, count);
	sandbox_sdl_audio_start();
	return len;
}

static ssize_t audio_capture(struct audio_t * audio, u8_t * buf, size_t count)
{
	struct sandbox_audio_private_data_t * dat = (struct sandbox_audio_private_data_t *)audio->priv;
	ssize_t len = fifo_get(dat->capfifo, (u8_t *)buf, count);
	sandbox_sdl_audio_start();
	return len;
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

	dat->rdat = rdat;

	audio->name = strdup(name);
	audio->init = audio_init,
	audio->exit = audio_exit,
	audio->open = audio_open,
	audio->close = audio_close,
	audio->playback = audio_playback,
	audio->capture = audio_capture,
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
