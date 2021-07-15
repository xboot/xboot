/*
 * driver/audio-pwm.c
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
#include <pwm/pwm.h>
#include <audio/audio.h>

struct audio_pwm_pdata_t {
	struct timer_t timer;
	struct fifo_t * fifo;
	struct pwm_t * pwm;
	int polarity;

	enum audio_rate_t rate;
	enum audio_format_t fmt;
	int ch;
	audio_callback_t cb;
	void * data;
	int running;
};

static void audio_pwm_resample(int16_t * out, int osr, int osample, int16_t * in, int isr, int isample)
{
	if(out && in)
	{
		float fixed = (1.0 / (1LL << 32));
		uint64_t frac = (1LL << 32);
		uint64_t step = ((uint64_t)((float)isr / (float)osr * frac + 0.5));
		uint64_t offset = 0;
		for(int i = 0; i < osample; i += 1)
		{
			*out++ = (int16_t)(in[0] + (in[1] - in[0]) * ((float)(offset >> 32) + ((offset & (frac - 1)) * fixed)));
			offset += step;
			in += (offset >> 32);
			offset &= (frac - 1);
		}
	}
}

static int audio_pwm_timer_function(struct timer_t * timer, void * data)
{
	struct audio_t * audio = (struct audio_t *)(data);
	struct audio_pwm_pdata_t * pdat = (struct audio_pwm_pdata_t *)audio->priv;
	uint32_t inbuf[256];
	int16_t outbuf[512];
	int16_t v;
	int isample, osample;
	int i;

	if(pdat->running)
	{
		if(__fifo_len(pdat->fifo) <= 0)
		{
			switch(pdat->fmt)
			{
			case AUDIO_FORMAT_S8:
				if(pdat->ch == 1)
				{
					isample = pdat->cb(pdat->data, inbuf, sizeof(inbuf));
					if(isample > 0)
					{
						osample = isample * 8000.0 / pdat->rate;
						osample -= osample % 2;
						int8_t * p = &((int8_t *)inbuf)[(isample - 1)];
						int16_t * q = &((int16_t *)inbuf)[osample - 1];
						for(i = 0; i < isample; i++)
							*q-- = (int16_t)((int)(*p--) * 128);
						audio_pwm_resample(outbuf, 8000, osample, (int16_t *)inbuf, pdat->rate, isample);
						__fifo_put(pdat->fifo, (unsigned char *)outbuf, osample << 1);
					}
				}
				else if(pdat->ch == 2)
				{
					isample = pdat->cb(pdat->data, inbuf, sizeof(inbuf)) >> 1;
					if(isample > 0)
					{
						osample = isample * 8000.0 / pdat->rate;
						osample -= osample % 2;
						int8_t * p = &((int8_t *)inbuf)[(isample - 1) << 1];
						int16_t * q = &((int16_t *)inbuf)[osample - 1];
						for(i = 0; i < isample; i++, p-=2)
							*q-- = (int16_t)(((int)p[0] + (int)p[1]) * 128);
						audio_pwm_resample(outbuf, 8000, osample, (int16_t *)inbuf, pdat->rate, isample);
						__fifo_put(pdat->fifo, (unsigned char *)outbuf, osample << 1);
					}
				}
				break;
			case AUDIO_FORMAT_S16:
				if(pdat->ch == 1)
				{
					isample = pdat->cb(pdat->data, inbuf, sizeof(inbuf)) >> 1;
					if(isample > 0)
					{
						osample = isample * 8000.0 / pdat->rate;
						osample -= osample % 2;
						audio_pwm_resample(outbuf, 8000, osample, (int16_t *)inbuf, pdat->rate, isample);
						__fifo_put(pdat->fifo, (unsigned char *)outbuf, osample << 1);
					}
				}
				else if(pdat->ch == 2)
				{
					isample = pdat->cb(pdat->data, inbuf, sizeof(inbuf)) >> 2;
					if(isample > 0)
					{
						osample = isample * 8000.0 / pdat->rate;
						osample -= osample % 2;
						int16_t * p = (int16_t *)inbuf;
						int16_t * q = (int16_t *)inbuf;
						for(i = 0; i < isample; i++, p+=2)
							*q++ = (int16_t)(((int)p[0] + (int)p[1]) >> 1);
						audio_pwm_resample(outbuf, 8000, osample, (int16_t *)inbuf, pdat->rate, isample);
						__fifo_put(pdat->fifo, (unsigned char *)outbuf, osample << 1);
					}
				}
				break;
			case AUDIO_FORMAT_S24:
				if(pdat->ch == 1)
				{
					isample = pdat->cb(pdat->data, inbuf, sizeof(inbuf)) / 3;
					if(isample > 0)
					{
						osample = isample * 8000.0 / pdat->rate;
						osample -= osample % 2;
						int8_t * p = (int8_t *)inbuf;
						int16_t * q = (int16_t *)inbuf;
						for(i = 0; i < isample; i++, p+=3)
							*q++ = (int16_t)((int32_t)((p[2] << 24) | (p[1] << 16) | (p[0] << 8)) >> 16);
						audio_pwm_resample(outbuf, 8000, osample, (int16_t *)inbuf, pdat->rate, isample);
						__fifo_put(pdat->fifo, (unsigned char *)outbuf, osample << 1);
					}
				}
				else if(pdat->ch == 2)
				{
					isample = pdat->cb(pdat->data, inbuf, sizeof(inbuf)) / 6;
					if(isample > 0)
					{
						osample = isample * 8000.0 / pdat->rate;
						osample -= osample % 2;
						int8_t * p = (int8_t *)inbuf;
						int16_t * q = (int16_t *)inbuf;
						for(i = 0; i < isample; i++, p+=6)
						{
							int32_t l = (int32_t)((p[2] << 24) | (p[1] << 16) | (p[0] << 8));
							int32_t r = (int32_t)((p[5] << 24) | (p[4] << 16) | (p[3] << 8));
							*q++ = (int16_t)((l + r) >> 17);
						}
						audio_pwm_resample(outbuf, 8000, osample, (int16_t *)inbuf, pdat->rate, isample);
						__fifo_put(pdat->fifo, (unsigned char *)outbuf, osample << 1);
					}
				}
				break;
			case AUDIO_FORMAT_S32:
				if(pdat->ch == 1)
				{
					isample = pdat->cb(pdat->data, inbuf, sizeof(inbuf)) >> 2;
					if(isample > 0)
					{
						osample = isample * 8000.0 / pdat->rate;
						osample -= osample % 2;
						int32_t * p = (int32_t *)inbuf;
						int16_t * q = (int16_t *)inbuf;
						for(i = 0; i < isample; i++)
							*q++ = (int16_t)(*p++ >> 16);
						audio_pwm_resample(outbuf, 8000, osample, (int16_t *)inbuf, pdat->rate, isample);
						__fifo_put(pdat->fifo, (unsigned char *)outbuf, osample << 1);
					}
				}
				else if(pdat->ch == 2)
				{
					isample = pdat->cb(pdat->data, inbuf, sizeof(inbuf)) >> 3;
					if(isample > 0)
					{
						osample = isample * 8000.0 / pdat->rate;
						osample -= osample % 2;
						int32_t * p = (int32_t *)inbuf;
						int16_t * q = (int16_t *)inbuf;
						for(i = 0; i < isample; i++, p+=2)
							*q++ = (int16_t)((p[0] + p[1]) >> 17);
						audio_pwm_resample(outbuf, 8000, osample, (int16_t *)inbuf, pdat->rate, isample);
						__fifo_put(pdat->fifo, (unsigned char *)outbuf, osample << 1);
					}
				}
				break;
			default:
				break;
			}
		}
		if(__fifo_get(pdat->fifo, (unsigned char *)&v, 2) == 2)
		{
			pwm_config(pdat->pwm, ((((int)v + 32768) * 16000) >> 16), 16000, pdat->polarity);
			pwm_enable(pdat->pwm);
			timer_forward(&pdat->timer, us_to_ktime(125));
			return 1;
		}
	}
	__fifo_reset(pdat->fifo);
	pwm_disable(pdat->pwm);
	pdat->running = 0;
	return 0;
}

static void audio_pwm_playback_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
	struct audio_pwm_pdata_t * pdat = (struct audio_pwm_pdata_t *)audio->priv;

	if(!pdat->running)
	{
		pdat->rate = rate;
		pdat->fmt = fmt;
		pdat->ch = ch;
		pdat->cb = cb;
		pdat->data = data;
		timer_start(&pdat->timer, ms_to_ktime(1));
		pdat->running = 1;
	}
}

static void audio_pwm_playback_stop(struct audio_t * audio)
{
	struct audio_pwm_pdata_t * pdat = (struct audio_pwm_pdata_t *)audio->priv;
	pdat->cb = NULL;
	pdat->data = NULL;
}

static int audio_pwm_ioctl(struct audio_t * audio, const char * cmd, void * arg)
{
	return -1;
}

static struct device_t * audio_pwm_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct audio_pwm_pdata_t * pdat;
	struct pwm_t * pwm;
	struct audio_t * audio;
	struct device_t * dev;

	if(!(pwm = search_pwm(dt_read_string(n, "pwm-name", NULL))))
		return NULL;

	pdat = malloc(sizeof(struct audio_pwm_pdata_t));
	if(!pdat)
		return NULL;

	audio = malloc(sizeof(struct audio_t));
	if(!audio)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, audio_pwm_timer_function, audio);
	pdat->fifo = fifo_alloc(1024);
	pdat->pwm = pwm;
	pdat->polarity = dt_read_bool(n, "pwm-polarity", 1);
	pdat->running = 0;

	audio->name = alloc_device_name(dt_read_name(n), -1);
	audio->playback_start = audio_pwm_playback_start;
	audio->playback_stop = audio_pwm_playback_stop;
	audio->capture_start = NULL;
	audio->capture_stop = NULL;
	audio->ioctl = audio_pwm_ioctl;
	audio->priv = pdat;

	if(!(dev = register_audio(audio, drv)))
	{
		timer_cancel(&pdat->timer);
		fifo_free(pdat->fifo);
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
		return NULL;
	}
	return dev;
}

static void audio_pwm_remove(struct device_t * dev)
{
	struct audio_t * audio = (struct audio_t *)dev->priv;
	struct audio_pwm_pdata_t * pdat = (struct audio_pwm_pdata_t *)audio->priv;

	if(audio)
	{
		unregister_audio(audio);
		timer_cancel(&pdat->timer);
		fifo_free(pdat->fifo);
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
	}
}

static void audio_pwm_suspend(struct device_t * dev)
{
}

static void audio_pwm_resume(struct device_t * dev)
{
}

static struct driver_t audio_pwm = {
	.name		= "audio-pwm",
	.probe		= audio_pwm_probe,
	.remove		= audio_pwm_remove,
	.suspend	= audio_pwm_suspend,
	.resume		= audio_pwm_resume,
};

static __init void audio_pwm_driver_init(void)
{
	register_driver(&audio_pwm);
}

static __exit void audio_pwm_driver_exit(void)
{
	unregister_driver(&audio_pwm);
}

driver_initcall(audio_pwm_driver_init);
driver_exitcall(audio_pwm_driver_exit);
