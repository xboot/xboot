/*
 * driver/audio/sound-wav.c
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

struct sound_data_wav_t {
	int fd;
};

static int sound_seek_wav(struct sound_t * snd, int offset)
{
	struct sound_data_wav_t * dat = (struct sound_data_wav_t *)snd->priv;

	if(offset < 0)
		offset = 0;
	if(offset > snd->info.length)
		offset = snd->info.length;

	if(lseek(dat->fd, sizeof(struct wav_header_t) + offset, SEEK_SET) > 0)
		snd->position = offset;
	return snd->position;
}

static int sound_read_wav(struct sound_t * snd, void * buf, int count)
{
	struct sound_data_wav_t * dat = (struct sound_data_wav_t *)snd->priv;
	int len;

	len = read(dat->fd, buf, count);
	snd->position += len;
	return len;
}

static void sound_close_wav(struct sound_t * snd)
{
	struct sound_data_wav_t * dat = (struct sound_data_wav_t *)snd->priv;
	free(snd->info.title);
	free(snd->info.singer);
	close(dat->fd);
	free(dat);
}

bool_t sound_load_wav(struct sound_t * snd, const char * filename)
{
	struct sound_data_wav_t * dat;
	struct wav_header_t header;
	int fd;

	if(!snd)
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

	dat = (struct sound_data_wav_t *)malloc(sizeof(struct sound_data_wav_t));
	if(!dat)
	{
		close(fd);
		return FALSE;
	}

	dat->fd = fd;

	snd->info.title = strdup(filename);
	snd->info.singer = strdup("unknown");
	snd->info.rate = (enum pcm_rate_t)header.samplerate;
	snd->info.fmt = (enum pcm_format_t)header.bps;
	snd->info.channel = header.channel;
	snd->info.length = header.datasz;
	snd->status = SOUND_STATUS_STOP;
	snd->volume = 100;
	snd->position = 0;
	snd->seek = sound_seek_wav;
	snd->read = sound_read_wav;
	snd->close = sound_close_wav;
	snd->priv = dat;

	return TRUE;
}
