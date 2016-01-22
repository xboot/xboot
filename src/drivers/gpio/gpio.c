/*
 * driver/gpio/gpio.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <gpio/gpio.h>

struct gpiochip_list_t
{
	struct gpiochip_t * chip;
	struct list_head entry;
};

struct gpiochip_list_t __gpiochip_list = {
	.entry = {
		.next	= &(__gpiochip_list.entry),
		.prev	= &(__gpiochip_list.entry),
	},
};
static spinlock_t __gpiochip_list_lock = SPIN_LOCK_INIT();

static struct kobj_t * search_class_gpiochip_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "gpiochip");
}

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

static struct gpiochip_t * search_gpiochip_with_no(int gpio)
{
	struct gpiochip_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(__gpiochip_list.entry), entry)
	{
		if( (gpio >= pos->chip->base) && (gpio < (pos->chip->base + pos->chip->ngpio)) )
			return pos->chip;
	}

	return NULL;
}

struct gpiochip_t * search_gpiochip(const char * name)
{
	struct gpiochip_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__gpiochip_list.entry), entry)
	{
		if(strcmp(pos->chip->name, name) == 0)
			return pos->chip;
	}

	return NULL;
}

bool_t register_gpiochip(struct gpiochip_t * chip)
{
	struct gpiochip_list_t * gl;
	irq_flags_t flags;

	if(!chip || !chip->name)
		return FALSE;

	if(search_gpiochip(chip->name))
		return FALSE;

	gl = malloc(sizeof(struct gpiochip_list_t));
	if(!gl)
		return FALSE;

	chip->kobj = kobj_alloc_directory(chip->name);
	kobj_add_regular(chip->kobj, "base", gpiochip_read_base, NULL, chip);
	kobj_add_regular(chip->kobj, "ngpio", gpiochip_read_ngpio, NULL, chip);
	kobj_add(search_class_gpiochip_kobj(), chip->kobj);
	gl->chip = chip;

	spin_lock_irqsave(&__gpiochip_list_lock, flags);
	list_add_tail(&gl->entry, &(__gpiochip_list.entry));
	spin_unlock_irqrestore(&__gpiochip_list_lock, flags);

	return TRUE;
}

bool_t unregister_gpiochip(struct gpiochip_t * chip)
{
	struct gpiochip_list_t * pos, * n;
	irq_flags_t flags;

	if(!chip || !chip->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__gpiochip_list.entry), entry)
	{
		if(pos->chip == chip)
		{
			spin_lock_irqsave(&__gpiochip_list_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__gpiochip_list_lock, flags);

			kobj_remove(search_class_gpiochip_kobj(), pos->chip->kobj);
			kobj_remove_self(chip->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

int gpio_is_valid(int gpio)
{
	return search_gpiochip_with_no(gpio) ? 1 : 0;
}

void gpio_set_cfg(int gpio, int cfg)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->set_cfg)
		chip->set_cfg(chip, gpio - chip->base, cfg);
}

int gpio_get_cfg(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->get_cfg)
		return chip->get_cfg(chip, gpio - chip->base);
	return 0;
}

void gpio_set_pull(int gpio, enum gpio_pull_t pull)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->set_pull)
		chip->set_pull(chip, gpio - chip->base, pull);
}

enum gpio_pull_t gpio_get_pull(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->get_pull)
		return chip->get_pull(chip, gpio - chip->base);
	return GPIO_PULL_NONE;
}

void gpio_set_drv(int gpio, enum gpio_drv_t drv)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->set_drv)
		chip->set_drv(chip, gpio - chip->base, drv);
}

enum gpio_drv_t gpio_get_drv(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->get_drv)
		return chip->get_drv(chip, gpio - chip->base);
	return GPIO_DRV_LOW;
}

void gpio_set_rate(int gpio, enum gpio_rate_t rate)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->set_rate)
		chip->set_rate(chip, gpio - chip->base, rate);
}

enum gpio_rate_t gpio_get_rate(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->get_rate)
		return chip->get_rate(chip, gpio - chip->base);
	return GPIO_RATE_SLOW;
}

void gpio_set_direction(int gpio, enum gpio_direction_t dir)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->set_dir)
		chip->set_dir(chip, gpio - chip->base, dir);
}

enum gpio_direction_t gpio_get_direction(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->get_dir)
		return chip->get_dir(chip, gpio - chip->base);
	return GPIO_DIRECTION_UNKOWN;
}

void gpio_set_value(int gpio, int value)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->set_value)
		chip->set_value(chip, gpio - chip->base, value);
}

int gpio_get_value(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->get_value)
		return chip->get_value(chip, gpio - chip->base);
	return 0;
}

void gpio_direction_output(int gpio, int value)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(!chip)
		return;

	if(chip->set_dir)
		chip->set_dir(chip, gpio - chip->base, GPIO_DIRECTION_OUTPUT);

	if(chip->set_value)
		chip->set_value(chip, gpio - chip->base, value);
}

void gpio_direction_input(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->set_dir)
		chip->set_dir(chip, gpio - chip->base, GPIO_DIRECTION_INPUT);
}

const char * gpio_to_irq(int gpio)
{
	struct gpiochip_t * chip = search_gpiochip_with_no(gpio);

	if(chip && chip->to_irq)
		return chip->to_irq(chip, gpio - chip->base);
	return NULL;
}
