/*
 * driver/key-rc5t620.c
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
#include <i2c/i2c.h>
#include <gpio/gpio.h>
#include <interrupt/interrupt.h>
#include <input/input.h>
#include <input/keyboard.h>

enum {
	RC5T620_LSIVER			= 0x00,
	RC5T620_OTPVER			= 0x01,
	RC5T620_IODAC			= 0x02,
	RC5T620_VINDAC			= 0x03,
	RC5T620_OUT32KEN		= 0x05,

	RC5T620_PSWR			= 0x07,
	RC5T620_PONHIS			= 0x09,
	RC5T620_POFFHIS			= 0x0a,
	RC5T620_WATCHDOG		= 0x0b,
	RC5T620_WATCHDOGCNT		= 0x0c,
	RC5T620_PWRFUNC			= 0x0d,
	RC5T620_SLPCNT			= 0x0e,
	RC5T620_REPCNT			= 0x0f,
	RC5T620_PWRONTIMSET		= 0x10,
	RC5T620_NOETIMESETCNT	= 0x11,
	RC5T620_PWRIREN			= 0x12,
	RC5T620_PWRIRQ			= 0x13,
	RC5T620_PWRMON			= 0x14,
	RC5T620_PWRIRSEL		= 0x15,
	RC5T620_DC1SLOT			= 0x16,
	RC5T620_DC2SLOT			= 0x17,
	RC5T620_DC3SLOT			= 0x18,
	RC5T620_DC4SLOT			= 0x19,
	RC5T620_DC5SLOT			= 0x1a,
	RC5T620_LDO1SLOT		= 0x1b,
	RC5T620_LDO2SLOT		= 0x1c,
	RC5T620_LDO3SLOT		= 0x1d,
	RC5T620_LDO4SLOT		= 0x1e,
	RC5T620_LDO5SLOT		= 0x1f,
	RC5T620_LDO6SLOT		= 0x20,
	RC5T620_LDO7SLOT		= 0x21,
	RC5T620_LDO8SLOT		= 0x22,
	RC5T620_LDO9SLOT		= 0x23,
	RC5T620_LDO10SLOT		= 0x24,
	RC5T620_PSO0SLOT		= 0x25,
	RC5T620_PSO1SLOT		= 0x26,
	RC5T620_PSO2SLOT		= 0x27,
	RC5T620_PSO3SLOT		= 0x28,
	RC5T620_PSO4SLOT		= 0x29,
	RC5T620_LDORTC1SLOT		= 0x2a,

	RC5T620_INTPOL			= 0x9c,
	RC5T620_INTEN			= 0x9d,
	RC5T620_INTMON			= 0x9e,
};

struct key_rc5t620_pdata_t {
	struct i2c_device_t * dev;
	int irq;
};

static bool_t rc5t620_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
{
	struct i2c_msg_t msgs[2];
	u8_t buf;

	msgs[0].addr = dev->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &reg;

	msgs[1].addr = dev->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = 1;
	msgs[1].buf = &buf;

	if(i2c_transfer(dev->i2c, msgs, 2) != 2)
		return FALSE;

	if(val)
		*val = buf;
	return TRUE;
}

static bool_t rc5t620_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
{
	struct i2c_msg_t msg;
	u8_t buf[2];

	buf[0] = reg;
	buf[1] = val;
    msg.addr = dev->addr;
    msg.flags = 0;
    msg.len = 2;
    msg.buf = &buf[0];

    if(i2c_transfer(dev->i2c, &msg, 1) != 1)
    	return FALSE;
    return TRUE;
}

static void key_rc5t620_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct key_rc5t620_pdata_t * pdat = (struct key_rc5t620_pdata_t *)input->priv;
	u8_t val = 0;

	rc5t620_read(pdat->dev, RC5T620_PWRMON, &val);
	if(val & 0x1)
		push_event_key_down(input, KEY_POWER);
	else
		push_event_key_up(input, KEY_POWER);

	rc5t620_read(pdat->dev, RC5T620_PWRIRQ, &val);
	val &= ~(0x1 << 0);
	rc5t620_write(pdat->dev, RC5T620_PWRIRQ, val);
}

static int key_rc5t620_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * key_rc5t620_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct key_rc5t620_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	int gpio = dt_read_int(n, "interrupt-gpio", -1);
	int irq = gpio_to_irq(gpio);
	u8_t val;

	if(!gpio_is_valid(gpio) || !irq_is_valid(irq))
		return NULL;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x32), 0);
	if(!i2cdev)
		return NULL;

	if(rc5t620_read(i2cdev, RC5T620_LSIVER, &val) && (val == 0x03))
	{
		rc5t620_write(i2cdev, RC5T620_PWRIRQ, 0x00);
		rc5t620_write(i2cdev, RC5T620_PWRIRSEL, 0x01);
		rc5t620_write(i2cdev, RC5T620_INTEN, 0x01);
		rc5t620_write(i2cdev, RC5T620_PWRIREN, 0x01);
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct key_rc5t620_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	pdat->dev = i2cdev;
	pdat->irq = irq;

	input->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	input->type = INPUT_TYPE_KEYBOARD;
	input->ioctl = key_rc5t620_ioctl;
	input->priv = pdat;

	gpio_set_pull(gpio, GPIO_PULL_UP);
	gpio_direction_input(gpio);
	request_irq(pdat->irq, key_rc5t620_interrupt, IRQ_TYPE_EDGE_FALLING, input);

	if(!register_input(&dev, input))
	{
		free_irq(pdat->irq);
		i2c_device_free(pdat->dev);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void key_rc5t620_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_rc5t620_pdata_t * pdat = (struct key_rc5t620_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		free_irq(pdat->irq);
		i2c_device_free(pdat->dev);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void key_rc5t620_suspend(struct device_t * dev)
{
}

static void key_rc5t620_resume(struct device_t * dev)
{
}

static struct driver_t key_rc5t620 = {
	.name		= "key-rc5t620",
	.probe		= key_rc5t620_probe,
	.remove		= key_rc5t620_remove,
	.suspend	= key_rc5t620_suspend,
	.resume		= key_rc5t620_resume,
};

static __init void key_rc5t620_driver_init(void)
{
	register_driver(&key_rc5t620);
}

static __exit void key_rc5t620_driver_exit(void)
{
	unregister_driver(&key_rc5t620);
}

driver_initcall(key_rc5t620_driver_init);
driver_exitcall(key_rc5t620_driver_exit);
