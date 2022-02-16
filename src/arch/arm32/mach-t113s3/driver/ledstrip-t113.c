/*
 * driver/led/ledstrip-t113.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <gpio/gpio.h>
#include <led/ledstrip.h>

enum {
	LEDC_CTRL				= 0x00,
	LED_T01_TIMING_CTRL		= 0x04,
	LEDC_DATA_FINISH_CNT	= 0x08,
	LED_RST_TIMING_CTRL		= 0x0c,
	LEDC_WAIT_TIME0_CTRL	= 0x10,
	LEDC_DATA				= 0x14,
	LEDC_DMA_CTRL			= 0x18,
	LEDC_INTC				= 0x1c,
	LEDC_INTS				= 0x20,
	LEDC_WAIT_TIME1_CTRL	= 0x28,
	LEDC_VER_NUM			= 0x2c,
	LEDC_FIFO_DATA0			= 0x30,
	LEDC_FIFO_DATA1			= 0x34,
	LEDC_FIFO_DATA2			= 0x38,
};

struct ledstrip_t113_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
	int gpio;
	int gpiocfg;
	int reset_ns;
	int mode;
	int t1h_ns;
	int t1l_ns;
	int t0h_ns;
	int t0l_ns;
	int wait_time0_ns;
	int wait_time1_ns;
	int wait_data_time_ns;
	int count;
	struct color_t * color;
};

static inline void ledc_set_reset_ns(struct ledstrip_t113_pdata_t * pdat, u32_t ns)
{
	u32_t n, val;

	if((ns < 84) || (ns > 327000))
		return;
	n = (ns - 42) / 42;
	val = read32(pdat->virt + LED_RST_TIMING_CTRL);
	val &= ~(0x1fff << 16);
	val |= n << 16;
	write32(pdat->virt + LED_RST_TIMING_CTRL, val);
}

static inline void ledc_set_t1h_ns(struct ledstrip_t113_pdata_t * pdat, u32_t ns)
{
	u32_t n, val;

	if((ns < 84) || (ns > 2560))
		return;
	n = (ns - 42) / 42;
	val = read32(pdat->virt + LED_T01_TIMING_CTRL);
	val &= ~(0x3f << 21);
	val |= n << 21;
	write32(pdat->virt + LED_T01_TIMING_CTRL, val);
}

static inline void ledc_set_t1l_ns(struct ledstrip_t113_pdata_t * pdat, u32_t ns)
{
	u32_t n, val;

	if((ns < 84) || (ns > 1280))
		return;
	n = (ns - 42) / 42;
	val = read32(pdat->virt + LED_T01_TIMING_CTRL);
	val &= ~(0x1f << 16);
	val |= n << 16;
	write32(pdat->virt + LED_T01_TIMING_CTRL, val);
}

static inline void ledc_set_t0h_ns(struct ledstrip_t113_pdata_t * pdat, u32_t ns)
{
	u32_t n, val;

	if((ns < 84) || (ns > 1280))
		return;
	n = (ns - 42) / 42;
	val = read32(pdat->virt + LED_T01_TIMING_CTRL);
	val &= ~(0x1f << 6);
	val |= n << 6;
	write32(pdat->virt + LED_T01_TIMING_CTRL, val);
}

static inline void ledc_set_t0l_ns(struct ledstrip_t113_pdata_t * pdat, u32_t ns)
{
	u32_t n, val;

	if((ns < 84) || (ns > 2560))
		return;
	n = (ns - 42) / 42;
	val = read32(pdat->virt + LED_T01_TIMING_CTRL);
	val &= ~(0x3f << 0);
	val |= n << 0;
	write32(pdat->virt + LED_T01_TIMING_CTRL, val);
}

static inline void ledc_set_wait_time0_ns(struct ledstrip_t113_pdata_t * pdat, u32_t ns)
{
	u32_t n, val;

	if((ns < 84) || (ns > 10000))
		return;
	n = (ns - 42) / 42;
	val = ((1 << 8) | (n << 0));
	write32(pdat->virt + LEDC_WAIT_TIME0_CTRL, val);
}

static inline void ledc_set_wait_time1_ns(struct ledstrip_t113_pdata_t * pdat, u32_t ns)
{
	u64_t tmp;
	u32_t n, val;

	if((ns < 84) || (ns > 85000000000))
		return;
	tmp = ns;
	n = do_udiv64(tmp, 42, NULL);
	n -= 1;
	val = ((1 << 31) | (n << 0));
	write32(pdat->virt + LEDC_WAIT_TIME1_CTRL, val);
}

static inline void ledc_set_wait_data_time_ns(struct ledstrip_t113_pdata_t * pdat, u32_t ns)
{
	u32_t n, val;

	if((ns < 84) || (ns > 655000))
		return;
	n = (ns - 42) / 42;
	val = 0;
	val &= ~(0x1fff << 16);
	val |= (n << 16);
	write32(pdat->virt + LEDC_DATA_FINISH_CNT, val);
}

static inline void ledc_set_length(struct ledstrip_t113_pdata_t * pdat, int length)
{
	u32_t val;

	if(length <= 0)
		return;

	val = read32(pdat->virt + LEDC_CTRL);
	val &= ~(0x1fff << 16);
	val |= length << 16;
	write32(pdat->virt + LEDC_CTRL, val);

	val = read32(pdat->virt + LED_RST_TIMING_CTRL);
	val &= ~0x3ff;
	val |= length - 1;
	write32(pdat->virt + LED_RST_TIMING_CTRL, val);
}

static inline void ledc_set_output_mode(struct ledstrip_t113_pdata_t * pdat, int m)
{
	u32_t val;

	val = read32(pdat->virt + LEDC_CTRL);
	val &= ~(0x7 << 6);
	val |= m << 6;
	write32(pdat->virt + LEDC_CTRL, val);
}

static inline void ledc_set_dma_mode(struct ledstrip_t113_pdata_t * pdat, int en)
{
	u32_t val;

	val = read32(pdat->virt + LEDC_DMA_CTRL);
	if(en)
		val |= 0x1 << 5;
	else
		val &= ~(0x1 << 5);
	write32(pdat->virt + LEDC_DMA_CTRL, val);
}

static inline void ledc_reset_en(struct ledstrip_t113_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt + LEDC_CTRL);
	val |= 1 << 10;
	write32(pdat->virt + LEDC_CTRL, val);
}

static inline void ledc_enable(struct ledstrip_t113_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt + LEDC_CTRL);
	val |= 1 << 0;
	write32(pdat->virt + LEDC_CTRL, val);
}

static void ledc_init(struct ledstrip_t113_pdata_t * pdat)
{
	ledc_reset_en(pdat);
	ledc_set_reset_ns(pdat, pdat->reset_ns);
	ledc_set_t1h_ns(pdat, pdat->t1h_ns);
	ledc_set_t1l_ns(pdat, pdat->t1l_ns);
	ledc_set_t0h_ns(pdat, pdat->t0h_ns);
	ledc_set_t0l_ns(pdat, pdat->t0l_ns);
	ledc_set_wait_time0_ns(pdat, pdat->wait_time0_ns);
	ledc_set_wait_time1_ns(pdat, pdat->wait_time1_ns);
	ledc_set_wait_data_time_ns(pdat, pdat->wait_data_time_ns);
}

static void ledstrip_t113_set_count(struct ledstrip_t * strip, int n)
{
	struct ledstrip_t113_pdata_t * pdat = (struct ledstrip_t113_pdata_t *)strip->priv;

	if((n != pdat->count) && (n > 0))
	{
		if(pdat->color)
			free(pdat->color);
		pdat->count = n;
		pdat->color = malloc(pdat->count * sizeof(struct color_t));
		memset(pdat->color, 0, pdat->count * sizeof(struct color_t));
	}
}

static int ledstrip_t113_get_count(struct ledstrip_t * strip)
{
	struct ledstrip_t113_pdata_t * pdat = (struct ledstrip_t113_pdata_t *)strip->priv;
	return pdat->count;
}

static void ledstrip_t113_set_color(struct ledstrip_t * strip, int i, struct color_t * c)
{
	struct ledstrip_t113_pdata_t * pdat = (struct ledstrip_t113_pdata_t *)strip->priv;
	memcpy(&pdat->color[i], c, sizeof(struct color_t));
}

static void ledstrip_t113_get_color(struct ledstrip_t * strip, int i, struct color_t * c)
{
	struct ledstrip_t113_pdata_t * pdat = (struct ledstrip_t113_pdata_t *)strip->priv;
	memcpy(c, &pdat->color[i], sizeof(struct color_t));
}

static void ledstrip_t113_refresh(struct ledstrip_t * strip)
{
	struct ledstrip_t113_pdata_t * pdat = (struct ledstrip_t113_pdata_t *)strip->priv;
	int i;

	ledc_set_output_mode(pdat, pdat->mode);
	ledc_set_dma_mode(pdat, 0);
	ledc_set_length(pdat, pdat->count);
	write32(pdat->virt + LEDC_INTC, (0 << 5) | (1 << 4) | (1 << 3) | (1 << 1) | (1 << 0));
	write32(pdat->virt + LEDC_INTS, 0x1b);
	ledc_enable(pdat);
	for(i = 0; i < pdat->count; i++)
	{
		while(read32(pdat->virt + LEDC_INTS) & (1 << 16));
		write32(pdat->virt + LEDC_DATA, (u32_t)((pdat->color[i].g << 16) | (pdat->color[i].r << 0) | (pdat->color[i].b << 0)));
	}
}

static struct device_t * ledstrip_t113_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ledstrip_t113_pdata_t * pdat;
	struct ledstrip_t * strip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct ledstrip_t113_pdata_t));
	if(!pdat)
		return NULL;

	strip = malloc(sizeof(struct ledstrip_t));
	if(!strip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->gpio = dt_read_int(n, "gpio", -1);
	pdat->gpiocfg = dt_read_int(n, "gpio-config", -1);
	pdat->mode = dt_read_int(n, "output-mode", 0);
	pdat->reset_ns = dt_read_int(n, "reset-ns", 84);
	pdat->t1h_ns = dt_read_int(n, "t1h-ns", 800);
	pdat->t1l_ns = dt_read_int(n, "t1l-ns", 320);
	pdat->t0h_ns = dt_read_int(n, "t0h-ns", 300);
	pdat->t0l_ns = dt_read_int(n, "t0l-ns", 800);
	pdat->wait_time0_ns = dt_read_int(n, "wait-time0-ns", 84);
	pdat->wait_time1_ns = dt_read_int(n, "wait-time1-ns", 84);
	pdat->wait_data_time_ns = dt_read_int(n, "wait-data-time-ns", 600000);
	pdat->count = 0;
	pdat->color = NULL;

	strip->name = alloc_device_name(dt_read_name(n), -1);
	strip->set_count = ledstrip_t113_set_count;
	strip->get_count = ledstrip_t113_get_count;
	strip->set_color = ledstrip_t113_set_color;
	strip->get_color = ledstrip_t113_get_color;
	strip->refresh = ledstrip_t113_refresh;
	strip->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
	{
		reset_assert(pdat->reset);
		udelay(1);
		reset_deassert(pdat->reset);
		udelay(1);
	}
	if(pdat->gpio >= 0)
	{
		if(pdat->gpiocfg >= 0)
			gpio_set_cfg(pdat->gpio, pdat->gpiocfg);
		gpio_set_pull(pdat->gpio, GPIO_PULL_UP);
	}
	ledc_init(pdat);
	ledstrip_t113_set_count(strip, dt_read_int(n, "count", 1));
	ledstrip_t113_refresh(strip);

	if(!(dev = register_ledstrip(strip, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(strip->name);
		free(strip->priv);
		free(strip);
		return NULL;
	}
	return dev;
}

static void ledstrip_t113_remove(struct device_t * dev)
{
	struct ledstrip_t * strip = (struct ledstrip_t *)dev->priv;
	struct ledstrip_t113_pdata_t * pdat = (struct ledstrip_t113_pdata_t *)strip->priv;

	if(strip)
	{
		unregister_ledstrip(strip);
		clk_disable(pdat->clk);
		free(pdat->clk);
		if(pdat->color)
			free(pdat->color);
		free_device_name(strip->name);
		free(strip->priv);
		free(strip);
	}
}

static void ledstrip_t113_suspend(struct device_t * dev)
{
}

static void ledstrip_t113_resume(struct device_t * dev)
{
}

static struct driver_t ledstrip_t113 = {
	.name		= "ledstrip-t113",
	.probe		= ledstrip_t113_probe,
	.remove		= ledstrip_t113_remove,
	.suspend	= ledstrip_t113_suspend,
	.resume		= ledstrip_t113_resume,
};

static __init void ledstrip_t113_driver_init(void)
{
	register_driver(&ledstrip_t113);
}

static __exit void ledstrip_t113_driver_exit(void)
{
	unregister_driver(&ledstrip_t113);
}

driver_initcall(ledstrip_t113_driver_init);
driver_exitcall(ledstrip_t113_driver_exit);
