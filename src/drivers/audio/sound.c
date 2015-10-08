/*
 * drivers/audio/sound.c
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

#include <audio/sound.h>

extern struct sound_loader_t __sound_loader_wav;

static struct sound_loader_t * __sound_loader[] = {
	&__sound_loader_wav,
	NULL,
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
	struct sound_loader_t ** i;
	const char * ext = fileext(filename);

	if(!ext)
		return NULL;

	for(i = __sound_loader; *i != NULL; i++)
	{
		if(strcasecmp(ext, (*i)->ext) == 0)
			return (*i);
	}
	return NULL;
}

struct sound_t * sound_alloc(const char * filename)
{
	struct sound_t * sound;
	struct sound_loader_t * loader = search_sound_loader(filename);

	if(!loader)
		return NULL;

	sound = malloc(sizeof(struct sound_t));
	if(!sound)
		return NULL;

	if(loader->load(sound, filename))
		return sound;

	free(sound);
	return NULL;
}

int sound_length(struct sound_t * sound)
{
	if(sound)
		return sound->length;
	return 0;
}

int sound_seek(struct sound_t * sound, int offset)
{
	if(sound && sound->seek)
		return sound->seek(sound, offset);
	return 0;
}

int sound_tell(struct sound_t * sound)
{
	if(sound)
		return sound->length;
	return 0;
}

int sound_read(struct sound_t * sound, void * buf, int count)
{
	if(sound && sound->read)
		return sound->read(sound, buf, count);
	return 0;
}

void sound_free(struct sound_t * sound)
{
	if(sound && sound->close)
	{
		sound->close(sound);
		free(sound);
	}
}
