/*
 * driver/key-rk816.c
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
#include <i2c/i2c.h>
#include <gpio/gpio.h>
#include <interrupt/interrupt.h>
#include <input/input.h>
#include <input/keyboard.h>

enum {
	RK816_CHIP_NAME 		= 0x17,
	RK816_CHIP_VER 			= 0x18,
	RK816_OTP_VER 			= 0x19,

	RK818_VB_MON			= 0x21,
	RK818_THERMAL			= 0x22,
	RK816_PWRON_LP_INT_TIME = 0x47,
	RK816_PWRON_DB 			= 0x48,
	RK816_DEV_CTRL 			= 0x4b,
	RK816_ON_SOURCE 		= 0xae,
	RK816_OFF_SOURCE 		= 0xaf,

	RK816_DCDC_EN1 			= 0x23,
	RK816_DCDC_EN2			= 0x24,
	RK816_SLP_DCDC_EN 		= 0x25,
	RK816_SLP_LDO_EN 		= 0x26,
	RK816_LDO_EN1 			= 0x27,
	RK816_LDO_EN2 			= 0x28,
	RK816_SW_BUCK_LDO_CFG	= 0x2a,
	RK816_SW2_CFG 			= 0xa6,
	RK816_BUCK5_ON_VSEL 	= 0x2b,
	RK816_BUCK5_SLP_VSEL 	= 0x2c,
	RK816_BUCK5_CFG 		= 0x2d,
	RK816_BUCK1_CFG 		= 0x2e,
	RK816_BUCK1_ON_VSEL 	= 0x2f,
	RK816_BUCK1_SLP_VSEL 	= 0x30,
	RK816_BUCK2_CFG 		= 0x32,
	RK816_BUCK2_ON_VSEL 	= 0x33,
	RK816_BUCK2_SLP_VSEL 	= 0x34,
	RK816_BUCK3_CFG 		= 0x36,
	RK816_BUCK4_CFG 		= 0x37,
	RK816_BUCK4_ON_VSEL 	= 0x38,
	RK816_BUCK4_SLP_VSEL 	= 0x39,
	RK816_LDO1_ON_VSEL 		= 0x3b,
	RK816_LDO1_SLP_VSEL 	= 0x3c,
	RK816_LDO2_ON_VSEL 		= 0x3d,
	RK816_LDO2_SLP_VSEL 	= 0x3e,
	RK816_LDO3_ON_VSEL 		= 0x3f,
	RK816_LDO3_SLP_VSEL 	= 0x40,
	RK816_LDO4_ON_VSEL 		= 0x41,
	RK816_LDO4_SLP_VSEL 	= 0x42,
	RK816_LDO5_ON_VSEL 		= 0x43,
	RK816_LDO5_SLP_VSEL 	= 0x44,
	RK816_LDO6_ON_VSEL 		= 0x45,
	RK816_LDO6_SLP_VSEL 	= 0x46,

	RK816_INT_STS1			= 0x49,
	RK816_INT_STS1_MSK		= 0x4a,
	RK816_INT_STS2			= 0x4c,
	RK816_INT_STS2_MSK		= 0x4d,
	RK816_INT_STS3			= 0x4e,
	RK816_INT_STS3_MSK		= 0x4f,
	RK816_GPIO_IO_POL		= 0x50,
};

struct key_rk816_pdata_t {
	struct i2c_device_t * dev;
	int irq;
};

static bool_t rk816_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t rk816_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static void key_rk816_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct key_rk816_pdata_t * pdat = (struct key_rk816_pdata_t *)input->priv;
	u8_t val = 0;

	rk816_read(pdat->dev, RK816_INT_STS1, &val);
	if(val != 0)
	{
		if(val & (1 << 5))
			push_event_key_down(input, KEY_POWER);
		else if(val & (1 << 6))
			push_event_key_up(input, KEY_POWER);
		rk816_write(pdat->dev, RK816_INT_STS1, val);
	}

	rk816_read(pdat->dev, RK816_INT_STS2, &val);
	if(val != 0)
	{
		rk816_write(pdat->dev, RK816_INT_STS2, val);
	}

	rk816_read(pdat->dev, RK816_INT_STS3, &val);
	if(val != 0)
	{
		rk816_write(pdat->dev, RK816_INT_STS3, val);
	}
}

static int key_rk816_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * key_rk816_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct key_rk816_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	int gpio = dt_read_int(n, "interrupt-gpio", -1);
	int irq = gpio_to_irq(gpio);
	u8_t val;

	if(!gpio_is_valid(gpio) || !irq_is_valid(irq))
		return NULL;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x1c), 0);
	if(!i2cdev)
		return NULL;

	if(rk816_read(i2cdev, RK816_CHIP_VER, &val))
	{
		rk816_write(i2cdev, RK816_INT_STS1_MSK, 0x9f);
		rk816_write(i2cdev, RK816_INT_STS2_MSK, 0xff);
		rk816_write(i2cdev, RK816_INT_STS3_MSK, 0xfc);
		rk816_write(i2cdev, RK816_INT_STS1, 0xff);
		rk816_write(i2cdev, RK816_INT_STS2, 0xff);
		rk816_write(i2cdev, RK816_INT_STS3, 0xff);
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct key_rk816_pdata_t));
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
	input->ioctl = key_rk816_ioctl;
	input->priv = pdat;

	gpio_set_pull(gpio, GPIO_PULL_UP);
	gpio_direction_input(gpio);
	request_irq(pdat->irq, key_rk816_interrupt, IRQ_TYPE_EDGE_FALLING, input);

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

static void key_rk816_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_rk816_pdata_t * pdat = (struct key_rk816_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		free_irq(pdat->irq);
		i2c_device_free(pdat->dev);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void key_rk816_suspend(struct device_t * dev)
{
}

static void key_rk816_resume(struct device_t * dev)
{
}

static struct driver_t key_rk816 = {
	.name		= "key-rk816",
	.probe		= key_rk816_probe,
	.remove		= key_rk816_remove,
	.suspend	= key_rk816_suspend,
	.resume		= key_rk816_resume,
};

static __init void key_rk816_driver_init(void)
{
	register_driver(&key_rk816);
}

static __exit void key_rk816_driver_exit(void)
{
	unregister_driver(&key_rk816);
}

driver_initcall(key_rk816_driver_init);
driver_exitcall(key_rk816_driver_exit);
