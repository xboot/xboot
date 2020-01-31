/*
 * driver/ts-f1c100s.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <reset/reset.h>
#include <gpio/gpio.h>
#include <interrupt/interrupt.h>
#include <input/input.h>
#include <tsfilter.h>

/* TP_CTRL0 bits */
#define ADC_FIRST_DLY(x)		((x) << 24)	/* 8 bits */
#define ADC_FIRST_DLY_MODE(x)	((x) << 23)
#define ADC_CLK_SEL(x)			((x) << 22)
#define ADC_CLK_DIV(x)			((x) << 20)	/* 3 bits */
#define FS_DIV(x)				((x) << 16)	/* 4 bits */
#define T_ACQ(x)				((x) << 0)	/* 16 bits */

/* TP_CTRL1 bits */
#define STYLUS_UP_DEBOUN(x)		((x) << 12)	/* 8 bits */
#define STYLUS_UP_DEBOUN_EN(x)	((x) << 9)
#define TOUCH_PAN_CALI_EN(x)	((x) << 6)
#define TP_DUAL_EN(x)			((x) << 5)
#define TP_MODE_EN(x)			((x) << 4)
#define TP_ADC_SELECT(x)		((x) << 3)
#define ADC_CHAN_SELECT(x)		((x) << 0)	/* 3 bits */

/* on sun6i, bits 3~6 are left shifted by 1 to 4~7 */
#define SUN6I_TP_MODE_EN(x)		((x) << 5)

/* TP_CTRL2 bits */
#define TP_SENSITIVE_ADJUST(x)	((x) << 28)	/* 4 bits */
#define TP_MODE_SELECT(x)		((x) << 26)	/* 2 bits */
#define PRE_MEA_EN(x)			((x) << 24)
#define PRE_MEA_THRE_CNT(x)		((x) << 0) 	/* 24 bits */

/* TP_CTRL3 bits */
#define FILTER_EN(x)			((x) << 2)
#define FILTER_TYPE(x)			((x) << 0)	/* 2 bits */

/* TP_INT_FIFOC irq and fifo mask / control bits */
#define TEMP_IRQ_EN(x)			((x) << 18)
#define OVERRUN_IRQ_EN(x)		((x) << 17)
#define DATA_IRQ_EN(x)			((x) << 16)
#define TP_DATA_XY_CHANGE(x)	((x) << 13)
#define FIFO_TRIG(x)			((x) << 8)	/* 5 bits */
#define DATA_DRQ_EN(x)			((x) << 7)
#define FIFO_FLUSH(x)			((x) << 4)
#define TP_UP_IRQ_EN(x)			((x) << 1)
#define TP_DOWN_IRQ_EN(x)		((x) << 0)

/* TP_INT_FIFOS irq and fifo status bits */
#define TEMP_DATA_PENDING		(1 << 18)
#define FIFO_OVERRUN_PENDING	(1 << 17)
#define FIFO_DATA_PENDING		(1 << 16)
#define TP_IDLE_FLG				(1 << 2)
#define TP_UP_PENDING			(1 << 1)
#define TP_DOWN_PENDING			(1 << 0)

/* TP_TPR bits */
#define TEMP_ENABLE(x)			((x) << 16)
#define TEMP_PERIOD(x)			((x) << 0)	/* t = x * 256 * 16 / clkin */

enum {
	TP_CTRL0 		= 0x00,
	TP_CTRL1 		= 0x04,
	TP_CTRL2 		= 0x08,
	TP_CTRL3 		= 0x0c,
	TP_INT_FIFOC 	= 0x10,
	TP_INT_FIFOS 	= 0x14,
	TP_TPR 			= 0x18,
	TP_CDAT 		= 0x1c,
	TEMP_DATA 		= 0x20,
	TP_DATA 		= 0x24,
};

struct ts_f1c100s_pdata_t {
	virtual_addr_t virt;
	struct tsfilter_t * filter;
	char * clk;
	int irq;
	int reset;
	int x1;
	int x1cfg;
	int y1;
	int y1cfg;
	int x2;
	int x2cfg;
	int y2;
	int y2cfg;

	int x, y;
	int press;
	int ignore_fifo_data;
};

static void ts_f1c100s_init(struct ts_f1c100s_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt + TP_CTRL0);
	write32(pdat->virt + TP_CTRL0, (0x1f << 23) | ADC_CLK_SEL(0) | ADC_CLK_DIV(2) | FS_DIV(6) | T_ACQ(63));
	
	val = read32(pdat->virt + TP_CTRL2);
	val = (0x08 << 28) | TP_MODE_SELECT(0) | (0 << 24) | 0xFFF;
	write32(pdat->virt+TP_CTRL2, val);

	val = read32(pdat->virt + TP_CTRL3);
	write32(pdat->virt + TP_CTRL3, FILTER_EN(0) | FILTER_TYPE(1) | val);
	write32(pdat->virt + TP_TPR, TEMP_ENABLE(1) | TEMP_PERIOD(1953));
	write32( pdat->virt + TP_CDAT, 0xc00);

	val = read32(pdat->virt + TP_CTRL1);
	val = (5 << 12) | (1 << 9) | (0 << 8) | (1 << 7) | (1 << 5) | TP_MODE_EN(0) | 0x00;
	write32(pdat->virt + TP_CTRL1, val);
	write32(pdat->virt + 0x38, 0x22);
	write32(pdat->virt + TP_INT_FIFOC, TEMP_IRQ_EN(0) | OVERRUN_IRQ_EN(0) | DATA_IRQ_EN(1) | FIFO_TRIG(1) | TP_DATA_XY_CHANGE(0)| FIFO_FLUSH(1) | TP_UP_IRQ_EN(1) | TP_DOWN_IRQ_EN(1));
}

static void ts_f1c100s_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)(data);
	struct ts_f1c100s_pdata_t * pdat = (struct ts_f1c100s_pdata_t *)input->priv;
	u32_t val;
	int x = 0, y = 0;

	val = read32(pdat->virt + TP_INT_FIFOS);
	if(val & TP_DOWN_PENDING)
	{
		if(!pdat->press)
		{
		}
	}
	if(val & TP_UP_PENDING)
	{
		pdat->press = 0;
		pdat->ignore_fifo_data = 1;
		tsfilter_clear(pdat->filter);
		push_event_touch_end(input, pdat->x, pdat->y, 0);
	}
	if(val & FIFO_DATA_PENDING)
	{
		y = read32(pdat->virt + TP_DATA);
		x = read32(pdat->virt + TP_DATA);

		if(!pdat->ignore_fifo_data)
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
				{
					push_event_touch_move(input, x, y, 0);
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
	write32(pdat->virt + TP_INT_FIFOS, val);
}

static int ts_f1c100s_ioctl(struct input_t * input, const char * cmd, void * arg)
{
	struct ts_f1c100s_pdata_t * pdat = (struct ts_f1c100s_pdata_t *)input->priv;

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

static struct device_t * ts_f1c100s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ts_f1c100s_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);
	int cal[7] = {1, 0, 0, 0, 1, 0, 1};
	int i;

	if(!irq_is_valid(irq))
		return NULL;

	pdat = malloc(sizeof(struct ts_f1c100s_pdata_t));
	if(!pdat)
		return NULL;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->filter = tsfilter_alloc(dt_read_int(n, "median-filter-length", 5), dt_read_int(n, "mean-filter-length", 5));
	if(dt_read_array_length(n, "calibration") == 7)
	{
		for(i = 0; i < 7; i++)
			cal[i] = dt_read_array_int(n, "calibration", i, cal[i]);
		tsfilter_setcal(pdat->filter, &cal[0]);
	}
	pdat->clk = strdup(clk);
	pdat->irq = irq;
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->x1 = dt_read_int(n, "x1-gpio", -1);
	pdat->x1cfg = dt_read_int(n, "x1-gpio-config", -1);
	pdat->x2 = dt_read_int(n, "x2-gpio", -1);
	pdat->x2cfg = dt_read_int(n, "x2-gpio-config", -1);
	pdat->y1 = dt_read_int(n, "y1-gpio", -1);
	pdat->y1cfg = dt_read_int(n, "y1-gpio-config", -1);
	pdat->y2 = dt_read_int(n, "y2-gpio", -1);
	pdat->y2cfg = dt_read_int(n, "y2-gpio-config", -1);
	pdat->x = 0;
	pdat->y = 0;
	pdat->press = 0;
	pdat->ignore_fifo_data = 1;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->ioctl = ts_f1c100s_ioctl;
	input->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	if(pdat->x1 >= 0)
	{
		if(pdat->x1cfg >= 0)
			gpio_set_cfg(pdat->x1, pdat->x1cfg);
		gpio_set_pull(pdat->x1, GPIO_PULL_UP);
	}
	if(pdat->x2 >= 0)
	{
		if(pdat->x2cfg >= 0)
			gpio_set_cfg(pdat->x2, pdat->x2cfg);
		gpio_set_pull(pdat->x2, GPIO_PULL_UP);
	}
	if(pdat->y1 >= 0)
	{
		if(pdat->y1cfg >= 0)
			gpio_set_cfg(pdat->y1, pdat->y1cfg);
		gpio_set_pull(pdat->y1, GPIO_PULL_UP);
	}
	if(pdat->y2 >= 0)
	{
		if(pdat->y2cfg >= 0)
			gpio_set_cfg(pdat->y2, pdat->y2cfg);
		gpio_set_pull(pdat->y2, GPIO_PULL_UP);
	}

	request_irq(pdat->irq, ts_f1c100s_interrupt, IRQ_TYPE_LEVEL_LOW, input);
	ts_f1c100s_init(pdat);

	if(!(dev = register_input(input, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_irq(pdat->irq);
		tsfilter_free(pdat->filter);
		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	return dev;
}

static void ts_f1c100s_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct ts_f1c100s_pdata_t * pdat = (struct ts_f1c100s_pdata_t *)input->priv;

	if(input)
	{
		unregister_input(input);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_irq(pdat->irq);
		tsfilter_free(pdat->filter);
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
