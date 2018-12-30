/*
 * driver/gpio-s5p6818-alv.c
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
	GPIOALV_PWRGATEREG					= 0x800,
	GPIOALV_ASYNCDETECTMODERSTREG0		= 0x804,
	GPIOALV_ASYNCDETECTMODESETREG0		= 0x808,
	GPIOALV_LOWASYNCDETECTMODEREADREG	= 0x80C,
	GPIOALV_ASYNCDETECTMODERSTREG1		= 0x810,
	GPIOALV_ASYNCDETECTMODESETREG1		= 0x814,
	GPIOALV_HIGHASYNCDETECTMODEREADREG	= 0x818,
	GPIOALV_DETECTMODERSTREG0			= 0x81C,
	GPIOALV_DETECTMODESETREG0			= 0x820,
	GPIOALV_FALLDETECTMODEREADREG		= 0x824,
	GPIOALV_DETECTMODERSTREG1			= 0x828,
	GPIOALV_DETECTMODESETREG1			= 0x82C,
	GPIOALV_RISEDETECTMODEREADREG		= 0x830,
	GPIOALV_DETECTMODERSTREG2			= 0x834,
	GPIOALV_DETECTMODESETREG2			= 0x838,
	GPIOALV_LOWDETECTMODEREADREG		= 0x83C,
	GPIOALV_DETECTMODERSTREG3			= 0x840,
	GPIOALV_DETECTMODESETREG3			= 0x844,
	GPIOALV_HIGHDETECTMODEREADREG		= 0x848,
	GPIOALV_DETECTENBRSTREG				= 0x84C,
	GPIOALV_DETECTENBSETREG				= 0x850,
	GPIOALV_DETECTENBREADREG			= 0x854,
	GPIOALV_INTENBRSTREG				= 0x858,
	GPIOALV_INTENBSETREG				= 0x85C,
	GPIOALV_INTENBREADREG				= 0x860,
	GPIOALV_DETECTPENDREG				= 0x864,
	GPIOALV_SCRATCHRSTREG				= 0x868,
	GPIOALV_SCRATCHSETREG				= 0x86C,
	GPIOALV_SCRATCHREADREG				= 0x870,
	GPIOALV_PADOUTENBRSTREG				= 0x874,
	GPIOALV_PADOUTENBSETREG				= 0x878,
	GPIOALV_PADOUTENBREADREG			= 0x87C,
	GPIOALV_PADPULLUPRSTREG				= 0x880,
	GPIOALV_PADPULLUPSETREG				= 0x884,
	GPIOALV_PADPULLUPREADREG			= 0x888,
	GPIOALV_PADOUTRSTREG				= 0x88C,
	GPIOALV_PADOUTSETREG				= 0x890,
	GPIOALV_PADOUTREADREG				= 0x894,
	GPIOALV_VDDCTRLRSTREG				= 0x898,
	GPIOALV_VDDCTRLSETREG				= 0x89C,
	GPIOALV_VDDCTRLREADREG				= 0x8A0,
	GPIOALV_CLEARWAKEUPSTATUS			= 0x8A4,
	GPIOALV_SLEEPWAKEUPSTATUS			= 0x8A8,
	GPIOALV_SCRATCHRST1					= 0x8AC,
	GPIOALV_SCRATCHRST2					= 0x8B8,
	GPIOALV_SCRATCHRST3					= 0x8C4,
	GPIOALV_SCRATCHRST4					= 0x8D0,
	GPIOALV_SCRATCHRST5					= 0x8DC,
	GPIOALV_SCRATCHRST6					= 0x8E8,
	GPIOALV_SCRATCHRST7					= 0x8F4,
	GPIOALV_SCRATCHRST8					= 0x900,
	GPIOALV_SCRATCHSET1					= 0x8B0,
	GPIOALV_SCRATCHSET2					= 0x8BC,
	GPIOALV_SCRATCHSET3					= 0x8C8,
	GPIOALV_SCRATCHSET4					= 0x8D4,
	GPIOALV_SCRATCHSET5					= 0x8E0,
	GPIOALV_SCRATCHSET6					= 0x8EC,
	GPIOALV_SCRATCHSET7					= 0x8F8,
	GPIOALV_SCRATCHSET8					= 0x904,
	GPIOALV_SCRATCHREAD1				= 0x8B4,
	GPIOALV_SCRATCHREAD2				= 0x8C0,
	GPIOALV_SCRATCHREAD3				= 0x8CC,
	GPIOALV_SCRATCHREAD4				= 0x8D8,
	GPIOALV_SCRATCHREAD5				= 0x8E4,
	GPIOALV_SCRATCHREAD6				= 0x8F0,
	GPIOALV_SCRATCHREAD7				= 0x8FC,
	GPIOALV_SCRATCHREAD8				= 0x908,
	GPIOALV_VDDOFFDELAYRST				= 0x90C,
	GPIOALV_VDDOFFDELAYSET				= 0x910,
	GPIOALV_VDDOFFDELAYVAL				= 0x914,
	GPIOALV_VDDOFFDELAYIME				= 0x918,
	GPIOALV_GPIOINPUTVALUE				= 0x91C,
	GPIOALV_PMUNISOLATE					= 0xD00,
	GPIOALV_PMUNPWRUPPRE				= 0xD04,
	GPIOALV_PMUNPWRUP					= 0xD08,
	GPIOALV_PMUNPWRUPACK				= 0xD0C,
};

struct gpio_s5p6818_alv_pdata_t
{
	virtual_addr_t virt;
	int base;
	int ngpio;
	int oirq;
};

static inline void gpiochip_write_enable(struct gpiochip_t * chip)
{
	struct gpio_s5p6818_alv_pdata_t * pdat = (struct gpio_s5p6818_alv_pdata_t *)chip->priv;
	write32(pdat->virt + GPIOALV_PWRGATEREG, 0x1);
}

static inline void gpiochip_write_disable(struct gpiochip_t * chip)
{
	struct gpio_s5p6818_alv_pdata_t * pdat = (struct gpio_s5p6818_alv_pdata_t *)chip->priv;
	write32(pdat->virt + GPIOALV_PWRGATEREG, 0x0);
}

static void gpio_s5p6818_alv_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
}

static int gpio_s5p6818_alv_get_cfg(struct gpiochip_t * chip, int offset)
{
	return 0;
}

static void gpio_s5p6818_alv_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpio_s5p6818_alv_pdata_t * pdat = (struct gpio_s5p6818_alv_pdata_t *)chip->priv;
	u32_t val = 0;

	if(offset >= chip->ngpio)
		return;

	gpiochip_write_enable(chip);
	switch(pull)
	{
	case GPIO_PULL_UP:
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADPULLUPSETREG, val);
		break;

	case GPIO_PULL_DOWN:
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADPULLUPRSTREG, val);
		break;

	case GPIO_PULL_NONE:
		break;

	default:
		break;
	}
	gpiochip_write_disable(chip);
}

static enum gpio_pull_t gpio_s5p6818_alv_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_alv_pdata_t * pdat = (struct gpio_s5p6818_alv_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	val = read32(pdat->virt + GPIOALV_PADPULLUPREADREG);
	if(!((val >> offset) & 0x1))
		return GPIO_PULL_DOWN;
	else
		return GPIO_PULL_UP;

	return GPIO_PULL_NONE;
}

static void gpio_s5p6818_alv_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
}

static enum gpio_drv_t gpio_s5p6818_alv_get_drv(struct gpiochip_t * chip, int offset)
{
	return GPIO_DRV_WEAK;
}

static void gpio_s5p6818_alv_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpio_s5p6818_alv_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void gpio_s5p6818_alv_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct gpio_s5p6818_alv_pdata_t * pdat = (struct gpio_s5p6818_alv_pdata_t *)chip->priv;
	u32_t val = 0;

	if(offset >= chip->ngpio)
		return;

	gpiochip_write_enable(chip);
	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADOUTENBRSTREG, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADOUTENBSETREG, val);
		break;

	default:
		break;
	}
	gpiochip_write_disable(chip);
}

static enum gpio_direction_t gpio_s5p6818_alv_get_dir(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_alv_pdata_t * pdat = (struct gpio_s5p6818_alv_pdata_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_INPUT;

	val = read32(pdat->virt + GPIOALV_PADOUTENBREADREG);
	d = (val >> offset) & 0x1;
	switch(d)
	{
	case 0x0:
		return GPIO_DIRECTION_INPUT;
	case 0x1:
		return GPIO_DIRECTION_OUTPUT;
	default:
		break;
	}
	return GPIO_DIRECTION_INPUT;
}

static void gpio_s5p6818_alv_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_s5p6818_alv_pdata_t * pdat = (struct gpio_s5p6818_alv_pdata_t *)chip->priv;
	u32_t val = 0;

	if(offset >= chip->ngpio)
		return;

	gpiochip_write_enable(chip);
	if(value)
	{
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADOUTSETREG, val);
	}
	else
	{
		val |= 0x1 << offset;
		write32(pdat->virt + GPIOALV_PADOUTRSTREG, val);
	}
	gpiochip_write_disable(chip);
}

static int gpio_s5p6818_alv_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_alv_pdata_t * pdat = (struct gpio_s5p6818_alv_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = read32(pdat->virt + GPIOALV_GPIOINPUTVALUE);
	return !!(val & (1 << offset));
}

static int gpio_s5p6818_alv_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_alv_pdata_t * pdat = (struct gpio_s5p6818_alv_pdata_t *)chip->priv;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;
	return pdat->oirq + offset;
}

static struct device_t * gpio_s5p6818_alv_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_s5p6818_alv_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_s5p6818_alv_pdata_t));
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
	chip->set_cfg = gpio_s5p6818_alv_set_cfg;
	chip->get_cfg = gpio_s5p6818_alv_get_cfg;
	chip->set_pull = gpio_s5p6818_alv_set_pull;
	chip->get_pull = gpio_s5p6818_alv_get_pull;
	chip->set_drv = gpio_s5p6818_alv_set_drv;
	chip->get_drv = gpio_s5p6818_alv_get_drv;
	chip->set_rate = gpio_s5p6818_alv_set_rate;
	chip->get_rate = gpio_s5p6818_alv_get_rate;
	chip->set_dir = gpio_s5p6818_alv_set_dir;
	chip->get_dir = gpio_s5p6818_alv_get_dir;
	chip->set_value = gpio_s5p6818_alv_set_value;
	chip->get_value = gpio_s5p6818_alv_get_value;
	chip->to_irq = gpio_s5p6818_alv_to_irq;
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

static void gpio_s5p6818_alv_remove(struct device_t * dev)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)dev->priv;

	if(chip && unregister_gpiochip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void gpio_s5p6818_alv_suspend(struct device_t * dev)
{
}

static void gpio_s5p6818_alv_resume(struct device_t * dev)
{
}

static struct driver_t gpio_s5p6818_alv = {
	.name		= "gpio-s5p6818-alv",
	.probe		= gpio_s5p6818_alv_probe,
	.remove		= gpio_s5p6818_alv_remove,
	.suspend	= gpio_s5p6818_alv_suspend,
	.resume		= gpio_s5p6818_alv_resume,
};

static __init void gpio_s5p6818_alv_driver_init(void)
{
	register_driver(&gpio_s5p6818_alv);
}

static __exit void gpio_s5p6818_alv_driver_exit(void)
{
	unregister_driver(&gpio_s5p6818_alv);
}

driver_initcall(gpio_s5p6818_alv_driver_init);
driver_exitcall(gpio_s5p6818_alv_driver_exit);
