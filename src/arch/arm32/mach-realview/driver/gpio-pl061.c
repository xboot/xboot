/*
 * driver/gpio-pl061.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

enum {
	GPIO_DIR	= 0x400,
	GPIO_IS		= 0x404,
	GPIO_IBE	= 0x408,
	GPIO_IEV	= 0x40C,
	GPIO_IE		= 0x410,
	GPIO_RIS	= 0x414,
	GPIO_MIS	= 0x418,
	GPIO_IC		= 0x41C,
};

struct gpio_pl061_pdata_t
{
	virtual_addr_t virt;
	int base;
	int ngpio;
	int oirq;
};

static void gpio_pl061_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
}

static int gpio_pl061_get_cfg(struct gpiochip_t * chip, int offset)
{
	return 0;
}

static void gpio_pl061_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
}

static enum gpio_pull_t gpio_pl061_get_pull(struct gpiochip_t * chip, int offset)
{
	return GPIO_PULL_NONE;
}

static void gpio_pl061_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
}

static enum gpio_drv_t gpio_pl061_get_drv(struct gpiochip_t * chip, int offset)
{
	return GPIO_DRV_WEAK;
}

static void gpio_pl061_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpio_pl061_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void gpio_pl061_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct gpio_pl061_pdata_t * pdat = (struct gpio_pl061_pdata_t *)chip->priv;
	u8_t val;

	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val = read8(pdat->virt + GPIO_DIR);
		val &= ~(1 << offset);
		write8(pdat->virt + GPIO_DIR, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val = read8(pdat->virt + GPIO_DIR);
		val |= 1 << offset;
		write8(pdat->virt + GPIO_DIR, val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t gpio_pl061_get_dir(struct gpiochip_t * chip, int offset)
{
	struct gpio_pl061_pdata_t * pdat = (struct gpio_pl061_pdata_t *)chip->priv;
	u8_t val;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_INPUT;

	val = read8(pdat->virt + GPIO_DIR);
	if((val & (1 << offset)) == 0)
		return GPIO_DIRECTION_INPUT;
	return GPIO_DIRECTION_OUTPUT;
}

static void gpio_pl061_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_pl061_pdata_t * pdat = (struct gpio_pl061_pdata_t *)chip->priv;
	write8(pdat->virt + (1 << (offset + 2)), !!value << offset);
}

static int gpio_pl061_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_pl061_pdata_t * pdat = (struct gpio_pl061_pdata_t *)chip->priv;
	return !!read8(pdat->virt + (1 << (offset + 2)));
}

static int gpio_pl061_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_pl061_pdata_t * pdat = (struct gpio_pl061_pdata_t *)chip->priv;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;
	return pdat->oirq + offset;
}

static struct device_t * gpio_pl061_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_pl061_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x061)
		return NULL;

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_pl061_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct gpiochip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->base = base;
	pdat->ngpio = ngpio;
	pdat->oirq = dt_read_int(n, "interrupt-offset", -1);

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->ngpio = pdat->ngpio;
	chip->set_cfg = gpio_pl061_set_cfg;
	chip->get_cfg = gpio_pl061_get_cfg;
	chip->set_pull = gpio_pl061_set_pull;
	chip->get_pull = gpio_pl061_get_pull;
	chip->set_drv = gpio_pl061_set_drv;
	chip->get_drv = gpio_pl061_get_drv;
	chip->set_rate = gpio_pl061_set_rate;
	chip->get_rate = gpio_pl061_get_rate;
	chip->set_dir = gpio_pl061_set_dir;
	chip->get_dir = gpio_pl061_get_dir;
	chip->set_value = gpio_pl061_set_value;
	chip->get_value = gpio_pl061_get_value;
	chip->to_irq = gpio_pl061_to_irq;
	chip->priv = pdat;

	if(!register_gpiochip(&dev, chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void gpio_pl061_remove(struct device_t * dev)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)dev->priv;

	if(chip && unregister_gpiochip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void gpio_pl061_suspend(struct device_t * dev)
{
}

static void gpio_pl061_resume(struct device_t * dev)
{
}

static struct driver_t gpio_pl061 = {
	.name		= "gpio-pl061",
	.probe		= gpio_pl061_probe,
	.remove		= gpio_pl061_remove,
	.suspend	= gpio_pl061_suspend,
	.resume		= gpio_pl061_resume,
};

static __init void gpio_pl061_driver_init(void)
{
	register_driver(&gpio_pl061);
}

static __exit void gpio_pl061_driver_exit(void)
{
	unregister_driver(&gpio_pl061);
}

driver_initcall(gpio_pl061_driver_init);
driver_exitcall(gpio_pl061_driver_exit);
