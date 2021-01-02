/*
 * driver/audio-v831.c
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
#include <reset/reset.h>
#include <audio/audio.h>

enum {
	AUDIO_DAC_DPC			= 0x000,
	AUDIO_DAC_FIFOC			= 0x010,
	AUDIO_DAC_FIFOS			= 0x014,
	AUDIO_DAC_TXDATA		= 0x020,
	AUDIO_DAC_CNT			= 0x024,
	AUDIO_DAC_DG			= 0x028,

	AUDIO_ADC_FIFOC			= 0x030,
	AUDIO_ADC_FIFOS			= 0x038,
	AUDIO_ADC_RXDATA		= 0x040,
	AUDIO_ADC_CNT			= 0x044,
	AUDIO_ADC_DG			= 0x04c,

	AUDIO_DAC_DAP_CTL		= 0x0f0,
	AUDIO_ADC_DAP_CTL		= 0x0f8,

	AUDIO_DAC_DRC_HHPFC		= 0x100,
	AUDIO_DAC_DRC_LHPFC		= 0x104,
	AUDIO_DAC_DRC_CTRL		= 0x108,
	AUDIO_DAC_DRC_LPFHAT	= 0x10c,
	AUDIO_DAC_DRC_LPFLAT	= 0x110,
	AUDIO_DAC_DRC_RPFHAT	= 0x114,
	AUDIO_DAC_DRC_RPFLAT	= 0x118,
	AUDIO_DAC_DRC_LPFHRT	= 0x11c,
	AUDIO_DAC_DRC_LPFLRT	= 0x120,
	AUDIO_DAC_DRC_RPFHRT	= 0x124,
	AUDIO_DAC_DRC_RPFLRT	= 0x128,
	AUDIO_DAC_DRC_LRMSHAT	= 0x12c,
	AUDIO_DAC_DRC_LRMSLAT	= 0x130,
	AUDIO_DAC_DRC_RRMSHAT	= 0x134,
	AUDIO_DAC_DRC_RRMSLAT	= 0x138,
	AUDIO_DAC_DRC_HCT		= 0x13c,
	AUDIO_DAC_DRC_LCT		= 0x140,
	AUDIO_DAC_DRC_HKC		= 0x144,
	AUDIO_DAC_DRC_LKC		= 0x148,
	AUDIO_DAC_DRC_HOPC 		= 0x14c,
	AUDIO_DAC_DRC_LOPC 		= 0x150,
	AUDIO_DAC_DRC_HLT 		= 0x154,
	AUDIO_DAC_DRC_LLT 		= 0x158,
	AUDIO_DAC_DRC_HKI 		= 0x15c,
	AUDIO_DAC_DRC_LKI 		= 0x160,
	AUDIO_DAC_DRC_HOPL 		= 0x164,
	AUDIO_DAC_DRC_LOPL 		= 0x168,
	AUDIO_DAC_DRC_HET 		= 0x16c,
	AUDIO_DAC_DRC_LET 		= 0x170,
	AUDIO_DAC_DRC_HKE 		= 0x174,
	AUDIO_DAC_DRC_LKE 		= 0x178,
	AUDIO_DAC_DRC_HOPE 		= 0x17c,
	AUDIO_DAC_DRC_LOPE 		= 0x180,
	AUDIO_DAC_DRC_HKN 		= 0x184,
	AUDIO_DAC_DRC_LKN 		= 0x188,
	AUDIO_DAC_DRC_SFHAT 	= 0x18c,
	AUDIO_DAC_DRC_SFLAT 	= 0x190,
	AUDIO_DAC_DRC_SFHRT 	= 0x194,
	AUDIO_DAC_DRC_SFLRT		= 0x198,
	AUDIO_DAC_DRC_MXGHS		= 0x19c,
	AUDIO_DAC_DRC_MXGLS		= 0x1a0,
	AUDIO_DAC_DRC_MNGHS		= 0x1a4,
	AUDIO_DAC_DRC_MNGLS		= 0x1a8,
	AUDIO_DAC_DRC_EPSHC		= 0x1ac,
	AUDIO_DAC_DRC_EPSLC		= 0x1b0,
	AUDIO_DAC_DRC_OPT		= 0x1b4,
	AUDIO_DAC_DRC_HPFHGAIN	= 0x1b8,
	AUDIO_DAC_DRC_HPFLGAIN	= 0x1bc,

	AUDIO_ADC_DRC_HHPFC		= 0x200,
	AUDIO_ADC_DRC_LHPFC		= 0x204,
	AUDIO_ADC_DRC_CTRL		= 0x208,
	AUDIO_ADC_DRC_LPFHAT	= 0x20c,
	AUDIO_ADC_DRC_LPFLAT	= 0x210,
	AUDIO_ADC_DRC_RPFHAT	= 0x214,
	AUDIO_ADC_DRC_RPFLAT	= 0x218,
	AUDIO_ADC_DRC_LPFHRT	= 0x21c,
	AUDIO_ADC_DRC_LPFLRT	= 0x220,
	AUDIO_ADC_DRC_RPFHRT	= 0x224,
	AUDIO_ADC_DRC_RPFLRT	= 0x228,
	AUDIO_ADC_DRC_LRMSHAT	= 0x22c,
	AUDIO_ADC_DRC_LRMSLAT	= 0x230,
	AUDIO_ADC_DRC_HCT		= 0x23c,
	AUDIO_ADC_DRC_LCT		= 0x240,
	AUDIO_ADC_DRC_HKC		= 0x244,
	AUDIO_ADC_DRC_LKC		= 0x248,
	AUDIO_ADC_DRC_HOPC		= 0x24c,
	AUDIO_ADC_DRC_LOPC		= 0x250,
	AUDIO_ADC_DRC_HLT		= 0x254,
	AUDIO_ADC_DRC_LLT		= 0x258,
	AUDIO_ADC_DRC_HKI		= 0x25c,
	AUDIO_ADC_DRC_LKI		= 0x260,
	AUDIO_ADC_DRC_HOPL		= 0x264,
	AUDIO_ADC_DRC_LOPL		= 0x268,
	AUDIO_ADC_DRC_HET		= 0x26c,
	AUDIO_ADC_DRC_LET		= 0x270,
	AUDIO_ADC_DRC_HKE		= 0x274,
	AUDIO_ADC_DRC_LKE		= 0x278,
	AUDIO_ADC_DRC_HOPE		= 0x27c,
	AUDIO_ADC_DRC_LOPE		= 0x280,
	AUDIO_ADC_DRC_HKN		= 0x284,
	AUDIO_ADC_DRC_LKN		= 0x288,
	AUDIO_ADC_DRC_SFHAT		= 0x28c,
	AUDIO_ADC_DRC_SFLAT		= 0x290,
	AUDIO_ADC_DRC_SFHRT		= 0x294,
	AUDIO_ADC_DRC_SFLRT		= 0x298,
	AUDIO_ADC_DRC_MXGHS		= 0x29c,
	AUDIO_ADC_DRC_MXGLS		= 0x2a0,
	AUDIO_ADC_DRC_MNGHS		= 0x2a4,
	AUDIO_ADC_DRC_MNGLS		= 0x2a8,
	AUDIO_ADC_DRC_EPSHC		= 0x2ac,
	AUDIO_ADC_DRC_EPSLC		= 0x2b0,
	AUDIO_ADC_DRC_OPT		= 0x2b4,
	AUDIO_ADC_DRC_HPFHGAIN	= 0x2b8,
	AUDIO_ADC_DRC_HPFLGAIN	= 0x2bc,

	AUDIO_ADCL_ANA_CTL		= 0x300,
	AUDIO_DAC_ANA_CTL		= 0x310,
	AUDIO_MICBIAS_ANA_CTL	= 0x318,
	AUDIO_BIAS_ANA_CTL		= 0x320,
};

struct audio_v831_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int reset;
};

static void audio_v831_playback_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
	struct audio_v831_pdata_t * pdat = (struct audio_v831_pdata_t *)audio->priv;
	(void)pdat;
}

static void audio_v831_playback_stop(struct audio_t * audio)
{
}

static void audio_v831_capture_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data)
{
}

static void audio_v831_capture_stop(struct audio_t * audio)
{
}

static int audio_v831_ioctl(struct audio_t * audio, const char * cmd, void * arg)
{
	return -1;
}

static struct device_t * audio_v831_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct audio_v831_pdata_t * pdat;
	struct audio_t * audio;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct audio_v831_pdata_t));
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
	pdat->reset = dt_read_int(n, "reset", -1);

	audio->name = alloc_device_name(dt_read_name(n), -1);
	audio->playback_start = audio_v831_playback_start;
	audio->playback_stop = audio_v831_playback_stop;
	audio->capture_start = audio_v831_capture_start;
	audio->capture_stop = audio_v831_capture_stop;
	audio->ioctl = audio_v831_ioctl;
	audio->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);

	if(!(dev = register_audio(audio, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
		return NULL;
	}
	return dev;
}

static void audio_v831_remove(struct device_t * dev)
{
	struct audio_t * audio = (struct audio_t *)dev->priv;
	struct audio_v831_pdata_t * pdat = (struct audio_v831_pdata_t *)audio->priv;

	if(audio)
	{
		unregister_audio(audio);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
	}
}

static void audio_v831_suspend(struct device_t * dev)
{
}

static void audio_v831_resume(struct device_t * dev)
{
}

static struct driver_t audio_v831 = {
	.name		= "audio-v831",
	.probe		= audio_v831_probe,
	.remove		= audio_v831_remove,
	.suspend	= audio_v831_suspend,
	.resume		= audio_v831_resume,
};

static __init void audio_v831_driver_init(void)
{
	register_driver(&audio_v831);
}

static __exit void audio_v831_driver_exit(void)
{
	unregister_driver(&audio_v831);
}

driver_initcall(audio_v831_driver_init);
driver_exitcall(audio_v831_driver_exit);
