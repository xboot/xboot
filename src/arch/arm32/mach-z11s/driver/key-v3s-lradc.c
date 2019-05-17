/*
 * driver/key-v3s-lradc.c
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
#include <interrupt/interrupt.h>
#include <input/input.h>
#include <input/keyboard.h>

enum {
	LRADC_CTRL	= 0x00,
	LRADC_INTC	= 0x04,
	LRADC_INTS	= 0x08,
	LRADC_DATA0	= 0x0c,
};

/* LRADC_CTRL bits */
#define FIRST_CONVERT_DLY(x)	((x) << 24) /* 8 bits */
#define CHAN_SELECT(x)			((x) << 22) /* 2 bits */
#define CONTINUE_TIME_SEL(x)	((x) << 16) /* 4 bits */
#define KEY_MODE_SEL(x)			((x) << 12) /* 2 bits */
#define LEVELA_B_CNT(x)			((x) << 8)  /* 4 bits */
#define HOLD_EN(x)				((x) << 6)
#define LEVELB_VOL(x)			((x) << 4)  /* 2 bits */
#define SAMPLE_RATE(x)			((x) << 2)  /* 2 bits */
#define ENABLE(x)				((x) << 0)

/* LRADC_INTC and LRADC_INTS bits */
#define CHAN1_KEYUP_IRQ			(1 << 12)
#define CHAN1_ALRDY_HOLD_IRQ	(1 << 11)
#define CHAN1_HOLD_IRQ			(1 << 10)
#define	CHAN1_KEYDOWN_IRQ		(1 << 9)
#define CHAN1_DATA_IRQ			(1 << 8)
#define CHAN0_KEYUP_IRQ			(1 << 4)
#define CHAN0_ALRDY_HOLD_IRQ	(1 << 3)
#define CHAN0_HOLD_IRQ			(1 << 2)
#define	CHAN0_KEYDOWN_IRQ		(1 << 1)
#define CHAN0_DATA_IRQ			(1 << 0)

struct adc_key_t {
	int min;
	int max;
	int keycode;
};

struct key_v3s_lradc_pdata_t {
	virtual_addr_t virt;
	int irq;
	int vref;
	struct adc_key_t * keys;
	int nkeys;
	int keycode;
};

static int key_v3s_lradc_get_keycode(struct key_v3s_lradc_pdata_t * pdat, int voltage)
{
	int i;

	for(i = 0; i < pdat->nkeys; i++)
	{
		if((voltage >= pdat->keys[i].min) && (voltage < pdat->keys[i].max))
			return pdat->keys[i].keycode;
	}
	return 0;
}

static void key_v3s_lradc_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct key_v3s_lradc_pdata_t * pdat = (struct key_v3s_lradc_pdata_t *)input->priv;
	u32_t ints, val;
	int voltage, keycode;

	ints  = read32(pdat->virt + LRADC_INTS);
	if(ints & CHAN0_KEYUP_IRQ)
	{
		if(pdat->keycode != 0)
			push_event_key_up(input, pdat->keycode);
		pdat->keycode = 0;
	}
	if((ints & CHAN0_KEYDOWN_IRQ) && (pdat->keycode == 0))
	{
		val = read32(pdat->virt + LRADC_DATA0) & 0x3f;
		voltage = val * pdat->vref / 63;
		keycode = key_v3s_lradc_get_keycode(pdat, voltage);
		if(keycode != 0)
		{
			pdat->keycode = keycode;
			push_event_key_down(input, pdat->keycode);
		}
	}
	write32(pdat->virt + LRADC_INTS, ints);
}

static int key_v3s_lradc_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * key_v3s_lradc_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct key_v3s_lradc_pdata_t * pdat;
	struct adc_key_t * keys;
	struct input_t * input;
	struct device_t * dev;
	struct dtnode_t o;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int irq = dt_read_int(n, "interrupt", -1);
	int nkeys, i;

	if(!irq_is_valid(irq))
		return NULL;

	if((nkeys = dt_read_array_length(n, "keys")) <= 0)
		return NULL;

	pdat = malloc(sizeof(struct key_v3s_lradc_pdata_t));
	if(!pdat)
		return NULL;

	keys = malloc(sizeof(struct adc_key_t) * nkeys);
	if(!keys)
	{
		free(pdat);
		return NULL;
	}

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		free(keys);
		return NULL;
	}

	for(i = 0; i < nkeys; i++)
	{
		dt_read_array_object(n, "keys", i, &o);
		keys[i].min = dt_read_int(&o, "min-voltage", 0);
		keys[i].max = dt_read_int(&o, "max-voltage", 0);
		keys[i].keycode = dt_read_int(&o, "key-code", 0);
	}

	pdat->virt = virt;
	pdat->irq = irq;
	pdat->vref = dt_read_int(n, "reference-voltage", 3000000) * 2 / 3;
	pdat->keys = keys;
	pdat->nkeys = nkeys;
	pdat->keycode = 0;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->ioctl = key_v3s_lradc_ioctl;
	input->priv = pdat;

	request_irq(pdat->irq, key_v3s_lradc_interrupt, IRQ_TYPE_NONE, input);

	/*
	 * Set sample time to 4 ms / 250 Hz. Wait 2 * 4 ms for key to
	 * stabilize on press, wait (1 + 1) * 4 ms for key release
	 */
	write32(pdat->virt + LRADC_CTRL, FIRST_CONVERT_DLY(2) | LEVELA_B_CNT(1) | HOLD_EN(1) | SAMPLE_RATE(0) | ENABLE(1));
	write32(pdat->virt + LRADC_INTC, CHAN0_KEYUP_IRQ | CHAN0_KEYDOWN_IRQ);

	if(!register_input(&dev, input))
	{
		write32(pdat->virt + LRADC_CTRL, 0);
		write32(pdat->virt + LRADC_INTC, 0);
		free_irq(pdat->irq);
		free(pdat->keys);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void key_v3s_lradc_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_v3s_lradc_pdata_t * pdat = (struct key_v3s_lradc_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		write32(pdat->virt + LRADC_CTRL, 0);
		write32(pdat->virt + LRADC_INTC, 0);
		free_irq(pdat->irq);
		free(pdat->keys);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void key_v3s_lradc_suspend(struct device_t * dev)
{
}

static void key_v3s_lradc_resume(struct device_t * dev)
{
}

static struct driver_t key_v3s_lradc = {
	.name		= "key-v3s-lradc",
	.probe		= key_v3s_lradc_probe,
	.remove		= key_v3s_lradc_remove,
	.suspend	= key_v3s_lradc_suspend,
	.resume		= key_v3s_lradc_resume,
};

static __init void key_v3s_lradc_driver_init(void)
{
	register_driver(&key_v3s_lradc);
}

static __exit void key_v3s_lradc_driver_exit(void)
{
	unregister_driver(&key_v3s_lradc);
}

driver_initcall(key_v3s_lradc_driver_init);
driver_exitcall(key_v3s_lradc_driver_exit);
