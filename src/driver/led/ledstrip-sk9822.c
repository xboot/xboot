/*
 * driver/led/ledstrip-sk9822.c
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

#include <xboot.h>
#include <gpio/gpio.h>
#include <led/ledstrip.h>

/*
 * SK9822 - SK9822 LED Strip Driver
 *
 * Example:
 *   "ledstrip-sk9822@0": {
 *       "dat-gpio": 10,
 *       "dat-gpio-config": -1,
 *       "dat-gpio-inverted": 0,
 *       "clk-gpio": 11,
 *       "clk-gpio-config": -1,
 *       "clk-gpio-inverted": 0,
 *       "brightness": 31,
 *       "count": 64
 *   }
 */

struct ledstrip_sk9822_pdata_t {
	int dat;
	int datcfg;
	int datinv;
	int clk;
	int clkcfg;
	int clkinv;
	int brightness;
	int count;
	uint32_t * buffer;
};

static void ledstrip_sk9822_send(struct ledstrip_t * strip, uint8_t byte)
{
	struct ledstrip_sk9822_pdata_t * pdat = (struct ledstrip_sk9822_pdata_t *)strip->priv;
	unsigned char mask;

	for(mask = 0x80; mask; mask >>= 1)
	{
		if(byte & mask)
			gpio_set_value(pdat->dat, pdat->datinv ? 0 : 1);
		else
			gpio_set_value(pdat->dat, pdat->datinv ? 1 : 0);
		gpio_set_value(pdat->clk, pdat->clkinv ? 1 : 0);
		gpio_set_value(pdat->clk, pdat->clkinv ? 0 : 1);
	}
}

static void ledstrip_sk9822_set_count(struct ledstrip_t * strip, int n)
{
	struct ledstrip_sk9822_pdata_t * pdat = (struct ledstrip_sk9822_pdata_t *)strip->priv;

	if((n != pdat->count) && (n > 0))
	{
		if(pdat->buffer)
			free(pdat->buffer);
		pdat->count = n;
		pdat->buffer = memalign(sizeof(uint32_t), pdat->count * sizeof(uint32_t));
		memset(pdat->buffer, 0, pdat->count * sizeof(uint32_t));
	}
}

static int ledstrip_sk9822_get_count(struct ledstrip_t * strip)
{
	struct ledstrip_sk9822_pdata_t * pdat = (struct ledstrip_sk9822_pdata_t *)strip->priv;
	return pdat->count;
}

static void ledstrip_sk9822_set_color(struct ledstrip_t * strip, int i, uint32_t color)
{
	struct ledstrip_sk9822_pdata_t * pdat = (struct ledstrip_sk9822_pdata_t *)strip->priv;
	pdat->buffer[i] = color;
}

static uint32_t ledstrip_sk9822_get_color(struct ledstrip_t * strip, int i)
{
	struct ledstrip_sk9822_pdata_t * pdat = (struct ledstrip_sk9822_pdata_t *)strip->priv;
	return pdat->buffer[i];
}

static void ledstrip_sk9822_refresh(struct ledstrip_t * strip)
{
	struct ledstrip_sk9822_pdata_t * pdat = (struct ledstrip_sk9822_pdata_t *)strip->priv;
	int i;

	ledstrip_sk9822_send(strip, 0x00);
	ledstrip_sk9822_send(strip, 0x00);
	ledstrip_sk9822_send(strip, 0x00);
	ledstrip_sk9822_send(strip, 0x00);
	for(i = 0; i < pdat->count; i++)
	{
		ledstrip_sk9822_send(strip, (0x7 << 5) | (pdat->brightness & 0x1f));
		ledstrip_sk9822_send(strip, (pdat->buffer[i] >>  0) & 0xff);
		ledstrip_sk9822_send(strip, (pdat->buffer[i] >>  8) & 0xff);
		ledstrip_sk9822_send(strip, (pdat->buffer[i] >> 16) & 0xff);
	}
	ledstrip_sk9822_send(strip, 0xff);
	ledstrip_sk9822_send(strip, 0xff);
	ledstrip_sk9822_send(strip, 0xff);
	ledstrip_sk9822_send(strip, 0xff);
}

static struct device_t * ledstrip_sk9822_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ledstrip_sk9822_pdata_t * pdat;
	struct ledstrip_t * strip;
	struct device_t * dev;
	int dat = dt_read_int(n, "dat-gpio", -1);
	int clk = dt_read_int(n, "clk-gpio", -1);

	if(!gpio_is_valid(dat) || !gpio_is_valid(clk))
		return NULL;

	pdat = malloc(sizeof(struct ledstrip_sk9822_pdata_t));
	if(!pdat)
		return NULL;

	strip = malloc(sizeof(struct ledstrip_t));
	if(!strip)
	{
		free(pdat);
		return NULL;
	}

	pdat->dat = dat;
	pdat->datcfg = dt_read_int(n, "dat-gpio-config", -1);
	pdat->datinv = dt_read_int(n, "dat-gpio-inverted", 0);
	pdat->clk = clk;
	pdat->clkcfg = dt_read_int(n, "clk-gpio-config", -1);
	pdat->clkinv = dt_read_int(n, "clk-gpio-inverted", 0);
	pdat->brightness = dt_read_int(n, "brightness", 31);
	pdat->count = 0;
	pdat->buffer = NULL;

	strip->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	strip->set_count = ledstrip_sk9822_set_count;
	strip->get_count = ledstrip_sk9822_get_count;
	strip->set_color = ledstrip_sk9822_set_color;
	strip->get_color = ledstrip_sk9822_get_color;
	strip->refresh = ledstrip_sk9822_refresh;
	strip->priv = pdat;

	if(pdat->dat >= 0)
	{
		if(pdat->datcfg >= 0)
			gpio_set_cfg(pdat->dat, pdat->datcfg);
		gpio_set_pull(pdat->dat, pdat->datinv ? GPIO_PULL_UP :GPIO_PULL_DOWN);
		gpio_set_direction(pdat->dat, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->dat, pdat->datinv ? 1 : 0);
	}
	if(pdat->clk >= 0)
	{
		if(pdat->clkcfg >= 0)
			gpio_set_cfg(pdat->clk, pdat->clkcfg);
		gpio_set_pull(pdat->clk, pdat->clkinv ? GPIO_PULL_UP :GPIO_PULL_DOWN);
		gpio_set_direction(pdat->clk, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->clk, pdat->clkinv ? 1 : 0);
	}
	ledstrip_sk9822_set_count(strip, dt_read_int(n, "count", 1));

	if(!register_ledstrip(&dev, strip))
	{
		free_device_name(strip->name);
		free(strip->priv);
		free(strip);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ledstrip_sk9822_remove(struct device_t * dev)
{
	struct ledstrip_t * strip = (struct ledstrip_t *)dev->priv;
	struct ledstrip_sk9822_pdata_t * pdat = (struct ledstrip_sk9822_pdata_t *)strip->priv;

	if(strip && unregister_ledstrip(strip))
	{
		if(pdat->buffer)
			free(pdat->buffer);

		free_device_name(strip->name);
		free(strip->priv);
		free(strip);
	}
}

static void ledstrip_sk9822_suspend(struct device_t * dev)
{
}

static void ledstrip_sk9822_resume(struct device_t * dev)
{
}

static struct driver_t ledstrip_sk9822 = {
	.name		= "ledstrip-sk9822",
	.probe		= ledstrip_sk9822_probe,
	.remove		= ledstrip_sk9822_remove,
	.suspend	= ledstrip_sk9822_suspend,
	.resume		= ledstrip_sk9822_resume,
};

static __init void ledstrip_sk9822_driver_init(void)
{
	register_driver(&ledstrip_sk9822);
}

static __exit void ledstrip_sk9822_driver_exit(void)
{
	unregister_driver(&ledstrip_sk9822);
}

driver_initcall(ledstrip_sk9822_driver_init);
driver_exitcall(ledstrip_sk9822_driver_exit);
