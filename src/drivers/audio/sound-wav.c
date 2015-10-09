/*
 * drivers/audio/sound-wav.c
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

struct wav_header_t {
	uint8_t		riff[4];
	uint32_t	riffsz;
	uint8_t		wave[4];
	uint8_t    	fmt[4];
	uint32_t	fmtsz;
	uint16_t   	fmttag;
	uint16_t	channel;
	uint32_t	samplerate;
	uint32_t	byterate;
	uint16_t	align;
	uint16_t	bps;
	uint8_t		data[4];
	uint32_t	datasz;
};

struct sound_wav_data_t {
	int fd;
};

static int sound_wav_seek(struct sound_t * sound, int offset)
{
	struct sound_wav_data_t * dat = (struct sound_wav_data_t *)sound->priv;

	if(offset < 0)
		offset = 0;
	if(offset > sound->length)
		offset = sound->length;

	if(lseek(dat->fd, sizeof(struct wav_header_t) + offset, SEEK_SET) > 0)
		sound->position = offset;
	return sound->position;
}

static int sound_wav_read(struct sound_t * sound, void * buf, int count)
{
	struct sound_wav_data_t * dat = (struct sound_wav_data_t *)sound->priv;
	int len;

	len = read(dat->fd, buf, count);
	sound->position += len;
	return len;
}

static void sound_wav_close(struct sound_t * sound)
{
	struct sound_wav_data_t * dat = (struct sound_wav_data_t *)sound->priv;
	close(dat->fd);
	free(dat);
}

static bool_t sound_wav_load(struct sound_t * sound, const char * filename)
{
	struct sound_wav_data_t * dat;
	struct wav_header_t header;
	int fd;

	if(!sound)
		return FALSE;

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return FALSE;

	if(read(fd, &header, sizeof(struct wav_header_t)) != sizeof(struct wav_header_t))
	{
		close(fd);
		return FALSE;
	}

	header.riffsz = be32_to_cpu(header.riffsz);
	header.fmtsz = be32_to_cpu(header.fmtsz);
	header.fmttag = be16_to_cpu(header.fmttag);
	header.channel = be16_to_cpu(header.channel);
	header.samplerate = be32_to_cpu(header.samplerate);
	header.byterate = be32_to_cpu(header.byterate);
	header.align = be16_to_cpu(header.align);
	header.bps = be16_to_cpu(header.bps);
	header.datasz = be32_to_cpu(header.datasz);

	if(	(memcmp(header.riff, "RIFF", 4) != 0) ||
		(memcmp(header.wave, "WAVE", 4) != 0) ||
		(memcmp(header.fmt,  "fmt ", 4) != 0) ||
		(memcmp(header.data, "data", 4) != 0) ||
		(header.fmttag != 1) || (header.datasz < header.align) )
	{
		close(fd);
		return FALSE;
	}

	dat = (struct sound_wav_data_t *)malloc(sizeof(struct sound_wav_data_t));
	if(!dat)
	{
		close(fd);
		return FALSE;
	}

	dat->fd = fd;

	sound->rate = (enum pcm_rate_t)header.samplerate;
	sound->fmt = (enum pcm_format_t)header.bps;
	sound->channel = header.channel;
	sound->position = 0;
	sound->length = header.datasz;
	sound->pause = 0;
	sound->loop = 0;
	sound->volume = 100;
	sound->seek = sound_wav_seek;
	sound->read = sound_wav_read;
	sound->close = sound_wav_close;
	sound->priv = dat;

	return TRUE;
}

struct sound_loader_t __sound_loader_wav = {
	.ext = "wav",
	.load = sound_wav_load,
};
