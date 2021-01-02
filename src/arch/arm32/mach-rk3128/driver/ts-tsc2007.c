/*
 * driver/ts-tsc2007.c
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
#include <tsfilter.h>

#define TSC2007_MEASURE_TEMP0		(0x0 << 4)
#define TSC2007_MEASURE_AUX			(0x2 << 4)
#define TSC2007_MEASURE_TEMP1		(0x4 << 4)
#define TSC2007_ACTIVATE_XN			(0x8 << 4)
#define TSC2007_ACTIVATE_YN			(0x9 << 4)
#define TSC2007_ACTIVATE_YP_XN		(0xa << 4)
#define TSC2007_SETUP				(0xb << 4)
#define TSC2007_MEASURE_X			(0xc << 4)
#define TSC2007_MEASURE_Y			(0xd << 4)
#define TSC2007_MEASURE_Z1			(0xe << 4)
#define TSC2007_MEASURE_Z2			(0xf << 4)

#define TSC2007_POWER_OFF_IRQ_EN	(0x0 << 2)
#define TSC2007_ADC_ON_IRQ_DIS0		(0x1 << 2)
#define TSC2007_ADC_OFF_IRQ_EN		(0x2 << 2)
#define TSC2007_ADC_ON_IRQ_DIS1		(0x3 << 2)

#define TSC2007_12BIT				(0x0 << 1)
#define TSC2007_8BIT				(0x1 << 1)
#define MAX_12BIT					((1 << 12) - 1)
#define ADC_ON_12BIT				(TSC2007_12BIT | TSC2007_ADC_ON_IRQ_DIS0)

#define READ_X						(ADC_ON_12BIT | TSC2007_MEASURE_X)
#define READ_Y						(ADC_ON_12BIT | TSC2007_MEASURE_Y)
#define READ_Z1						(ADC_ON_12BIT | TSC2007_MEASURE_Z1)
#define READ_Z2						(ADC_ON_12BIT | TSC2007_MEASURE_Z2)
#define PWRDOWN						(TSC2007_12BIT | TSC2007_POWER_OFF_IRQ_EN)

struct ts_tsc2007_pdata_t {
	struct timer_t timer;
	struct i2c_device_t * dev;
	struct tsfilter_t * filter;
	int irq;
	int interval;
	int x_plate_ohms;
	int max_rt;
	int x, y;
	int press;
};

static bool_t tsc2007_read(struct i2c_device_t * dev, u8_t cmd, int * val)
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

static void tsc2007_read_values(struct ts_tsc2007_pdata_t * pdat, int * x, int * y, int * z1, int * z2)
{
	tsc2007_read(pdat->dev, READ_Y, y);
	tsc2007_read(pdat->dev, READ_X, x);
	tsc2007_read(pdat->dev, READ_Z1, z1);
	tsc2007_read(pdat->dev, READ_Z2, z2);
	tsc2007_read(pdat->dev, PWRDOWN, NULL);
}

static u32_t tsc2007_calculate_pressure(struct ts_tsc2007_pdata_t * pdat, int x, int y, int z1, int z2)
{
	u32_t rt = 0;

	if(x >= MAX_12BIT)
		x = 0;

	if((x > 0) && (z1 > 0))
	{
		rt = z2 - z1;
		rt *= x;
		rt *= pdat->x_plate_ohms;
		rt /= z1;
		rt = (rt + 2047) >> 12;
	}
	return rt;
}

static int tsc2007_timer_function(struct timer_t * timer, void * data)
{
	struct input_t * input = (struct input_t *)(data);
	struct ts_tsc2007_pdata_t * pdat = (struct ts_tsc2007_pdata_t *)input->priv;
	int x = 0, y = 0, z1 = 0, z2 = 0;
	u32_t rt;

	tsc2007_read_values(pdat, &x, &y, &z1, &z2);
	rt = tsc2007_calculate_pressure(pdat, x, y, z1, z2);
	if(rt > pdat->max_rt)
	{
		timer_forward_now(timer, ms_to_ktime(pdat->interval));
		return 1;
	}
	else if(rt > 0)
	{
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
		timer_forward_now(timer, ms_to_ktime(pdat->interval));
		return 1;
	}
	else
	{
		if(pdat->press)
		{
			push_event_touch_end(input, pdat->x, pdat->y, 0);
			pdat->press = 0;
		}
	}
	enable_irq(pdat->irq);
	return 0;
}

static void tsc2007_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct ts_tsc2007_pdata_t * pdat = (struct ts_tsc2007_pdata_t *)input->priv;

	disable_irq(pdat->irq);
	timer_start_now(&pdat->timer, ms_to_ktime(1));
}

static int ts_tsc2007_ioctl(struct input_t * input, const char * cmd, void * arg)
{
	struct ts_tsc2007_pdata_t * pdat = (struct ts_tsc2007_pdata_t *)input->priv;

	switch(shash(cmd))
	{
	case 0x50460f76: /* "touchscreen-set-calibration" */
		if(arg)
		{
			tsfilter_setcal(pdat->filter, (int *)arg);
			return 0;
		}
		break;
	case 0xa8ecea6a: /* "touchscreen-get-calibration" */
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

static struct device_t * ts_tsc2007_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ts_tsc2007_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	int gpio = dt_read_int(n, "interrupt-gpio", -1);
	int gpiocfg = dt_read_int(n, "interrupt-gpio-config", -1);
	int irq = gpio_to_irq(gpio);
	int cal[7] = {1, 0, 0, 0, 1, 0, 1};
	int i;

	if(!gpio_is_valid(gpio) || !irq_is_valid(irq))
		return NULL;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x48), 0);
	if(!i2cdev)
		return NULL;

	if(!tsc2007_read(i2cdev, PWRDOWN, NULL))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct ts_tsc2007_pdata_t));
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

	timer_init(&pdat->timer, tsc2007_timer_function, input);
	pdat->dev = i2cdev;
	pdat->filter = tsfilter_alloc(dt_read_int(n, "median-filter-length", 5), dt_read_int(n, "mean-filter-length", 5));
	if(dt_read_array_length(n, "calibration") == 7)
	{
		for(i = 0; i < 7; i++)
			cal[i] = dt_read_array_int(n, "calibration", i, cal[i]);
		tsfilter_setcal(pdat->filter, &cal[0]);
	}
	pdat->irq = irq;
	pdat->interval = dt_read_int(n, "poll-interval-ms", 10);
	pdat->x_plate_ohms = dt_read_int(n, "x-plate-ohms", 600);
	pdat->max_rt = dt_read_int(n, "max-rt", MAX_12BIT);
	pdat->x = 0;
	pdat->y = 0;
	pdat->press = 0;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->ioctl = ts_tsc2007_ioctl;
	input->priv = pdat;

	if(gpio >= 0)
	{
		if(gpiocfg >= 0)
			gpio_set_cfg(gpio, gpiocfg);
		gpio_set_pull(gpio, GPIO_PULL_UP);
		gpio_set_direction(gpio, GPIO_DIRECTION_INPUT);
	}
	if(!request_irq(pdat->irq, tsc2007_interrupt, IRQ_TYPE_EDGE_FALLING, input))
	{
		tsfilter_free(pdat->filter);
		free_irq(pdat->irq);
		timer_cancel(&pdat->timer);
		i2c_device_free(pdat->dev);
		return NULL;
	}

	if(!(dev = register_input(input, drv)))
	{
		tsfilter_free(pdat->filter);
		free_irq(pdat->irq);
		timer_cancel(&pdat->timer);
		i2c_device_free(pdat->dev);
		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	return dev;
}

static void ts_tsc2007_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct ts_tsc2007_pdata_t * pdat = (struct ts_tsc2007_pdata_t *)input->priv;

	if(input)
	{
		unregister_input(input);
		tsfilter_free(pdat->filter);
		free_irq(pdat->irq);
		timer_cancel(&pdat->timer);
		i2c_device_free(pdat->dev);
		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void ts_tsc2007_suspend(struct device_t * dev)
{
}

static void ts_tsc2007_resume(struct device_t * dev)
{
}

static struct driver_t ts_tsc2007 = {
	.name		= "ts-tsc2007",
	.probe		= ts_tsc2007_probe,
	.remove		= ts_tsc2007_remove,
	.suspend	= ts_tsc2007_suspend,
	.resume		= ts_tsc2007_resume,
};

static __init void ts_tsc2007_driver_init(void)
{
	register_driver(&ts_tsc2007);
}

static __exit void ts_tsc2007_driver_exit(void)
{
	unregister_driver(&ts_tsc2007);
}

driver_initcall(ts_tsc2007_driver_init);
driver_exitcall(ts_tsc2007_driver_exit);
