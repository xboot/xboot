/*
 * driver/ts-ft6336u.c
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
#include <i2c/i2c.h>
#include <gpio/gpio.h>
#include <interrupt/interrupt.h>
#include <input/input.h>

enum {
	FT6336U_MODE_SWITCH				= 0x00,
	FT6336U_TD_STATUS				= 0x02,
	FT6336U_P1_XH					= 0x03,
	FT6336U_P1_XL					= 0x04,
	FT6336U_P1_YH					= 0x05,
	FT6336U_P1_YL					= 0x06,
	FT6336U_P1_WEIGHT				= 0x07,
	FT6336U_P1_MISC					= 0x08,
	FT6336U_P2_XH					= 0x09,
	FT6336U_P2_XL					= 0x0a,
	FT6336U_P2_YH					= 0x0b,
	FT6336U_P2_YL					= 0x0c,
	FT6336U_P2_WEIGHT				= 0x0d,
	FT6336U_P2_MISC					= 0x0e,
	FT6336U_ID_G_THGROUP			= 0x80,
	FT6336U_ID_G_THDIFF				= 0x85,
	FT6336U_ID_G_CTRL				= 0x86,
	FT6336U_ID_G_TIMEENTERMONITOR	= 0x87,
	FT6336U_ID_G_PERIODACTIVE		= 0x88,
	FT6336U_ID_G_PERIODMONITOR		= 0x89,
	FT6336U_ID_G_FREQ_HOPPING_EN	= 0x8b,
	FT6336U_ID_G_TEST_MODE_FILTER	= 0x96,
	FT6336U_ID_G_CIPHER_MID			= 0x9f,
	FT6336U_ID_G_CIPHER_LOW			= 0xa0,
	FT6336U_ID_G_LIB_VERSION_H		= 0xa1,
	FT6336U_ID_G_LIB_VERSION_L		= 0xa2,
	FT6336U_ID_G_CIPHER_HIGH		= 0xa3,
	FT6336U_ID_G_MODE				= 0xa4,
	FT6336U_ID_G_PMODE				= 0xa5,
	FT6336U_ID_G_FIRMID				= 0xa6,
	FT6336U_ID_G_FOCALTECH_ID		= 0xa8,
	FT6336U_ID_G_VIRTUAL_KEY_THRES	= 0xa9,
	FT6336U_ID_G_IS_CALLING			= 0xad,
	FT6336U_ID_G_FACTORY_MODE		= 0xae,
	FT6336U_ID_G_RELEASE_CODE_ID	= 0xaf,
	FT6336U_ID_G_FACE_DEC_MODE		= 0xb0,
	FT6336U_ID_G_STATE				= 0xbc,
	FT6336U_ID_G_GESTURE_ENABLE		= 0xd0,
};

struct ts_ft6336u_pdata_t {
	struct i2c_device_t * dev;
	int irq;
};

static bool_t ft6336u_read(struct i2c_device_t * dev, u8_t reg, u8_t * buf, int len)
{
	struct i2c_msg_t msgs[2];

    msgs[0].addr = dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = len;
    msgs[1].buf = buf;

    if(i2c_transfer(dev->i2c, msgs, 2) != 2)
    	return FALSE;
    return TRUE;
}

static bool_t ft6336u_write(struct i2c_device_t * dev, u8_t reg, u8_t * buf, int len)
{
	struct i2c_msg_t msg;
	u8_t mbuf[256];

	if(len > sizeof(mbuf) - 1)
		len = sizeof(mbuf) - 1;
	mbuf[0] = reg;
	memcpy(&mbuf[1], buf, len);

    msg.addr = dev->addr;
    msg.flags = 0;
    msg.len = len + 1;
    msg.buf = &mbuf[0];

    if(i2c_transfer(dev->i2c, &msg, 1) != 1)
    	return FALSE;
    return TRUE;
}

static bool_t ft6336u_initial(struct i2c_device_t * dev)
{
	u8_t h, m, l;
	u8_t vh, vl;

	if(!ft6336u_write(dev, FT6336U_MODE_SWITCH, (u8_t[]){ 0 }, 1))
		return FALSE;
	if(!ft6336u_read(dev, FT6336U_ID_G_CIPHER_LOW, &l, 1) || (l != 0x02))
		return FALSE;
	if(!ft6336u_read(dev, FT6336U_ID_G_CIPHER_MID, &m, 1))
		return FALSE;
	if(!ft6336u_read(dev, FT6336U_ID_G_CIPHER_HIGH, &h, 1))
		return FALSE;
	if(!ft6336u_read(dev, FT6336U_ID_G_LIB_VERSION_H, &vh, 1))
		return FALSE;
	if(!ft6336u_read(dev, FT6336U_ID_G_LIB_VERSION_L, &vl, 1))
		return FALSE;
	LOG("FT6336U chip ID: 0x%02x%02x%02x, Library version: 0x%02x%02x", h, m, l, vh, vl);
	return TRUE;
}

static void ft6336u_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct ts_ft6336u_pdata_t * pdat = (struct ts_ft6336u_pdata_t *)input->priv;
	u8_t buf[4];

	disable_irq(pdat->irq);
	if(ft6336u_read(pdat->dev, FT6336U_P1_XH, buf, 4))
	{
		int x = ((buf[0] << 8) | buf[1]) & 0xfff;
		int y = ((buf[2] << 8) | buf[3]) & 0xfff;
		switch((buf[0] >> 6) & 0x3)
		{
		case 0x0:
			push_event_touch_begin(input, x, y, 0);
			break;
		case 0x2:
			push_event_touch_move(input, x, y, 0);
			break;
		case 0x1:
			push_event_touch_end(input, x, y, 0);
			break;
		default:
			break;
		}
	}
	enable_irq(pdat->irq);
}

static int ts_ft6336u_ioctl(struct input_t * input, const char * cmd, void * arg)
{
	return -1;
}

static struct device_t * ts_ft6336u_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ts_ft6336u_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	int gpio = dt_read_int(n, "interrupt-gpio", -1);
	int gpiocfg = dt_read_int(n, "interrupt-gpio-config", -1);
	int irq = gpio_to_irq(gpio);
	int rst = dt_read_int(n, "reset-gpio", -1);
	int rstcfg = dt_read_int(n, "reset-gpio-config", -1);

	if(!gpio_is_valid(gpio) || !irq_is_valid(irq))
		return NULL;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x48), 0);
	if(!i2cdev)
		return NULL;

	if(rst >= 0)
	{
		if(rstcfg >= 0)
			gpio_set_cfg(rst, rstcfg);
		gpio_set_pull(rst, GPIO_PULL_UP);
		gpio_set_direction(rst, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(rst, 0);
		mdelay(50);
		gpio_set_value(rst, 1);
		mdelay(200);
	}
	if(!ft6336u_initial(i2cdev))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}
	pdat = malloc(sizeof(struct ts_ft6336u_pdata_t));
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

	memset(pdat, 0, sizeof(struct ts_ft6336u_pdata_t));
	pdat->dev = i2cdev;
	pdat->irq = irq;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->ioctl = ts_ft6336u_ioctl;
	input->priv = pdat;

	if(gpio >= 0)
	{
		if(gpiocfg >= 0)
			gpio_set_cfg(gpio, gpiocfg);
		gpio_set_pull(gpio, GPIO_PULL_NONE);
	}
	request_irq(pdat->irq, ft6336u_interrupt, IRQ_TYPE_EDGE_FALLING, input);

	if(!(dev = register_input(input, drv)))
	{
		free_irq(pdat->irq);
		i2c_device_free(pdat->dev);
		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	return dev;
}

static void ts_ft6336u_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct ts_ft6336u_pdata_t * pdat = (struct ts_ft6336u_pdata_t *)input->priv;

	if(input)
	{
		unregister_input(input);
		free_irq(pdat->irq);
		i2c_device_free(pdat->dev);
		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void ts_ft6336u_suspend(struct device_t * dev)
{
}

static void ts_ft6336u_resume(struct device_t * dev)
{
}

static struct driver_t ts_ft6336u = {
	.name		= "ts-ft6336u",
	.probe		= ts_ft6336u_probe,
	.remove		= ts_ft6336u_remove,
	.suspend	= ts_ft6336u_suspend,
	.resume		= ts_ft6336u_resume,
};

static __init void ts_ft6336u_driver_init(void)
{
	register_driver(&ts_ft6336u);
}

static __exit void ts_ft6336u_driver_exit(void)
{
	unregister_driver(&ts_ft6336u);
}

driver_initcall(ts_ft6336u_driver_init);
driver_exitcall(ts_ft6336u_driver_exit);
