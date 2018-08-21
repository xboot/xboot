/*
 * driver/fb-ssd1309.c
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
#include <dma/dma.h>
#include <spi/spi.h>
#include <gpio/gpio.h>
#include <framebuffer/framebuffer.h>

/*
 * OLED - Solomon SSD1309 OLED Driver
 *
 * Example:
 *	"fb-ssd1309@0": {
 *		"spi-bus": "spi-h3.0",
 *		"chip-select": 0,
 *		"mode": 0,
 *		"speed": 50000000,
 *		"reset-gpio": -1,
 *		"reset-gpio-config": -1,
 *		"cd-gpio": 65,
 *		"cd-gpio-config": 1,
 *		"width": 128,
 *		"height": 64,
 *		"physical-width": 35,
 *		"physical-height": 17,
 *		"bits-per-pixel": 32
 *	}
 */

struct fb_ssd1309_pdata_t {
	struct spi_device_t * dev;
	int rst;
	int rstcfg;
	int cd;
	int cdcfg;
	int width;
	int height;
	int pwidth;
	int pheight;
	int bpp;
	int brightness;
};

static void ssd1309_write_command(struct fb_ssd1309_pdata_t * pdat, u8_t cmd)
{
	spi_device_select(pdat->dev);
	gpio_set_value(pdat->cd, 0);
	spi_device_write_then_read(pdat->dev, &cmd, 1, 0, 0);
	gpio_set_value(pdat->cd, 1);
	spi_device_deselect(pdat->dev);
}

static void ssd1309_write_data(struct fb_ssd1309_pdata_t * pdat, u8_t dat)
{
	spi_device_select(pdat->dev);
	spi_device_write_then_read(pdat->dev, &dat, 1, 0, 0);
	spi_device_deselect(pdat->dev);
}

static void ssd1309_init(struct fb_ssd1309_pdata_t * pdat)
{
	/* Unlock driver IC */
	ssd1309_write_command(pdat, 0xfd);
	ssd1309_write_data(pdat, 0x12);

	/* Display off */
	ssd1309_write_command(pdat, 0xae);

	/* Set clock as 116 fps */
	ssd1309_write_command(pdat, 0xd5);
	ssd1309_write_command(pdat, 0xa0);

	/* 1/64 duty */
	ssd1309_write_command(pdat, 0xa8);
	ssd1309_write_command(pdat, 0x3f);

	/* Set display offest */
	ssd1309_write_command(pdat, 0xd3);
	ssd1309_write_command(pdat, 0x00);

	/* Set display start line */
	ssd1309_write_command(pdat, 0x40 | 0x00);

	/* Set page addressing mode */
	ssd1309_write_command(pdat, 0x20);
	ssd1309_write_command(pdat, 0x02);

	/* Set segment remap */
	ssd1309_write_command(pdat, 0xa1);

	/* Set common remap */
	ssd1309_write_command(pdat, 0xc8);

	/* Set common config */
	ssd1309_write_command(pdat, 0xda);
	ssd1309_write_command(pdat, 0x12);

	/* Set segment output current */
	ssd1309_write_command(pdat, 0x81);
	ssd1309_write_command(pdat, 0x00);

	/* Set precharge as 2 clocks & discharge as 5 clocks */
	ssd1309_write_command(pdat, 0xd9);
	ssd1309_write_command(pdat, 0x25);

	/* Set vcom deselect level */
	ssd1309_write_command(pdat, 0xdb);
	ssd1309_write_command(pdat, 0x34);

	/* Disable entire display */
	ssd1309_write_command(pdat, 0xa4);

	/* Disable inverse display */
	ssd1309_write_command(pdat, 0xa6);
}

static void fb_setbl(struct framebuffer_t * fb, int brightness)
{
	struct fb_ssd1309_pdata_t * pdat = (struct fb_ssd1309_pdata_t *)fb->priv;

	if(pdat->brightness != brightness)
	{
		ssd1309_write_command(pdat, 0x81);
		ssd1309_write_command(pdat, (0xff * brightness / CONFIG_MAX_BRIGHTNESS) & 0xff);
		if(brightness == 0)
			ssd1309_write_command(pdat, 0xae);
		else
			ssd1309_write_command(pdat, 0xaf);
		pdat->brightness = brightness;
	}
}

static int fb_getbl(struct framebuffer_t * fb)
{
	struct fb_ssd1309_pdata_t * pdat = (struct fb_ssd1309_pdata_t *)fb->priv;
	return pdat->brightness;
}

struct render_t * fb_create(struct framebuffer_t * fb)
{
	struct fb_ssd1309_pdata_t * pdat = (struct fb_ssd1309_pdata_t *)fb->priv;
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	pixlen = pdat->width * pdat->height * pdat->bpp / 8;
	pixels = memalign(4, pixlen);
	if(!pixels)
		return NULL;

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(pixels);
		return NULL;
	}

	render->width = pdat->width;
	render->height = pdat->height;
	render->pitch = (pdat->width * pdat->bpp / 8 + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;
	render->pixlen = pixlen;
	render->priv = NULL;

	return render;
}

void fb_destroy(struct framebuffer_t * fb, struct render_t * render)
{
	if(render)
	{
		free(render->pixels);
		free(render);
	}
}

void fb_present(struct framebuffer_t * fb, struct render_t * render)
{
	struct fb_ssd1309_pdata_t * pdat = (struct fb_ssd1309_pdata_t *)fb->priv;
	int x, y, i, o;
	u32_t * p;
	u8_t v;

	if(render && render->pixels)
	{
		p = render->pixels;
		for(y = 0; y < pdat->height / 8; y++)
		{
			ssd1309_write_command(pdat, 0xb0 | y);
			ssd1309_write_command(pdat, 0x00 | ((0 >> 0) & 0xf));
			ssd1309_write_command(pdat, 0x10 | ((0 >> 4) & 0xf));
			for(x = 0; x < pdat->width; x++)
			{
				for(v = 0, i = 0; i < 8; i++)
				{
					o = (y * 8 + i) * pdat->width + x;
					v |= ((p[o] & 0xffffff) ? 1 : 0) << i;
				}
				ssd1309_write_data(pdat, v);
			}
		}
	}
}

static struct device_t * fb_ssd1309_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_ssd1309_pdata_t * pdat;
	struct framebuffer_t * fb;
	struct device_t * dev;
	struct spi_device_t * spidev;
	int cd = dt_read_int(n, "cd-gpio", -1);

	if(!gpio_is_valid(cd))
		return NULL;

	spidev = spi_device_alloc(dt_read_string(n, "spi-bus", NULL), dt_read_int(n, "chip-select", 0), dt_read_int(n, "mode", 0), 8, dt_read_int(n, "speed", 0));
	if(!spidev)
		return NULL;

	pdat = malloc(sizeof(struct fb_ssd1309_pdata_t));
	if(!pdat)
	{
		spi_device_free(spidev);
		return NULL;
	}

	fb = malloc(sizeof(struct framebuffer_t));
	if(!fb)
	{
		spi_device_free(spidev);
		free(pdat);
		return NULL;
	}

	pdat->dev = spidev;
	pdat->rst = dt_read_int(n, "reset-gpio", -1);
	pdat->rstcfg = dt_read_int(n, "reset-gpio-config", -1);
	pdat->cd = cd;
	pdat->cdcfg = dt_read_int(n, "cd-gpio-config", -1);
	pdat->width = dt_read_int(n, "width", 128);
	pdat->height = dt_read_int(n, "height", 64);
	pdat->pwidth = dt_read_int(n, "physical-width", 35);
	pdat->pheight = dt_read_int(n, "physical-height", 17);
	pdat->bpp = dt_read_int(n, "bits-per-pixel", 32);
	pdat->brightness = -1;

	fb->name = alloc_device_name(dt_read_name(n), -1);
	fb->width = pdat->width;
	fb->height = pdat->height;
	fb->pwidth = pdat->pwidth;
	fb->pheight = pdat->pheight;
	fb->bpp = pdat->bpp;
	fb->setbl = fb_setbl;
	fb->getbl = fb_getbl;
	fb->create = fb_create;
	fb->destroy = fb_destroy;
	fb->present = fb_present;
	fb->priv = pdat;

	if(pdat->rst >= 0)
	{
		if(pdat->rst >= 0)
			gpio_set_cfg(pdat->rst, pdat->rstcfg);
		gpio_set_pull(pdat->rst, GPIO_PULL_UP);
		gpio_direction_output(pdat->rst, 0);
		udelay(1);
		gpio_direction_output(pdat->rst, 1);
		udelay(1);
	}
	if(pdat->cd >= 0)
	{
		if(pdat->cd >= 0)
			gpio_set_cfg(pdat->cd, pdat->cdcfg);
		gpio_set_pull(pdat->cd, GPIO_PULL_UP);
		gpio_direction_output(pdat->cd, 1);
	}
	ssd1309_init(pdat);

	if(!register_framebuffer(&dev, fb))
	{
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void fb_ssd1309_remove(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;

	if(fb && unregister_framebuffer(fb))
	{
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_ssd1309_suspend(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_ssd1309_pdata_t * pdat = (struct fb_ssd1309_pdata_t *)fb->priv;

	ssd1309_write_command(pdat, 0xae);
	ssd1309_write_command(pdat, 0xa5);
}

static void fb_ssd1309_resume(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_ssd1309_pdata_t * pdat = (struct fb_ssd1309_pdata_t *)fb->priv;

	ssd1309_write_command(pdat, 0xa4);
	ssd1309_write_command(pdat, 0xaf);
}

static struct driver_t fb_ssd1309 = {
	.name		= "fb-ssd1309",
	.probe		= fb_ssd1309_probe,
	.remove		= fb_ssd1309_remove,
	.suspend	= fb_ssd1309_suspend,
	.resume		= fb_ssd1309_resume,
};

static __init void fb_ssd1309_driver_init(void)
{
	register_driver(&fb_ssd1309);
}

static __exit void fb_ssd1309_driver_exit(void)
{
	unregister_driver(&fb_ssd1309);
}

driver_initcall(fb_ssd1309_driver_init);
driver_exitcall(fb_ssd1309_driver_exit);
