/*
 * driver/gpio/gpio.c
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

#include <gpio/gpio.h>

static ssize_t gpiochip_read_base(struct kobj_t * kobj, void * buf, size_t size)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)kobj->priv;
	return sprintf(buf, "%d", chip->base);
}

static ssize_t gpiochip_read_ngpio(struct kobj_t * kobj, void * buf, size_t size)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)kobj->priv;
	return sprintf(buf, "%d", chip->ngpio);
}

struct gpiochip_t * search_gpiochip(int gpio)
{
	struct device_t * pos, * n;
	struct gpiochip_t * chip;

	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_GPIOCHIP], head)
	{
		chip = (struct gpiochip_t *)(pos->priv);
		if((gpio >= chip->base) && (gpio < (chip->base + chip->ngpio)))
			return chip;
	}
	return NULL;
}

bool_t register_gpiochip(struct device_t ** device, struct gpiochip_t * chip)
{
	struct device_t * dev;

	if(!chip || !chip->name)
		return FALSE;

	if(chip->base < 0 || chip->ngpio <= 0)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(chip->name);
	dev->type = DEVICE_TYPE_GPIOCHIP;
	dev->priv = chip;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "base", gpiochip_read_base, NULL, chip);
	kobj_add_regular(dev->kobj, "ngpio", gpiochip_read_ngpio, NULL, chip);

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

bool_t unregister_gpiochip(struct gpiochip_t * chip)
{
	struct device_t * dev;

	if(!chip || !chip->name)
		return FALSE;

	if(chip->base < 0 || chip->ngpio <= 0)
		return FALSE;

	dev = search_device(chip->name, DEVICE_TYPE_GPIOCHIP);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int gpio_is_valid(int gpio)
{
	return search_gpiochip(gpio) ? 1 : 0;
}

void gpio_set_cfg(int gpio, int cfg)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->set_cfg)
		chip->set_cfg(chip, gpio - chip->base, cfg);
}

int gpio_get_cfg(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->get_cfg)
		return chip->get_cfg(chip, gpio - chip->base);
	return 0;
}

void gpio_set_pull(int gpio, enum gpio_pull_t pull)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->set_pull)
		chip->set_pull(chip, gpio - chip->base, pull);
}

enum gpio_pull_t gpio_get_pull(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->get_pull)
		return chip->get_pull(chip, gpio - chip->base);
	return GPIO_PULL_NONE;
}

void gpio_set_drv(int gpio, enum gpio_drv_t drv)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->set_drv)
		chip->set_drv(chip, gpio - chip->base, drv);
}

enum gpio_drv_t gpio_get_drv(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->get_drv)
		return chip->get_drv(chip, gpio - chip->base);
	return GPIO_DRV_WEAK;
}

void gpio_set_rate(int gpio, enum gpio_rate_t rate)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->set_rate)
		chip->set_rate(chip, gpio - chip->base, rate);
}

enum gpio_rate_t gpio_get_rate(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->get_rate)
		return chip->get_rate(chip, gpio - chip->base);
	return GPIO_RATE_SLOW;
}

void gpio_set_direction(int gpio, enum gpio_direction_t dir)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->set_dir)
		chip->set_dir(chip, gpio - chip->base, dir);
}

enum gpio_direction_t gpio_get_direction(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->get_dir)
		return chip->get_dir(chip, gpio - chip->base);
	return GPIO_DIRECTION_INPUT;
}

void gpio_set_value(int gpio, int value)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->set_value)
		chip->set_value(chip, gpio - chip->base, value);
}

int gpio_get_value(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->get_value)
		return chip->get_value(chip, gpio - chip->base);
	return 0;
}

void gpio_direction_output(int gpio, int value)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(!chip)
		return;

	if(chip->set_dir)
		chip->set_dir(chip, gpio - chip->base, GPIO_DIRECTION_OUTPUT);
	if(chip->set_value)
		chip->set_value(chip, gpio - chip->base, value);
}

int gpio_direction_input(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip)
	{
		if(chip->set_dir)
			chip->set_dir(chip, gpio - chip->base, GPIO_DIRECTION_INPUT);
		if(chip->get_value)
			return chip->get_value(chip, gpio - chip->base);
	}
	return 0;
}

int gpio_to_irq(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip(gpio);

	if(chip && chip->to_irq)
		return chip->to_irq(chip, gpio - chip->base);
	return -1;
}
