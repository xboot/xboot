/*
 * driver/audio-f1c200s.c
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
#include <clk/clk.h>
#include <dma/dma.h>
#include <gpio/gpio.h>
#include <reset/reset.h>
#include <interrupt/interrupt.h>
#include <audio/audio.h>
#include <f1c200s-dma.h>

#define snd_update_bits(a, b, c, s) \
	write32(a, (read32(a) & (~((u32_t)(c) << b))) | ((u32_t)(s) << b))

enum {
	AUDIO_DAC_DPC				= 0x00,
	AUDIO_DAC_FIFOC				= 0x04,
	AUDIO_DAC_FIFOS				= 0x08,
	AUDIO_DAC_TXDATA			= 0x0c,
	AUDIO_ADC_FIFOC				= 0x10,
	AUDIO_ADC_FIFOS				= 0x14,
	AUDIO_ADC_RXDATA			= 0x18,
	AUDIO_DAC_MIXER_CTRL		= 0x20,
	AUDIO_ADC_MIXER_CTRL		= 0x24,
	AUDIO_ADDA_TUNE				= 0x28,
	AUDIO_BIAS_DA16_CAL_CTRL0	= 0x2c,
	AUDIO_BIAS_DA16_CAL_CTRL1	= 0x34,
	AUDIO_AC_DAC_CNT			= 0x40,
	AUDIO_AC_ADC_CNT			= 0x44,
	AUDIO_AC_DAC_DG				= 0x48,
	AUDIO_AC_ADC_DG				= 0x4c,
	AUDIO_AC_ADC_DAP_CTR		= 0x70,
	AUDIO_AC_ADC_DAP_LCTR		= 0x74,
	AUDIO_AC_ADC_DAP_RCTR		= 0x78,
	AUDIO_AC_ADC_DAP_PARA		= 0x7c,
	AUDIO_AC_ADC_DAP_LAC		= 0x80,
	AUDIO_AC_ADC_DAP_LDAT		= 0x84,
	AUDIO_AC_ADC_DAP_RAC		= 0x88,
	AUDIO_AC_ADC_DAP_RDAT		= 0x8c,
	AUDIO_ADC_DAP_HPFC			= 0x90,
	AUDIO_ADC_DAP_LINAC			= 0x94,
	AUDIO_ADC_DAP_RINAC			= 0x98,
	AUDIO_ADC_DAP_ORT			= 0x9c,
};

#define DAC_DRQ_CLR_CNT			21
#define FIFO_FLUSH				0
#define FIR_VER			28
#define ADC_FIFO_FLUSH		0
#define PH_COM_PROTEC		21
#define PH_COM_FC		22
#define HP_VOL			0
#define EN_DAC			31
#define HP_POWER_EN		15
#define L_MIXER_MUTE_R_DAC	9
#define L_MIXER_MUTE_L_DAC	8
#define R_MIXER_MUTE_R_DAC	17
#define R_MIXER_MUTE_L_DAC	16
#define PH_L_MUTE		26
#define PH_R_MUTE		27
#define DAC_AG_R_EN		31
#define DAC_AG_L_EN		30

struct audio_f1c200s_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int irq;
	int reset;
	int dma_playback;
	int dma_capture;

	struct {
		int16_t buffer[4096];
		audio_callback_t cb;
		void * data;
		int dmaflag;
		int running;
	} playback;
};

static inline u64_t f1c200s_audio_get_clk(enum audio_rate_t rate)
{
	u64_t clk;

	switch(rate)
	{
	case AUDIO_RATE_7350:
	case AUDIO_RATE_11025:
	case AUDIO_RATE_14700:
	case AUDIO_RATE_22050:
	case AUDIO_RATE_29400:
	case AUDIO_RATE_44100:
	case AUDIO_RATE_88200:
	case AUDIO_RATE_176400:
		clk = 22579200 * 8;
		break;
	case AUDIO_RATE_8000:
	case AUDIO_RATE_12000:
	case AUDIO_RATE_16000:
	case AUDIO_RATE_24000:
	case AUDIO_RATE_32000:
	case AUDIO_RATE_48000:
	case AUDIO_RATE_96000:
	case AUDIO_RATE_192000:
		clk = 24576000 * 8;
		break;
	default:
		clk = 24576000 * 8;
		break;
	}
	return clk;
}

static inline int f1c200s_audio_get_fs(enum audio_rate_t rate)
{
	int fs;

	switch(rate)
	{
	case AUDIO_RATE_7350:
	case AUDIO_RATE_8000:
		fs = 5;
		break;
	case AUDIO_RATE_11025:
	case AUDIO_RATE_12000:
		fs = 4;
		break;
	case AUDIO_RATE_14700:
	case AUDIO_RATE_16000:
		fs = 3;
		break;
	case AUDIO_RATE_22050:
	case AUDIO_RATE_24000:
		fs = 2;
		break;
	case AUDIO_RATE_29400:
	case AUDIO_RATE_32000:
		fs = 1;
		break;
	case AUDIO_RATE_44100:
	case AUDIO_RATE_48000:
		fs = 0;
		break;
	case AUDIO_RATE_88200:
	case AUDIO_RATE_96000:
		fs = 7;
		break;
	case AUDIO_RATE_176400:
	case AUDIO_RATE_192000:
		fs = 6;
		break;
	default:
		fs = 0;
		break;
	}
	return fs;
}

static inline void f1c200s_audio_init(struct audio_f1c200s_pdata_t * pdat)
{
	gpio_direction_output(1, 1);

	snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, DAC_DRQ_CLR_CNT, 0x3, 0x3);
	snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, FIR_VER, 0x1, 0x0);
	snd_update_bits(pdat->virt + AUDIO_ADC_FIFOC, ADC_FIFO_FLUSH, 0x1, 0x1);
	snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 4, 0x1, 0x0);
	snd_update_bits(pdat->virt + AUDIO_DAC_DPC, 31, 0x1, 0x0);

	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, PH_COM_PROTEC, 0x1, 0x1);
	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, PH_COM_FC, 0x3, 0x3);
	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, HP_VOL, 0x3f, 0x38);

	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, HP_POWER_EN, 0x1, 0x1);
	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, L_MIXER_MUTE_R_DAC, 0x1, 0x1);
	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, L_MIXER_MUTE_L_DAC, 0x1, 0x1);
	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, R_MIXER_MUTE_R_DAC, 0x1, 0x1);
	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, R_MIXER_MUTE_L_DAC, 0x1, 0x1);

	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, PH_L_MUTE, 0x1, 0x1);
	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, PH_R_MUTE, 0x1, 0x1);

	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, DAC_AG_L_EN, 0x1, 0x1);
	snd_update_bits(pdat->virt + AUDIO_DAC_MIXER_CTRL, DAC_AG_R_EN, 0x1, 0x1);
}

static void audio_f1c200s_playback_complete(void * data)
{
	struct audio_t * audio = (struct audio_t *)data;
	struct audio_f1c200s_pdata_t * pdat = (struct audio_f1c200s_pdata_t *)audio->priv;
	int len;

	if(pdat->playback.running)
	{
		len = pdat->playback.cb(pdat->playback.data, pdat->playback.buffer, sizeof(pdat->playback.buffer));
		if(len > 0)
			dma_start(pdat->dma_playback, (void *)pdat->playback.buffer, (void *)pdat->virt + AUDIO_DAC_TXDATA, len, pdat->playback.dmaflag, audio_f1c200s_playback_complete, audio);
		else
		{
			pdat->playback.running = 0;
			snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 4, 0x1, 0x0);
			snd_update_bits(pdat->virt + AUDIO_DAC_DPC, 31, 0x1, 0x0);
		}
	}
}

static void audio_f1c200s_playback_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
	struct audio_f1c200s_pdata_t * pdat = (struct audio_f1c200s_pdata_t *)audio->priv;
	int len;

	if(!pdat->playback.running)
	{
		clk_set_rate("pll-audio", f1c200s_audio_get_clk(rate));
		snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 29, 0x7, f1c200s_audio_get_fs(rate));
		switch(fmt)
		{
		case AUDIO_FORMAT_S16:
			snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 5, 0x1, 0x0);
			snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 24, 0x3, 0x3);
			break;
		case AUDIO_FORMAT_S24:
			snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 5, 0x1, 0x1);
			snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 24, 0x3, 0x0);
			break;
		default:
			return;
		}
		switch(ch)
		{
		case 1:
			snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 6, 0x1, 0x1);
			break;
		case 2:
			snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 6, 0x1, 0x0);
			break;
		default:
			return;
		}
		snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 0, 0x1, 0x1);
		snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 8, 0x3f, 0xf);
		if(rate > 320000)
			snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 28, 0x1, 0x0);
		else
			snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 28, 0x1, 0x1);
		snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 26, 0x1, 0x0);
		snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 0, 0x1, 0x1);
		snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 4, 0x1, 0x1);
		snd_update_bits(pdat->virt + AUDIO_DAC_DPC, 31, 0x1, 0x1);

		pdat->playback.dmaflag = DMA_S_TYPE(DMA_TYPE_MEMTODEV);
		pdat->playback.dmaflag |= DMA_S_SRC_INC(DMA_INCREASE) | DMA_S_DST_INC(DMA_CONSTANT);
		pdat->playback.dmaflag |= DMA_S_SRC_WIDTH(DMA_WIDTH_16BIT) | DMA_S_DST_WIDTH(DMA_WIDTH_16BIT);
		pdat->playback.dmaflag |= DMA_S_SRC_BURST(DMA_BURST_SIZE_1) | DMA_S_DST_BURST(DMA_BURST_SIZE_1);
		pdat->playback.dmaflag |= DMA_S_SRC_PORT(F1C200S_NDMA_PORT_SDRAM) | DMA_S_DST_PORT(F1C200S_NDMA_PORT_AUDIO);
		pdat->playback.cb = cb;
		pdat->playback.data = data;
		pdat->playback.running = 1;
		len = pdat->playback.cb(pdat->playback.data, pdat->playback.buffer, ch * fmt / 8);
		dma_start(pdat->dma_playback, (void *)pdat->playback.buffer, (void *)pdat->virt + AUDIO_DAC_TXDATA, len, pdat->playback.dmaflag, audio_f1c200s_playback_complete, audio);
	}
}

static void audio_f1c200s_playback_stop(struct audio_t * audio)
{
	struct audio_f1c200s_pdata_t * pdat = (struct audio_f1c200s_pdata_t *)audio->priv;
	pdat->playback.running = 0;
	snd_update_bits(pdat->virt + AUDIO_DAC_FIFOC, 4, 0x1, 0x0);
	snd_update_bits(pdat->virt + AUDIO_DAC_DPC, 31, 0x1, 0x0);
}

static void audio_f1c200s_capture_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
}

static void audio_f1c200s_capture_stop(struct audio_t * audio)
{
}

static int audio_f1c200s_ioctl(struct audio_t * audio, const char * cmd, void * arg)
{
	switch(shash(cmd))
	{
	case 0x892b3889: /* "audio-set-playback-volume" */
		break;
	case 0x3eee6d7d: /* "audio-get-playback-volume" */
		break;
	case 0x6dab0056: /* "audio-set-capture-volume" */
		break;
	case 0x44a166ca: /* "audio-get-capture-volume" */
		break;
	default:
		break;
	}
	return -1;
}

static void f1c200s_audio_interrupt(void * data)
{
	struct audio_t * audio = (struct audio_t *)data;
	struct audio_f1c200s_pdata_t * pdat = (struct audio_f1c200s_pdata_t *)audio->priv;
	write32(pdat->virt + AUDIO_DAC_FIFOS, read32(pdat->virt + AUDIO_DAC_FIFOS));
	write32(pdat->virt + AUDIO_ADC_FIFOS, read32(pdat->virt + AUDIO_ADC_FIFOS));
}

static struct device_t * audio_f1c200s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct audio_f1c200s_pdata_t * pdat;
	struct audio_t * audio;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);
	int dma_playback = dt_read_int(n, "dma-channel-playback", -1);
	int dma_capture = dt_read_int(n, "dma-channel-capture", -1);

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	if(!dma_is_valid(dma_playback))
		return NULL;

	if(!dma_is_valid(dma_capture))
		return NULL;

	pdat = malloc(sizeof(struct audio_f1c200s_pdata_t));
	if(!pdat)
		return NULL;

	audio = malloc(sizeof(struct audio_t));
	if(!audio)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->irq = irq;
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->dma_playback = dma_playback;
	pdat->dma_capture = dma_capture;
	pdat->playback.running = 0;

	audio->name = alloc_device_name(dt_read_name(n), -1);
	audio->playback_start = audio_f1c200s_playback_start;
	audio->playback_stop = audio_f1c200s_playback_stop;
	audio->capture_start = audio_f1c200s_capture_start;
	audio->capture_stop = audio_f1c200s_capture_stop;
	audio->ioctl = audio_f1c200s_ioctl;
	audio->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	request_irq(pdat->irq, f1c200s_audio_interrupt, IRQ_TYPE_NONE, audio);
	f1c200s_audio_init(pdat);

	if(!(dev = register_audio(audio, drv)))
	{
		free_irq(pdat->irq);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
		return NULL;
	}
	return dev;
}

static void audio_f1c200s_remove(struct device_t * dev)
{
	struct audio_t * audio = (struct audio_t *)dev->priv;
	struct audio_f1c200s_pdata_t * pdat = (struct audio_f1c200s_pdata_t *)audio->priv;

	if(audio)
	{
		unregister_audio(audio);
		free_irq(pdat->irq);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
	}
}

static void audio_f1c200s_suspend(struct device_t * dev)
{
}

static void audio_f1c200s_resume(struct device_t * dev)
{
}

static struct driver_t audio_f1c200s = {
	.name		= "audio-f1c200s",
	.probe		= audio_f1c200s_probe,
	.remove		= audio_f1c200s_remove,
	.suspend	= audio_f1c200s_suspend,
	.resume		= audio_f1c200s_resume,
};

static __init void audio_f1c200s_driver_init(void)
{
	register_driver(&audio_f1c200s);
}

static __exit void audio_f1c200s_driver_exit(void)
{
	unregister_driver(&audio_f1c200s);
}

driver_initcall(audio_f1c200s_driver_init);
driver_exitcall(audio_f1c200s_driver_exit);
