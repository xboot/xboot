/*
 * driver/gpio-s5l8930.c
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

struct gpio_s5l8930_pdata_t
{
	virtual_addr_t virt;
	int base;
	int ngpio;
	int oirq;
};

static void gpio_s5l8930_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpio_s5l8930_pdata_t * pdat = (struct gpio_s5l8930_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(cfg)
	{
	case 0: /* Input */
		cfg = 0x8;
		break;

	case 1: /* Output */
		cfg = 0x9;
		break;

	case 2:
		cfg = 0x18;
		break;

	case 3:
		cfg = 0x28;
		break;

	case 4:
		cfg = 0x38;
		break;

	default:
		return;
	}
	val = read32(pdat->virt + offset * 4);
	val |= (0x1 << 9);
	write32(pdat->virt + offset * 4, (val & ~(0x3f << 1)) | (cfg << 1));
}

static int gpio_s5l8930_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5l8930_pdata_t * pdat = (struct gpio_s5l8930_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = (read32(pdat->virt + offset * 4) >> 1) & 0x3f;
	switch(val)
	{
	case 0x8:	/* Input */
		val = 0;
		break;

	case 0x9:	/* Output */
		val = 1;
		break;

	case 0x18:
		val = 2;
		break;

	case 0x28:
		val = 3;
		break;

	case 0x38:
		val = 4;
		break;

	default:
		val = 0;
		break;
	}
	return val;
}

static void gpio_s5l8930_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpio_s5l8930_pdata_t * pdat = (struct gpio_s5l8930_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		val = 0x3;
		break;
	case GPIO_PULL_DOWN:
		val = 0x1;
		break;
	case GPIO_PULL_NONE:
		val = 0x0;
		break;
	default:
		return;
	}
	write32(pdat->virt + offset * 4, (read32(pdat->virt + offset * 4) & ~(0x3 << 7)) | (val << 7));
}

static enum gpio_pull_t gpio_s5l8930_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5l8930_pdata_t * pdat = (struct gpio_s5l8930_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	val = read32(pdat->virt + offset * 4);
	switch((val >> 7) & 0x3)
	{
	case 0x0:
		return GPIO_PULL_NONE;
	case 0x1:
		return GPIO_PULL_DOWN;
	case 0x3:
		return GPIO_PULL_UP;
	default:
		break;
	}
	return GPIO_PULL_NONE;
}

static void gpio_s5l8930_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
}

static enum gpio_drv_t gpio_s5l8930_get_drv(struct gpiochip_t * chip, int offset)
{
	return GPIO_DRV_WEAK;
}

static void gpio_s5l8930_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpio_s5l8930_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void gpio_s5l8930_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	gpio_s5l8930_set_cfg(chip, offset, (dir == GPIO_DIRECTION_OUTPUT) ? 1 : 0);
}

static enum gpio_direction_t gpio_s5l8930_get_dir(struct gpiochip_t * chip, int offset)
{
	return (gpio_s5l8930_get_cfg(chip, offset) == 1) ? GPIO_DIRECTION_OUTPUT : GPIO_DIRECTION_INPUT;
}

static void gpio_s5l8930_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_s5l8930_pdata_t * pdat = (struct gpio_s5l8930_pdata_t *)chip->priv;

	if(offset >= chip->ngpio)
		return;
	write32(pdat->virt + offset * 4, (read32(pdat->virt + offset * 4) & ~(0x1 << 0)) | (value ? (1 << 0) : (0 << 0)));
}

static int gpio_s5l8930_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5l8930_pdata_t * pdat = (struct gpio_s5l8930_pdata_t *)chip->priv;

	if(offset >= chip->ngpio)
		return 0;
	return (read32(pdat->virt + offset * 4) & (0x1 << 0)) ? 1 : 0;
}

static int gpio_s5l8930_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5l8930_pdata_t * pdat = (struct gpio_s5l8930_pdata_t *)chip->priv;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;
	return pdat->oirq + offset;
}

static struct device_t * gpio_s5l8930_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_s5l8930_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_s5l8930_pdata_t));
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
	chip->set_cfg = gpio_s5l8930_set_cfg;
	chip->get_cfg = gpio_s5l8930_get_cfg;
	chip->set_pull = gpio_s5l8930_set_pull;
	chip->get_pull = gpio_s5l8930_get_pull;
	chip->set_drv = gpio_s5l8930_set_drv;
	chip->get_drv = gpio_s5l8930_get_drv;
	chip->set_rate = gpio_s5l8930_set_rate;
	chip->get_rate = gpio_s5l8930_get_rate;
	chip->set_dir = gpio_s5l8930_set_dir;
	chip->get_dir = gpio_s5l8930_get_dir;
	chip->set_value = gpio_s5l8930_set_value;
	chip->get_value = gpio_s5l8930_get_value;
	chip->to_irq = gpio_s5l8930_to_irq;
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

static void gpio_s5l8930_remove(struct device_t * dev)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)dev->priv;

	if(chip && unregister_gpiochip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void gpio_s5l8930_suspend(struct device_t * dev)
{
}

static void gpio_s5l8930_resume(struct device_t * dev)
{
}

static struct driver_t gpio_s5l8930 = {
	.name		= "gpio-s5l8930",
	.probe		= gpio_s5l8930_probe,
	.remove		= gpio_s5l8930_remove,
	.suspend	= gpio_s5l8930_suspend,
	.resume		= gpio_s5l8930_resume,
};

static __init void gpio_s5l8930_driver_init(void)
{
	register_driver(&gpio_s5l8930);
}

static __exit void gpio_s5l8930_driver_exit(void)
{
	unregister_driver(&gpio_s5l8930);
}

driver_initcall(gpio_s5l8930_driver_init);
driver_exitcall(gpio_s5l8930_driver_exit);
