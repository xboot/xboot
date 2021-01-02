/*
 * driver/led-sandbox.c
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
#include <led/led.h>
#include <sandbox.h>

struct led_sandbox_pdata_t {
	char * path;
	int brightness;
	int max_brightness;
};

static void led_sandbox_set(struct led_t * led, int brightness)
{
	struct led_sandbox_pdata_t * pdat = (struct led_sandbox_pdata_t *)led->priv;
	char tmp[32];
	int v;

	if(pdat->brightness != brightness)
	{
		v = brightness * pdat->max_brightness / 1000;
		sprintf(tmp, "%d", v);
		sandbox_sysfs_write_string(pdat->path, tmp);
		pdat->brightness = brightness;
	}
}

static int led_sandbox_get(struct led_t * led)
{
	struct led_sandbox_pdata_t * pdat = (struct led_sandbox_pdata_t *)led->priv;
	return pdat->brightness;
}

static struct device_t * led_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct led_sandbox_pdata_t * pdat;
	struct led_t * led;
	struct device_t * dev;
	char tpath[512], tmp[32];
	char * path = dt_read_string(n, "path", NULL);
	int brightness = -1;
	int max_brightness = -1;

	if(!path || !sandbox_sysfs_access(path, "rw"))
		return NULL;

	strlcpy(tpath, path, sizeof(tpath));
	dirname(tpath);
	strlcat(tpath, "/max_brightness", sizeof(tpath));
	if(sandbox_sysfs_read_string(tpath, tmp))
		max_brightness = strtol(tmp, NULL, 0);
	if(max_brightness <= 0)
		return NULL;

	if(sandbox_sysfs_read_string(path, tmp))
	{
		brightness = strtol(tmp, NULL, 0);
		if(max_brightness > 0)
			brightness = brightness * 1000 / max_brightness > 0;
	}

	pdat = malloc(sizeof(struct led_sandbox_pdata_t));
	if(!pdat)
		return NULL;

	led = malloc(sizeof(struct led_t));
	if(!led)
	{
		free(pdat);
		return NULL;
	}

	pdat->path = strdup(path);
	pdat->brightness = brightness;
	pdat->max_brightness = max_brightness;

	led->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	led->set = led_sandbox_set;
	led->get = led_sandbox_get;
	led->priv = pdat;

	if(!(dev = register_led(led, drv)))
	{
		if(pdat->path)
			free(pdat->path);
		free_device_name(led->name);
		free(led->priv);
		free(led);
		return NULL;
	}
	return dev;
}

static void led_sandbox_remove(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_sandbox_pdata_t * pdat = (struct led_sandbox_pdata_t *)led->priv;

	if(led)
	{
		unregister_led(led);
		if(pdat->path)
			free(pdat->path);
		free_device_name(led->name);
		free(led->priv);
		free(led);
	}
}

static void led_sandbox_suspend(struct device_t * dev)
{
}

static void led_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t led_sandbox = {
	.name		= "led-sandbox",
	.probe		= led_sandbox_probe,
	.remove		= led_sandbox_remove,
	.suspend	= led_sandbox_suspend,
	.resume		= led_sandbox_resume,
};

static __init void led_sandbox_driver_init(void)
{
	register_driver(&led_sandbox);
}

static __exit void led_sandbox_driver_exit(void)
{
	unregister_driver(&led_sandbox);
}

driver_initcall(led_sandbox_driver_init);
driver_exitcall(led_sandbox_driver_exit);
