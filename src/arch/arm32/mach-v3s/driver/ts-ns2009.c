/*
 * driver/ts-ns2009.c
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
#include <tsfilter.h>

enum {
	NS2009_LOW_POWER_READ_X		= 0xc0,
	NS2009_LOW_POWER_READ_Y		= 0xd0,
	NS2009_LOW_POWER_READ_Z1	= 0xe0,
	NS2009_LOW_POWER_READ_Z2	= 0xf0,
};

struct ts_ns2009_pdata_t {
	struct timer_t timer;
	struct i2c_device_t * dev;
	struct tsfilter_t * filter;
	int interval;
	int x, y;
	int press;
};

static bool_t ns2009_read(struct i2c_device_t * dev, u8_t cmd, int * val)
{
	struct i2c_msg_t msgs[2];
	u8_t buf[2];

    msgs[0].addr = dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &cmd;

    msgs[1].addr = dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 2;
    msgs[1].buf = buf;

    if(i2c_transfer(dev->i2c, msgs, 2) != 2)
    	return FALSE;
    if(val)
    	*val = (buf[0] << 4) | (buf[1] >> 4);
    return TRUE;
}

static int ns2009_timer_function(struct timer_t * timer, void * data)
{
	struct input_t * input = (struct input_t *)(data);
	struct ts_ns2009_pdata_t * pdat = (struct ts_ns2009_pdata_t *)input->priv;
	int x = 0, y = 0, z1 = 0;

	if(ns2009_read(pdat->dev, NS2009_LOW_POWER_READ_Z1, &z1))
	{
		if(z1 > 80)
		{
			ns2009_read(pdat->dev, NS2009_LOW_POWER_READ_X, &x);
			ns2009_read(pdat->dev, NS2009_LOW_POWER_READ_Y, &y);
			tsfilter_update(pdat->filter, &x, &y);

			if(!pdat->press)
			{
				push_event_touch_begin(input, x, y, 0);
				pdat->press = 1;
			}
			else
			{
				if((pdat->x != x) || (pdat->y != y))
					push_event_touch_move(input, x, y, 0);
			}
			pdat->x = x;
			pdat->y = y;
		}
		else
		{
			if(pdat->press)
			{
				tsfilter_clear(pdat->filter);
				push_event_touch_end(input, pdat->x, pdat->y, 0);
				pdat->press = 0;
			}
		}
	}

	timer_forward_now(timer, ms_to_ktime(pdat->interval));
	return 1;
}

static int ts_ns2009_ioctl(struct input_t * input, int cmd, void * arg)
{
	struct ts_ns2009_pdata_t * pdat = (struct ts_ns2009_pdata_t *)input->priv;

	switch(cmd)
	{
	case INPUT_IOCTL_TOUCHSCEEN_SET_CALIBRATION:
		if(arg)
		{
			tsfilter_setcal(pdat->filter, (int *)arg);
			return 0;
		}
		break;
	case INPUT_IOCTL_TOUCHSCEEN_GET_CALIBRATION:
		if(arg)
		{
			memcpy(arg, pdat->filter->cal, sizeof(int) * 7);
			return 0;
		}
		break;
	default:
		break;
	}
	return -1;
}

static struct device_t * ts_ns2009_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ts_ns2009_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	int cal[7] = {1, 0, 0, 0, 1, 0, 1};
	int i;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x48), 0);
	if(!i2cdev)
		return NULL;

	if(!ns2009_read(i2cdev, NS2009_LOW_POWER_READ_Z1, NULL))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct ts_ns2009_pdata_t));
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

	timer_init(&pdat->timer, ns2009_timer_function, input);
	pdat->dev = i2cdev;
	pdat->filter = tsfilter_alloc(dt_read_int(n, "median-filter-length", 5), dt_read_int(n, "mean-filter-length", 5));
	if(dt_read_array_length(n, "calibration") == 7)
	{
		for(i = 0; i < 7; i++)
			cal[i] = dt_read_array_int(n, "calibration", i, cal[i]);
		tsfilter_setcal(pdat->filter, &cal[0]);
	}
	pdat->interval = dt_read_int(n, "poll-interval-ms", 10);
	pdat->x = 0;
	pdat->y = 0;
	pdat->press = 0;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->ioctl = ts_ns2009_ioctl;
	input->priv = pdat;
	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));

	if(!register_input(&dev, input))
	{
		tsfilter_free(pdat->filter);
		timer_cancel(&pdat->timer);
		i2c_device_free(pdat->dev);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ts_ns2009_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct ts_ns2009_pdata_t * pdat = (struct ts_ns2009_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		tsfilter_free(pdat->filter);
		timer_cancel(&pdat->timer);
		i2c_device_free(pdat->dev);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void ts_ns2009_suspend(struct device_t * dev)
{
}

static void ts_ns2009_resume(struct device_t * dev)
{
}

static struct driver_t ts_ns2009 = {
	.name		= "ts-ns2009",
	.probe		= ts_ns2009_probe,
	.remove		= ts_ns2009_remove,
	.suspend	= ts_ns2009_suspend,
	.resume		= ts_ns2009_resume,
};

static __init void ts_ns2009_driver_init(void)
{
	register_driver(&ts_ns2009);
}

static __exit void ts_ns2009_driver_exit(void)
{
	unregister_driver(&ts_ns2009);
}

driver_initcall(ts_ns2009_driver_init);
driver_exitcall(ts_ns2009_driver_exit);
