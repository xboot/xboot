/*
 * driver/audio/sound.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <audio/pool.h>
#include <audio/audio.h>
#include <audio/sound.h>

extern bool_t sound_load_wav(struct sound_t * snd, const char * filename);

struct sound_loader_t
{
	const char * ext;
	bool_t (*load)(struct sound_t * snd, const char * filename);
};

static struct sound_loader_t __sound_loader[] = {
	{"wav",	sound_load_wav},
	{NULL,	NULL},
};

static const char * fileext(const char * name)
{
	const char * ret = NULL;
	const char * p;

	if(name != NULL)
	{
		ret = p = strchr(name, '.');
		while(p != NULL)
		{
			p = strchr(p + 1, '.');
			if(p != NULL)
				ret = p;
		}
		if(ret != NULL)
			ret++;
	}
	return ret;
}

static struct sound_loader_t * search_sound_loader(const char * filename)
{
	struct sound_loader_t * loader = &__sound_loader[0];
	const char * ext = fileext(filename);

	if(!ext)
		return NULL;

	while(loader && loader->ext)
	{
		if(strcasecmp(ext, loader->ext) == 0)
			return loader;
		loader++;
	}
	return NULL;
}

struct sound_t * sound_alloc(const char * filename)
{
	struct sound_t * snd;
	struct sound_loader_t * loader = search_sound_loader(filename);

	if(!loader)
		return NULL;

	snd = malloc(sizeof(struct sound_t));
	if(!snd)
		return NULL;

	if(loader->load(snd, filename))
		return snd;

	free(snd);
	return NULL;
}

void sound_free(struct sound_t * snd)
{
	if(snd)
	{
		sound_stop(snd);
		if(snd->close)
			snd->close(snd);
		free(snd);
	}
}

struct sound_info_t * sound_get_info(struct sound_t * snd)
{
	if(snd)
		return &snd->info;
	return NULL;
}

enum sound_status_t sound_get_status(struct sound_t * snd)
{
	if(snd)
		return snd->status;
	return SOUND_STATUS_STOP;
}

void sound_set_volume(struct sound_t * snd, int percent)
{
	if(snd)
	{
		if(percent < 0)
			percent = 0;
		if(percent > 100)
			percent = 100;
		snd->volume = percent;
	}
}

int sound_get_volume(struct sound_t * snd)
{
	if(snd)
		return snd->volume;
	return 0;
}

void sound_set_position(struct sound_t * snd, int position)
{
	if(snd && snd->seek)
	{
		if(position < 0)
			position = 0;
		if(position > snd->info.length)
			position = snd->info.length;
		snd->seek(snd, position);
	}
}

int sound_get_position(struct sound_t * snd)
{
	if(snd)
		return snd->position;
	return 0;
}

void sound_play(struct sound_t * snd)
{
	if(snd)
	{
		snd->status = SOUND_STATUS_PLAY;
		sound_pool_add(snd);
		audio_playback(search_first_audio());
	}
}

void sound_pause(struct sound_t * snd)
{
	if(snd)
	{
		snd->status = SOUND_STATUS_PAUSE;
	}
}

void sound_stop(struct sound_t * snd)
{
	if(snd)
	{
		snd->status = SOUND_STATUS_STOP;
		sound_pool_del(snd);
		sound_set_position(snd, 0);
	}
}
