/*
 * driver/led/led.c
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
#include <led/led.h>

static ssize_t led_read_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	struct led_t * led = (struct led_t *)kobj->priv;
	int brightness;

	brightness = led_get_brightness(led);
	return sprintf(buf, "%d", brightness);
}

static ssize_t led_write_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	struct led_t * led = (struct led_t *)kobj->priv;
	int brightness = strtol(buf, NULL, 0);

	led_set_brightness(led, brightness);
	return size;
}

static ssize_t led_read_max_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	return sprintf(buf, "%u", CONFIG_MAX_BRIGHTNESS);
}

struct led_t * search_led(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_LED);
	if(!dev)
		return NULL;
	return (struct led_t *)dev->priv;
}

bool_t register_led(struct device_t ** device, struct led_t * led)
{
	struct device_t * dev;

	if(!led || !led->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(led->name);
	dev->type = DEVICE_TYPE_LED;
	dev->priv = led;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "brightness", led_read_brightness, led_write_brightness, led);
	kobj_add_regular(dev->kobj, "max_brightness", led_read_max_brightness, NULL, led);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_led(struct led_t * led)
{
	struct device_t * dev;

	if(!led || !led->name)
		return FALSE;

	dev = search_device(led->name, DEVICE_TYPE_LED);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void led_set_brightness(struct led_t * led, int brightness)
{
	if(led && led->set)
	{
		if(brightness < 0)
			brightness = 0;
		else if(brightness > CONFIG_MAX_BRIGHTNESS)
			brightness = CONFIG_MAX_BRIGHTNESS;
		led->set(led, brightness);
	}
}

int led_get_brightness(struct led_t * led)
{
	int brightness = 0;

	if(led && led->get)
		brightness = led->get(led);

	if(brightness < 0)
		brightness = 0;
	else if(brightness > CONFIG_MAX_BRIGHTNESS)
		brightness = CONFIG_MAX_BRIGHTNESS;

	return brightness;
}
