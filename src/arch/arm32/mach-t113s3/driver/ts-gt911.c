/*
 * driver/ts-gt911.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
	GT911_CONFIG_DATA		= 0x8047,
	GT911_PRODUCT_ID		= 0x8140,
	GT911_FIRMWARE_VERSION	= 0x8144,
	GT911_STATUS			= 0x814E,
	GT911_COOR_ADDR			= 0x814F,
};

struct ts_gt911_pdata_t {
	struct i2c_device_t * dev;
	int irq;
	struct {
		int x, y;
		int press;
		int valid;
	} node[5];
};

static bool_t gt911_read(struct i2c_device_t * dev, u16_t reg, u8_t * buf, int len)
{
	struct i2c_msg_t msgs[2];
	u8_t mbuf[2];

	mbuf[0] = (reg >> 8) & 0xff;
	mbuf[1] = (reg >> 0) & 0xff;
	msgs[0].addr = dev->addr;
	msgs[0].flags = 0;
	msgs[0].len = 2;
	msgs[0].buf = &mbuf[0];

	msgs[1].addr = dev->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = len;
	msgs[1].buf = buf;

	if(i2c_transfer(dev->i2c, msgs, 2) != 2)
		return FALSE;
	return TRUE;
}

static bool_t gt911_write(struct i2c_device_t * dev, u16_t reg, u8_t * buf, int len)
{
	struct i2c_msg_t msg;
	u8_t mbuf[256];

	if(len > sizeof(mbuf) - 1)
		len = sizeof(mbuf) - 1;
	mbuf[0] = (reg >> 8) & 0xff;
	mbuf[1] = (reg >> 0) & 0xff;
	memcpy(&mbuf[2], buf, len);

	msg.addr = dev->addr;
	msg.flags = 0;
	msg.len = len + 2;
	msg.buf = &mbuf[0];

	if(i2c_transfer(dev->i2c, &msg, 1) != 1)
		return FALSE;
	return TRUE;
}

static const u8_t gt911_config_data[] = {
	0x41, 0x20, 0x03, 0xE0, 0x01, 0x05, 0x0C, 0x00, 0x01, 0x0A,
	0x28, 0x0F, 0x50, 0x32, 0x03, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x17, 0x19, 0x1C, 0x14, 0x87, 0x29, 0x0A,
	0x66, 0x68, 0xEB, 0x04, 0x00, 0x00, 0x01, 0x00, 0x02, 0x11,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x5A, 0x8C, 0x94, 0xC5, 0x02, 0x07, 0x19, 0x00, 0x04,
	0x93, 0x5E, 0x00, 0x87, 0x66, 0x00, 0x7C, 0x70, 0x00, 0x72,
	0x7A, 0x00, 0x6B, 0x86, 0x00, 0x6A, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10,
	0x12, 0x14, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x1D,
	0x1E, 0x1F, 0x20, 0x21, 0x22, 0x24, 0x26, 0x28, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xBB, 0x01,
};

static bool_t gt911_send_cfg(struct i2c_device_t * dev, u8_t * cfg, int len)
{
	u8_t sum = 0;
	int i;

	for(i = 0; i < len - 2; i++)
		sum += cfg[i];
	sum = (~sum) + 1;

	if(sum != cfg[len - 2])
		return FALSE;
	if(cfg[len - 1] != 1)
		return FALSE;
	return gt911_write(dev, GT911_CONFIG_DATA, cfg, len);
}

static bool_t gt911_initial(struct i2c_device_t * dev)
{
	u8_t cfg;
	u8_t id[4];
	u8_t ver[2];

	if(!gt911_read(dev, GT911_CONFIG_DATA, &cfg, 1))
		return FALSE;
	if(!gt911_read(dev, GT911_PRODUCT_ID, &id[0], 4))
		return FALSE;
	if(!gt911_read(dev, GT911_FIRMWARE_VERSION, &ver[0], 2))
		return FALSE;

	LOG("GT911 Version: %c%c%c%c(0x%02x%02x)(0x%02x)", id[0], id[1], id[2], id[3], ver[1], ver[0], cfg);
	return gt911_send_cfg(dev, (u8_t *)gt911_config_data, ARRAY_SIZE(gt911_config_data));
}

static void gt911_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct ts_gt911_pdata_t * pdat = (struct ts_gt911_pdata_t *)input->priv;
	u8_t status, buf[40];
	u8_t * p;
	int count, i;
	int id, x, y;

	disable_irq(pdat->irq);
	for(i = 0; i < 5; i++)
	{
		pdat->node[i].valid = 0;
	}
	if(gt911_read(pdat->dev, GT911_STATUS, &status, 1) && (status & (1 << 7)))
	{
		count = status & 0x0f;
		if(count > 0 && count < 5)
		{
			if(gt911_read(pdat->dev, GT911_COOR_ADDR, &buf[0], count << 3))
			{
				for(i = 0; i < count; i++)
				{
					p = &buf[i << 3];
					id = p[0];
					x = (p[2] << 8) | (p[1] << 0);
					y = (p[4] << 8) | (p[3] << 0);
					if(pdat->node[id].x != x || pdat->node[id].y != y)
					{
						if(pdat->node[id].press == 0)
						{
							push_event_touch_begin(input, x, y, id);
							pdat->node[id].press = 1;
						}
						else if(pdat->node[id].press == 1)
						{
							push_event_touch_move(input, x, y, id);
						}
					}
					pdat->node[id].x = x;
					pdat->node[id].y = y;
					pdat->node[id].valid = 1;
				}
			}
		}
		status = 0;
		gt911_write(pdat->dev, GT911_STATUS, &status, 1);
	}
	for(i = 0; i < 5; i++)
	{
		if((pdat->node[i].press == 1) && (pdat->node[i].valid == 0))
		{
			push_event_touch_end(input, pdat->node[i].x, pdat->node[i].y, i);
			pdat->node[i].press = 0;
		}
	}
	enable_irq(pdat->irq);
}

static int ts_gt911_ioctl(struct input_t * input, const char * cmd, void * arg)
{
	return -1;
}

static struct device_t * ts_gt911_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ts_gt911_pdata_t * pdat;
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

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x14), 0);
	if(!i2cdev)
		return NULL;

	if(rst >= 0)
	{
		if(rstcfg >= 0)
			gpio_set_cfg(rst, rstcfg);
		gpio_set_pull(rst, GPIO_PULL_UP);
		gpio_set_direction(rst, GPIO_DIRECTION_OUTPUT);

		gpio_set_value(rst, 0);
		mdelay(10);
		gpio_set_pull(gpio, GPIO_PULL_UP);
		gpio_set_direction(gpio, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(gpio, 1);
		udelay(100);
		gpio_set_value(rst, 1);
		mdelay(5);
	}
	if(!gt911_initial(i2cdev))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct ts_gt911_pdata_t));
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

	memset(pdat, 0, sizeof(struct ts_gt911_pdata_t));
	pdat->dev = i2cdev;
	pdat->irq = irq;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->ioctl = ts_gt911_ioctl;
	input->priv = pdat;

	if(gpio >= 0)
	{
		if(gpiocfg >= 0)
			gpio_set_cfg(gpio, gpiocfg);
		gpio_set_pull(gpio, GPIO_PULL_DOWN);
	}
	request_irq(pdat->irq, gt911_interrupt, IRQ_TYPE_EDGE_RISING, input);

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

static void ts_gt911_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct ts_gt911_pdata_t * pdat = (struct ts_gt911_pdata_t *)input->priv;

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

static void ts_gt911_suspend(struct device_t * dev)
{
}

static void ts_gt911_resume(struct device_t * dev)
{
}

static struct driver_t ts_gt911 = {
	.name		= "ts-gt911",
	.probe		= ts_gt911_probe,
	.remove		= ts_gt911_remove,
	.suspend	= ts_gt911_suspend,
	.resume		= ts_gt911_resume,
};

static __init void ts_gt911_driver_init(void)
{
	register_driver(&ts_gt911);
}

static __exit void ts_gt911_driver_exit(void)
{
	unregister_driver(&ts_gt911);
}

driver_initcall(ts_gt911_driver_init);
driver_exitcall(ts_gt911_driver_exit);
