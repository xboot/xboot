/*
 * kernel/sound/sound.c
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
#include <sound/sound.h>

struct sound_t * sound_alloc(int sample)
{
	struct sound_t * snd;
	void * source;

	if(sample <= 0)
		return NULL;

	snd = malloc(sizeof(struct sound_t));
	if(!snd)
		return NULL;

	source = memalign(4, sample << 2);
	if(!source)
	{
		free(snd);
		return NULL;
	}

	init_list_head(&snd->list);
	snd->source = source;
	snd->sample = sample;
	snd->postion = 0;
	snd->lvol = 4096;
	snd->rvol = 4096;
	snd->gain = 1.0f;
	snd->pan = 0.0f;
	snd->loop = 1;
	snd->cb = NULL;

	return snd;
}

void sound_free(struct sound_t * snd)
{
	if(snd)
	{
		if(snd->source)
			free(snd->source);
		free(snd);
	}
}

static void sound_resample(int16_t * out, int osr, int osample, int16_t * in, int isr, int isample, int channel)
{
	if(out && in)
	{
		float fixed = (1.0 / (1LL << 32));
		uint64_t frac = (1LL << 32);
		uint64_t step = ((uint64_t)((float)isr / (float)osr * frac + 0.5));
		uint64_t offset = 0;
		for(int i = 0; i < osample; i += 1)
		{
			for(int c = 0; c < channel; c++)
				*out++ = (int16_t)(in[c] + (in[c + channel] - in[c]) * ((float)(offset >> 32) + ((offset & (frac - 1)) * fixed)));
			offset += step;
			in += (offset >> 32) * channel;
			offset &= (frac - 1);
		}
	}
}

struct wav_header_t {
	uint8_t riff[4];
	uint32_t riffsz;
	uint8_t wave[4];
	uint8_t fmt[4];
	uint32_t fmtsz;
	uint16_t fmttag;
	uint16_t channel;
	uint32_t samplerate;
	uint32_t byterate;
	uint16_t align;
	uint16_t bps;
	uint8_t data[4];
	uint32_t datasz;
};

static inline struct sound_t * sound_alloc_from_xfs_wav(struct xfs_context_t * ctx, const char * filename)
{
	struct wav_header_t header;
	struct xfs_file_t * file;
	struct sound_t * snd;
	uint32_t tmp[512];
	uint32_t * inbuf;
	int isample, osample;

	if(!(file = xfs_open_read(ctx, filename)))
		return NULL;

	if(xfs_read(file, &header, sizeof(struct wav_header_t)) != sizeof(struct wav_header_t))
	{
		xfs_close(file);
		return NULL;
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

	if( (memcmp(header.riff, "RIFF", 4) != 0) ||
		(memcmp(header.wave, "WAVE", 4) != 0) ||
		(memcmp(header.fmt,  "fmt ", 4) != 0) ||
		(memcmp(header.data, "data", 4) != 0) ||
		(header.fmttag != 1) || (header.datasz < header.align) )
	{
		xfs_close(file);
		return NULL;
	}
	if(((header.channel != 1) && (header.channel != 2)) || ((header.bps != 8) && (header.bps != 16)))
	{
		xfs_close(file);
		return NULL;
	}

	isample = header.datasz / header.align;
	osample = isample * 48000.0 / header.samplerate;
	osample -= osample % 2;
	inbuf = malloc(isample << 2);
	if(!inbuf)
	{
		xfs_close(file);
		return NULL;
	}
	snd = sound_alloc(osample);
	if(!snd)
	{
		free(inbuf);
		xfs_close(file);
		return NULL;
	}
	if(header.channel == 1)
	{
		if(header.bps == 8)
		{
			int16_t * p = (int16_t *)inbuf;
			int8_t * q = (int8_t *)tmp;
			int16_t v;
			s64_t n, i;
			while((n = xfs_read(file, tmp, sizeof(tmp))) > 0)
			{
				for(i = 0; i < n; i++)
				{
					v = q[i] << 8;
					*p++ = v;
					*p++ = v;
				}
			}
		}
		else if(header.bps == 16)
		{
			int16_t * p = (int16_t *)inbuf;
			int16_t * q = (int16_t *)tmp;
			int16_t v;
			s64_t n, i;
			while((n = xfs_read(file, tmp, sizeof(tmp))) > 0)
			{
				for(i = 0; i < (n >> 1); i++)
				{
					v = q[i];
					*p++ = v;
					*p++ = v;
				}
			}
		}
	}
	else if(header.channel == 2)
	{
		if(header.bps == 8)
		{
			int16_t * p = (int16_t *)inbuf;
			int8_t * q = (int8_t *)tmp;
			s64_t n, i;
			while((n = xfs_read(file, tmp, sizeof(tmp))) > 0)
			{
				for(i = 0; i < n; i += 2)
				{
					*p++ = q[i] << 8;
					*p++ = q[i + 1] << 8;
				}
			}
		}
		else if(header.bps == 16)
		{
			xfs_read(file, inbuf, header.datasz);
		}
	}
	sound_resample((int16_t *)snd->source, 48000, osample, (int16_t *)inbuf, header.samplerate, isample, 2);
	free(inbuf);
	xfs_close(file);

	return snd;
}

#define STB_VORBIS_NO_STDIO
#include <stb_vorbis.c.h>
static inline struct sound_t * sound_alloc_from_xfs_ogg(struct xfs_context_t * ctx, const char * filename)
{
	struct xfs_file_t * file;
	struct sound_t * snd;
	uint32_t * inbuf;
	int isample, osample;
	uint8_t * mem;
	s64_t mlen;
	int channel;
	int rate;
	short * ogg;

	if(!(file = xfs_open_read(ctx, filename)))
		return NULL;
	if((mlen = xfs_length(file)) <= 0)
	{
		xfs_close(file);
		return NULL;
	}
	if(!(mem = malloc(mlen)))
	{
		xfs_close(file);
		return NULL;
	}
	if(xfs_read(file, mem, mlen) != mlen)
	{
		xfs_close(file);
		return NULL;
	}
	xfs_close(file);

	isample = stb_vorbis_decode_memory(mem, mlen, &channel, &rate, &ogg);
	if(isample <= 0)
	{
		free(mem);
		return NULL;
	}
	osample = isample * 48000.0 / rate;
	osample -= osample % 2;
	inbuf = malloc(isample << 2);
	if(!inbuf)
	{
		free(mem);
		free(ogg);
		return NULL;
	}
	snd = sound_alloc(osample);
	if(!snd)
	{
		free(mem);
		free(ogg);
		free(inbuf);
		return NULL;
	}
	if(channel == 1)
	{
		int16_t * p = (int16_t *)inbuf;
		int16_t * q = (int16_t *)ogg;
		int16_t v;
		for(int i = 0; i < isample; i++)
		{
			v = q[i];
			*p++ = v;
			*p++ = v;
		}
	}
	else if(channel == 2)
	{
		memcpy(inbuf, ogg, isample << 2);
	}
	sound_resample((int16_t *)snd->source, 48000, osample, (int16_t *)inbuf, rate, isample, 2);
	free(mem);
	free(ogg);
	free(inbuf);

	return snd;
}

struct sound_t * sound_alloc_from_xfs(struct xfs_context_t * ctx, const char * filename)
{
	const char * ext = fileext(filename);
	if(strcasecmp(ext, "wav") == 0)
		return sound_alloc_from_xfs_wav(ctx, filename);
	else if(strcasecmp(ext, "ogg") == 0)
		return sound_alloc_from_xfs_ogg(ctx, filename);
	return NULL;
}

struct sound_t * sound_alloc_tone(int frequency, int millisecond)
{
	struct sound_t * snd;
	uint32_t * p;
	int16_t v;
	float t;
	int sample;
	int i;

	if((frequency > 0) && (frequency < 24000))
	{
		if(millisecond <= 0)
			sample = 48000.0 / (float)frequency;
		else
			sample = millisecond * 48;
		snd = sound_alloc(sample);
		if(snd)
		{
			t = (2 * M_PI / 48000.0) * (float)frequency;
			for(i = 0, p = snd->source; i < sample; i++, p++)
			{
				v = (int16_t)(sinf(t * (float)i) * 32767.0);
				*p = (v << 16) | v;
			}
			if(millisecond > 0)
				snd->loop = 1;
			else
				snd->loop = -1;
			return snd;
		}
	}
	return NULL;
}
