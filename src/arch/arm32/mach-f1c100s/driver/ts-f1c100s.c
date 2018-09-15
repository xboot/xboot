/*
 * driver/ts-f1c100s.c
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
#include <gpio/gpio.h>
#include <interrupt/interrupt.h>
#include <input/input.h>
#include <tsfilter.h>
#include <f1c100s/reg-tp.h>
#include <f1c100s-gpio.h>

#define ENABLE_DEBUG	(1)
#define USE_SOFT_FILTER	(0)

#if ENABLE_DEBUG
#define DEBUG_PRINT	printf
#else
#define DEBUG_PRINT(...)
#endif

struct ts_f1c100s_pdata_t {
	struct tsfilter_t * filter;
	int x, y;
	int press;
	int cal[7];
	int irq;
	int ignore_fifo_data;
};


static int ts_f1c100s_init(void)
{
	u32_t val;

	/* Config GPIOA0 as  TPX1 TPY1 TP X2 TPY2*/
	gpio_set_cfg(F1C100S_GPIOA0, 0x02);
	gpio_set_cfg(F1C100S_GPIOA1, 0x02);
	gpio_set_cfg(F1C100S_GPIOA2, 0x02);
	gpio_set_cfg(F1C100S_GPIOA3, 0x02);
	
	val = read32(TP_BASE_ADDR+TP_CTRL0);
	/*FRAME Freq = 24M / 6 = 4Mhz(CLK_IN)/ 2^13(8192) = 488 hz*/
	/*Conversion Time = 1 / (4MHz/13Cycles) = 3.25us*/
	/*T_ACQ  = CLK_IN /(16*(1+63)) = 3906.25hz*/
	write32(TP_BASE_ADDR+TP_CTRL0, (0x1f << 23)|ADC_CLK_SEL(0) | ADC_CLK_DIV(2) | FS_DIV(8) | T_ACQ(63));
	
	/*00: FIFO store X1,Y1 data for single touch no pressure mode*/
	val = read32(TP_BASE_ADDR+TP_CTRL2);
	val = (0x08 << 28) | TP_MODE_SELECT(0) | (0 << 24)| 0xFFF;
	write32(TP_BASE_ADDR+TP_CTRL2,	val);
	
	val = read32(TP_BASE_ADDR+TP_CTRL3);//Enable HW filter
	write32(TP_BASE_ADDR + TP_CTRL3, FILTER_EN(1) | FILTER_TYPE(1) | val);
	
	write32(TP_BASE_ADDR + TP_TPR, TEMP_ENABLE(1) | TEMP_PERIOD(1953));//Temperature convertion

	write32( TP_BASE_ADDR + TP_CDAT, 0xc00);

	val = read32(TP_BASE_ADDR+TP_CTRL1);
	val = (5<<12) | (1<< 9) | (0<<8) | (1 << 7) | (1 << 5) | TP_MODE_EN(0) | 0x00;	//4 channel enable
	write32( TP_BASE_ADDR + TP_CTRL1, val);
	
	write32( TP_BASE_ADDR + 0x38, 0x22);	
	
	write32(TP_BASE_ADDR + TP_INT_FIFOC , TEMP_IRQ_EN(0) | OVERRUN_IRQ_EN(0) | DATA_IRQ_EN(1) | FIFO_TRIG(1) | TP_DATA_XY_CHANGE(0)| FIFO_FLUSH(1) | TP_UP_IRQ_EN(1) | TP_DOWN_IRQ_EN(1));

	return 0;
}

static void ts_f1c100s_xy_correction(struct ts_f1c100s_pdata_t * pdat, int *x, int *y)
{
	*x = (pdat->cal[2] + pdat->cal[0] * *x + pdat->cal[1] * *y) / pdat->cal[6];
	*y = (pdat->cal[5] + pdat->cal[3] * *x + pdat->cal[4] * *y) / pdat->cal[6];
}

static void ts_f1c100s_irq_handler(void * data)
{
	struct input_t * input = (struct input_t *)(data);
	struct ts_f1c100s_pdata_t * pdat = (struct ts_f1c100s_pdata_t *)input->priv;
	int x = 0, y = 0;

	u32_t val = read32(TP_BASE_ADDR + TP_INT_FIFOS);
	if (val & TP_DOWN_PENDING)//Down
	{
		if(!pdat->press)
		{
			DEBUG_PRINT("\r\nDn");
		}
	}
	
	if (val & TP_UP_PENDING)//Up
	{
		pdat->press = 0;
		pdat->ignore_fifo_data = 1;
		#if !USE_SOFT_FILTER
		tsfilter_clear(pdat->filter);
		#endif
		push_event_touch_end(input, pdat->x, pdat->y, 0);
		DEBUG_PRINT("\r\nUp:x:%d,y:%d",pdat->x, pdat->y);
	}

	if (val & FIFO_DATA_PENDING) //fifo data irq
	{
		y = read32(TP_BASE_ADDR + TP_DATA);
		x = read32(TP_BASE_ADDR + TP_DATA);
		/* The 1st location reported after an up event is unreliable */
		if (!pdat->ignore_fifo_data) 
		{
			#if !USE_SOFT_FILTER
			ts_f1c100s_xy_correction(pdat, &x, &y);
			#else
			tsfilter_update(pdat->filter, &x, &y);
			#endif

			if (!pdat->press)
			{
				push_event_touch_begin(input, x, y, 0);
				pdat->press = 1;
				DEBUG_PRINT("\r\nDn2:x:%d,y:%d", x, y);
			}
			else
			{
				if ((pdat->x != x) || (pdat->y != y))
				{
					push_event_touch_move(input, x, y, 0);
					DEBUG_PRINT("\r\nMv:x:%d,y:%d", x, y);
				}
			}
			pdat->x = x;
			pdat->y = y;
		}
		else
		{
			pdat->ignore_fifo_data = 0;
		}
	}

	// if (val & (1 << 18))//Temperature
	// {
	// 		temperature = read32(TP_BASE_ADDR+0x20);
	// }

	write32(TP_BASE_ADDR+TP_INT_FIFOS, val);
}

static int ts_f1c100s_ioctl(struct input_t * input, int cmd, void * arg)
{
	struct ts_f1c100s_pdata_t * pdat = (struct ts_f1c100s_pdata_t *)input->priv;
	int cal[7];

	if(cmd == INPUT_IOCTL_TOUCHSCEEN_SET_CALIBRATION)
	{
		if(!arg)
			return -1;
#if USE_SOFT_FILTER
		memcpy(cal, arg, sizeof(int) * 7);
		tsfilter_setcal(pdat->filter, cal);
#endif
		memcpy(pdat->cal, arg, sizeof(int) * 7);
		return 0;
	}
	return -1;
}

static struct device_t * ts_f1c100s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ts_f1c100s_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	int cal[7] = {1, 0, 0, 0, 1, 0, 1};

	pdat = malloc(sizeof(struct ts_f1c100s_pdata_t));
	if(!pdat)
	{
		return NULL;
	}
	memcpy(pdat->cal, cal, sizeof(int) * 7);

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return NULL;
	}

	ts_f1c100s_init();

	pdat->irq = dt_read_int(n, "irq", 20);
	if(!request_irq(pdat->irq, ts_f1c100s_irq_handler, IRQ_TYPE_LEVEL_LOW, input))
	{
		return NULL;
	}
	enable_irq(pdat->irq);
	
	if(dt_read_array_length(n, "calibration") == 7)
	{
		int i;
		for(i = 0; i < 7; i++)
			pdat->cal[i] = dt_read_array_int(n, "calibration", i, cal[i]);
	}

	pdat->ignore_fifo_data = 1;

	#if USE_SOFT_FILTER
	pdat->filter = tsfilter_alloc(dt_read_int(n, "median-filter-length", 5), dt_read_int(n, "mean-filter-length", 5));
	tsfilter_setcal(pdat->filter, &(pdat->cal[0]));
	#endif
	
	pdat->x = 0;
	pdat->y = 0;
	pdat->press = 0;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->type = INPUT_TYPE_TOUCHSCREEN;
	input->ioctl = ts_f1c100s_ioctl;
	input->priv = pdat;

	if(!register_input(&dev, input))
	{
		#if USE_SOFT_FILTER
		tsfilter_free(pdat->filter);
		#endif
		disable_irq(pdat->irq);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ts_f1c100s_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct ts_f1c100s_pdata_t * pdat = (struct ts_f1c100s_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		#if USE_SOFT_FILTER
		tsfilter_free(pdat->filter);
		#endif

		disable_irq(pdat->irq);
		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void ts_f1c100s_suspend(struct device_t * dev)
{
}

static void ts_f1c100s_resume(struct device_t * dev)
{
}

static struct driver_t ts_f1c100s = {
	.name		= "ts-f1c100s",
	.probe		= ts_f1c100s_probe,
	.remove		= ts_f1c100s_remove,
	.suspend	= ts_f1c100s_suspend,
	.resume		= ts_f1c100s_resume,
};

static __init void ts_f1c100s_driver_init(void)
{
	register_driver(&ts_f1c100s);
}

static __exit void ts_f1c100s_driver_exit(void)
{
	unregister_driver(&ts_f1c100s);
}

driver_initcall(ts_f1c100s_driver_init);
driver_exitcall(ts_f1c100s_driver_exit);
