/*
 * driver/led/ledstrip-ws2812.c
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
#include <spi/spi.h>
#include <led/ledstrip.h>

/*
 * WS2812 - WS2812 LED Strip Driver
 *
 * Example:
 *   "ledstrip-ws2812@0": {
 *       "spi-bus": "spi-f1c100s.0",
 *       "chip-select": 0,
 *       "mode": 0,
 *       "speed": 6400000,
 *       "count": 10
 *   }
 */

struct ledstrip_ws2812_pdata_t {
	struct spi_device_t * spidev;
	int count;
	struct color_t * color;
	int buflen;
	unsigned char * buffer;
};

static void ledstrip_ws2812_set_count(struct ledstrip_t * strip, int n)
{
	struct ledstrip_ws2812_pdata_t * pdat = (struct ledstrip_ws2812_pdata_t *)strip->priv;

	if((n != pdat->count) && (n > 0))
	{
		if(pdat->color)
			free(pdat->color);
		if(pdat->buffer)
			free(pdat->buffer);
		pdat->count = n;
		pdat->color = malloc(pdat->count * sizeof(struct color_t));
		memset(pdat->color, 0, pdat->count * sizeof(struct color_t));
		pdat->buflen = n * 24 + 240;
		pdat->buffer = malloc(pdat->buflen);
		memset(&pdat->buffer[0], 0xe0, n * 24);
		memset(&pdat->buffer[n * 24], 0, 240);
	}
}

static int ledstrip_ws2812_get_count(struct ledstrip_t * strip)
{
	struct ledstrip_ws2812_pdata_t * pdat = (struct ledstrip_ws2812_pdata_t *)strip->priv;
	return pdat->count;
}

static void ledstrip_ws2812_set_color(struct ledstrip_t * strip, int i, struct color_t * c)
{
	struct ledstrip_ws2812_pdata_t * pdat = (struct ledstrip_ws2812_pdata_t *)strip->priv;
	unsigned char * p = &pdat->buffer[i * 24];
	int n;

	memcpy(&pdat->color[i], c, sizeof(struct color_t));
	for(n = 0; n < 8; n++)
		*p++ = (c->g & (0x80 >> n)) ? 0xf8: 0xe0;
	for(n = 0; n < 8; n++)
		*p++ = (c->r & (0x80 >> n)) ? 0xf8: 0xe0;
	for(n = 0; n < 8; n++)
		*p++ = (c->b & (0x80 >> n)) ? 0xf8: 0xe0;
}

static void ledstrip_ws2812_get_color(struct ledstrip_t * strip, int i, struct color_t * c)
{
	struct ledstrip_ws2812_pdata_t * pdat = (struct ledstrip_ws2812_pdata_t *)strip->priv;
	memcpy(c, &pdat->color[i], sizeof(struct color_t));
}

static void ledstrip_ws2812_refresh(struct ledstrip_t * strip)
{
	struct ledstrip_ws2812_pdata_t * pdat = (struct ledstrip_ws2812_pdata_t *)strip->priv;
	spi_device_write_then_read(pdat->spidev, pdat->buffer, pdat->buflen, NULL, 0);
}

static struct device_t * ledstrip_ws2812_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ledstrip_ws2812_pdata_t * pdat;
	struct ledstrip_t * strip;
	struct device_t * dev;
	struct spi_device_t * spidev;

	spidev = spi_device_alloc(dt_read_string(n, "spi-bus", NULL), dt_read_int(n, "chip-select", 0), dt_read_int(n, "type", 0), dt_read_int(n, "mode", 0), 8, dt_read_int(n, "speed", 6400000));
	if(!spidev)
		return NULL;

	pdat = malloc(sizeof(struct ledstrip_ws2812_pdata_t));
	if(!pdat)
		return NULL;

	strip = malloc(sizeof(struct ledstrip_t));
	if(!strip)
	{
		free(pdat);
		return NULL;
	}

	pdat->spidev = spidev;
	pdat->count = 0;
	pdat->color = NULL;
	pdat->buflen = 0;
	pdat->buffer = NULL;

	strip->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	strip->set_count = ledstrip_ws2812_set_count;
	strip->get_count = ledstrip_ws2812_get_count;
	strip->set_color = ledstrip_ws2812_set_color;
	strip->get_color = ledstrip_ws2812_get_color;
	strip->refresh = ledstrip_ws2812_refresh;
	strip->priv = pdat;

	ledstrip_ws2812_set_count(strip, dt_read_int(n, "count", 1));
	ledstrip_ws2812_refresh(strip);

	if(!(dev = register_ledstrip(strip, drv)))
	{
		free_device_name(strip->name);
		free(strip->priv);
		free(strip);
		return NULL;
	}
	return dev;
}

static void ledstrip_ws2812_remove(struct device_t * dev)
{
	struct ledstrip_t * strip = (struct ledstrip_t *)dev->priv;
	struct ledstrip_ws2812_pdata_t * pdat = (struct ledstrip_ws2812_pdata_t *)strip->priv;

	if(strip)
	{
		unregister_ledstrip(strip);
		if(pdat->color)
			free(pdat->color);
		free_device_name(strip->name);
		free(strip->priv);
		free(strip);
	}
}

static void ledstrip_ws2812_suspend(struct device_t * dev)
{
}

static void ledstrip_ws2812_resume(struct device_t * dev)
{
}

static struct driver_t ledstrip_ws2812 = {
	.name		= "ledstrip-ws2812",
	.probe		= ledstrip_ws2812_probe,
	.remove		= ledstrip_ws2812_remove,
	.suspend	= ledstrip_ws2812_suspend,
	.resume		= ledstrip_ws2812_resume,
};

static __init void ledstrip_ws2812_driver_init(void)
{
	register_driver(&ledstrip_ws2812);
}

static __exit void ledstrip_ws2812_driver_exit(void)
{
	unregister_driver(&ledstrip_ws2812);
}

driver_initcall(ledstrip_ws2812_driver_init);
driver_exitcall(ledstrip_ws2812_driver_exit);
