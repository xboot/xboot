/*
 * kernel/core/gpio.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <spinlock.h>
#include <xboot/gpio.h>

struct gpio_list_t
{
	struct gpio_t * gpio;
	struct list_head entry;
};

struct gpio_list_t __gpio_list = {
	.entry = {
		.next	= &(__gpio_list.entry),
		.prev	= &(__gpio_list.entry),
	},
};
static spinlock_t __gpio_list_lock = SPIN_LOCK_INIT();

struct gpio_t * search_gpio(const char * name)
{
	struct gpio_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__gpio_list.entry), entry)
	{
		if(strcmp(pos->gpio->name, name) == 0)
			return pos->gpio;
	}

	return NULL;
}

struct gpio_t * search_gpio_with_no(int no)
{
	struct gpio_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(__gpio_list.entry), entry)
	{
		if( (no >= pos->gpio->base) && (no < (pos->gpio->base + pos->gpio->ngpio)) )
			return pos->gpio;
	}

	return NULL;
}

bool_t register_gpio(struct gpio_t * gpio)
{
	struct gpio_list_t * gl;

	if(!gpio || !gpio->name)
		return FALSE;

	if(search_gpio(gpio->name))
		return FALSE;

	gl = malloc(sizeof(struct gpio_list_t));
	if(!gl)
		return FALSE;

	gl->gpio = gpio;

	spin_lock_irq(&__gpio_list_lock);
	list_add_tail(&gl->entry, &(__gpio_list.entry));
	spin_unlock_irq(&__gpio_list_lock);

	return TRUE;
}

bool_t unregister_gpio(struct gpio_t * gpio)
{
	struct gpio_list_t * pos, * n;

	if(!gpio || !gpio->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__gpio_list.entry), entry)
	{
		if(pos->gpio == gpio)
		{
			spin_lock_irq(&__gpio_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__gpio_list_lock);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

int gpio_is_valid(int no)
{
	return search_gpio_with_no(no) ? 1 : 0;
}

void gpio_set_cfg(int no, int cfg)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->set_cfg)
		gpio->set_cfg(gpio, no - gpio->base, cfg);
}

int gpio_get_cfg(int no)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->get_cfg)
		return gpio->get_cfg(gpio, no - gpio->base);
	return 0;
}

void gpio_set_pull(int no, enum gpio_pull_t pull)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->set_pull)
		gpio->set_pull(gpio, no - gpio->base, pull);
}

enum gpio_pull_t gpio_get_pull(int no)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->get_pull)
		return gpio->get_pull(gpio, no - gpio->base);
	return GPIO_PULL_NONE;
}

void gpio_set_drv(int no, enum gpio_drv_t drv)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->set_drv)
		gpio->set_drv(gpio, no - gpio->base, drv);
}

enum gpio_drv_t gpio_get_drv(int no)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->get_drv)
		return gpio->get_drv(gpio, no - gpio->base);
	return GPIO_DRV_NONE;
}

void gpio_set_rate(int no, enum gpio_rate_t rate)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->set_rate)
		gpio->set_rate(gpio, no - gpio->base, rate);
}

enum gpio_rate_t gpio_get_rate(int no)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->get_rate)
		return gpio->get_rate(gpio, no - gpio->base);
	return GPIO_RATE_NONE;
}

void gpio_set_direction(int no, enum gpio_direction_t dir)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->set_dir)
		gpio->set_dir(gpio, no - gpio->base, dir);
}

enum gpio_direction_t gpio_get_direction(int no)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->get_dir)
		return gpio->get_dir(gpio, no - gpio->base);
	return GPIO_DIRECTION_NONE;
}

void gpio_set_value(int no, int value)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->set_value)
		gpio->set_value(gpio, no - gpio->base, value);
}

int gpio_get_value(int no)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->get_value)
		return gpio->get_value(gpio, no - gpio->base);
	return 0;
}

void gpio_direction_output(int no, int value)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(!gpio)
		return;

	if(gpio->set_dir)
		gpio->set_dir(gpio, no - gpio->base, GPIO_DIRECTION_OUTPUT);

	if(gpio->set_value)
		gpio->set_value(gpio, no - gpio->base, value);
}

void gpio_direction_input(int no)
{
	struct gpio_t * gpio = search_gpio_with_no(no);

	if(gpio && gpio->set_dir)
		gpio->set_dir(gpio, no - gpio->base, GPIO_DIRECTION_INPUT);
}

static s32_t gpio_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct gpio_list_t * pos, * n;
	s8_t * p;
	s32_t len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), "[gpio]");

	list_for_each_entry_safe(pos, n, &(__gpio_list.entry), entry)
	{
		len += sprintf((char *)(p + len), "\r\n %s: [%d - %d]", pos->gpio->name, pos->gpio->base, pos->gpio->base + pos->gpio->ngpio);
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (u8_t *)(p + offset), len);
	free(p);

	return len;
}

static struct proc_t gpio_proc = {
	.name	= "gpio",
	.read	= gpio_proc_read,
};

static __init void gpio_proc_init(void)
{
	proc_register(&gpio_proc);
}

static __exit void gpio_proc_exit(void)
{
	proc_unregister(&gpio_proc);
}

core_initcall(gpio_proc_init);
core_exitcall(gpio_proc_exit);
