/*
 * driver/ps2key-rk3288.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <i2c/i2c.h>
#include <gpio/gpio.h>
#include <clk/clk.h>
#include <interrupt/interrupt.h>
#include <input/input.h>
#include <input/keyboard.h>

enum {
	PS2C_CTRL	= 0x00,
	PS2C_RBR	= 0x04,
	PS2C_TBR	= 0x08,
	PS2C_STAT	= 0x0c,
	PS2C_IER	= 0x10,
	PS2C_ICR	= 0x14,
	PS2C_ISR	= 0x18,
	PS2C_TRR1	= 0x1c,
	PS2C_TRR2	= 0x20,
	PS2C_TRR3	= 0x24,
	PS2C_RTR	= 0x28,
	PS2C_WTR	= 0x2c,
	PS2C_FLT	= 0x30,
};

/* ps2c sending timeout enable */
#define TX_TIMEOUT_EN		(1 << 4)

/* ps2c receiving timeout enable */
#define RX_TIMEOUT_EN		(1 << 3)

/* PS2C mode */
#define PS2_MODE_MASK		((1 << 2) | (1 << 1))

#define PS2_MODE_SHIFT		1

/* ps2c enable */
#define PS2C_CTRL_EN		(1 << 0)

/* enable ps2c sending timeout interrupt */
#define TX_TIMEOUT_INT_EN	(1 << 3)

/* enable ps2c finish sending one byte interrupt */
#define FSH_TX_INT_EN		(1 << 2)

/* enable ps2c receiving timeout interrupt */
#define RX_TIMEOUT_INT_EN	(1 << 1)

/* enable ps2c finish receiving one byte interrupt */
#define FSH_RX_INT_EN		(1 << 0)

/* clear ps2c sending timeout interrupt */
#define TX_TIMEOUT_INT_CLR	(1 << 3)

/* clear ps2c finish sending one byte interrupt */
#define FSH_TX_INT_CLR		(1 << 2)

/* clear ps2c receiving timeout interrupt */
#define RX_TIMEOUT_INT_CLR	(1 << 1)

/* clear ps2c finish receiving one byte interrupt */
#define FSH_RX_INT_CLR		(1 << 0)

/* ps2c sending timeout interrupt status */
#define TX_TIMEOUT_INT_STATUS	(1 << 3)

/* ps2c finish sending one byte interrupt status */
#define FSH_TX_INT_STATUS	(1 << 2)

/* ps2c receiving timeout interrupt status */
#define RX_TIMEOUT_INT_STATUS	(1 << 1)

/* ps2c finish receiving one byte interrupt status */
#define FSH_RX_INT_STATUS	(1 << 0)

/* ps2c sending busy */
#define PS2C_STAT_TX_BUSY	(1 << 7)

/* ps2c sending timeout */
#define PS2C_STAT_TX_TIMEOUT	(1 << 6)

/* ps2c sending error */
#define PS2C_STAT_TX_ERROR	(1 << 5)

/* ps2c sending buffer empty */
#define PS2C_STAT_TX_EMPTY	(1 << 4)

/* ps2c receiving busy */
#define PS2C_STAT_RX_BUSY	(1 << 3)

/* ps2c receiving timeout */
#define PS2C_STAT_RX_TIMEOUT	(1 << 2)

/* ps2c receiving error */
#define PS2C_STAT_RX_ERROR	(1 << 1)

/* ps2c receiving buffer full */
#define PS2C_STAT_RX_FULL	(1 << 0)

enum {
	RX_MODE = 0,
	TX_MODE,
	INHIBITON_MODE,
	RESERVED
};

#define PS2_RX_MODE            (RX_MODE << PS2_MODE_SHIFT)
#define PS2_TX_MODE            (TX_MODE << PS2_MODE_SHIFT)
#define PS2_INHIBITON_MODE     (INHIBITON_MODE << PS2_MODE_SHIFT)

struct ps2key_rk3288_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int sda;
	int sdacfg;
	int scl;
	int sclcfg;
	int irq;
};

static void ps2key_rk3288_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct ps2key_rk3288_pdata_t * pdat = (struct ps2key_rk3288_pdata_t *)input->priv;
	u32_t isr;
	u32_t status;
	u8_t byte;

	isr = read32(pdat->virt + PS2C_ISR) & 0xf;

	printf("\r\n ENTER = = isr = 0x%08x\r\n", isr);
	if(isr & TX_TIMEOUT_INT_STATUS)
	{
		printf("sending timeout\r\n");
	}

	if(isr & RX_TIMEOUT_INT_STATUS)
	{
		printf("RX_TIMEOUT_INT_STATUS timeout\r\n");
	}

	if(isr & FSH_RX_INT_STATUS)
	{
		status = read32(pdat->virt + PS2C_STAT);
		printf("status = 0x%08x\r\n", status);

		if (status & PS2C_STAT_RX_ERROR)
		{
			printf("rx error\r\n");
			write32(pdat->virt + PS2C_STAT, (PS2C_STAT_RX_ERROR << 16) | (~PS2C_STAT_RX_ERROR));
			/* must be read and clear full status */
			if (status & PS2C_STAT_RX_FULL)
			{
				printf("rx err but must be read\r\n");
				byte = read32(pdat->virt + PS2C_RBR) & 0xff;
			}
		}

		if (status & PS2C_STAT_RX_TIMEOUT) {
			printf("rx timeout\r\n");
			write32(pdat->virt + PS2C_STAT, (PS2C_STAT_RX_TIMEOUT << 16) | (~PS2C_STAT_RX_TIMEOUT));
		}

		if (status & PS2C_STAT_RX_FULL) {
			byte = read32(pdat->virt + PS2C_RBR);
			printf("byte = %02x\r\n", byte);
		}
	}

	/* if finish sending  we enable rx mode */
	if (isr & FSH_TX_INT_STATUS)
	{
		write32(pdat->virt + PS2C_CTRL, (PS2C_CTRL_EN << 16) | (~PS2C_CTRL_EN));
		write32(pdat->virt + PS2C_CTRL, (PS2_MODE_MASK << 16 | PS2_RX_MODE));
		write32(pdat->virt + PS2C_RTR, 24000000);
		write32(pdat->virt + PS2C_CTRL, (RX_TIMEOUT_EN << 16) | RX_TIMEOUT_EN);
		write32(pdat->virt + PS2C_CTRL, (PS2C_CTRL_EN << 16) | PS2C_CTRL_EN);
	}

	/* clear interrupts status */
	write32(pdat->virt + PS2C_ICR, (isr << 16) | isr);
}

static int ps2key_rk3288_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * ps2key_rk3288_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ps2key_rk3288_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	pdat = malloc(sizeof(struct ps2key_rk3288_pdata_t));
	if(!pdat)
		return NULL;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->sda = dt_read_int(n, "sda-gpio", -1);
	pdat->sdacfg = dt_read_int(n, "sda-gpio-config", -1);
	pdat->scl = dt_read_int(n, "scl-gpio", -1);
	pdat->sclcfg = dt_read_int(n, "scl-gpio-config", -1);
	pdat->irq = irq;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->type = INPUT_TYPE_KEYBOARD;
	input->ioctl = ps2key_rk3288_ioctl;
	input->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->sda >= 0)
	{
		if(pdat->sdacfg >= 0)
			gpio_set_cfg(pdat->sda, pdat->sdacfg);
		gpio_set_pull(pdat->sda, GPIO_PULL_UP);
	}
	if(pdat->scl >= 0)
	{
		if(pdat->sclcfg >= 0)
			gpio_set_cfg(pdat->scl, pdat->sclcfg);
		gpio_set_pull(pdat->scl, GPIO_PULL_UP);
	}
	request_irq(pdat->irq, ps2key_rk3288_interrupt, IRQ_TYPE_NONE, input);

	write32(pdat->virt + PS2C_CTRL, (PS2C_CTRL_EN << 16) | (~PS2C_CTRL_EN));
	/*ps2c works as inhibition mode, inhibits the ps2 device send data */
	write32(pdat->virt + PS2C_CTRL, ((PS2_MODE_MASK << 16) | (PS2_INHIBITON_MODE)));
	/* ps2c enable */
	write32(pdat->virt + PS2C_CTRL, (PS2C_CTRL_EN << 16) | PS2C_CTRL_EN);


	unsigned long clk_rate = clk_get_rate(pdat->clk);
	unsigned long time_req = 0;

	/* ps2c disable, end ps2c inhibition mode */
	write32(pdat->virt + PS2C_CTRL, (PS2C_CTRL_EN << 16) | (~PS2C_CTRL_EN));

	/* PS/2 receiving mode  */
	write32(pdat->virt + PS2C_CTRL, ((PS2_MODE_MASK << 16) | PS2_RX_MODE));

	/*
	 * 1. according to TRM datasheet PS2C cnt_clk = clk_rate
	 * the register PS2C_RTR time unit is one cnt_clk cycle
	 * so 1us = clk_rate / 1000000,
	 * and 1000ms = 1000000 * clk_rate / 1000000 = clk_rate
	 * ps2c receiving timeout require time 1000ms
	 */

	time_req = clk_rate;
	write32(pdat->virt + PS2C_RTR, time_req);

	/*
	 * ps2c receiving timeout enable. Only when this bit is set, there
	 * PS2C_STAT[2](ps2c_rv_timeouot) is access
	 */
	write32(pdat->virt + PS2C_CTRL, (RX_TIMEOUT_EN << 16) | RX_TIMEOUT_EN );

	/* enable ps2c receiving timeout interrupt */
	write32(pdat->virt + PS2C_IER, (RX_TIMEOUT_INT_EN << 16) | RX_TIMEOUT_INT_EN);
	/* ps2c enable */
	write32(pdat->virt + PS2C_CTRL, (PS2C_CTRL_EN << 16) | PS2C_CTRL_EN);

	/* enable ps2c finish receiving one byte interrupt  */
	write32(pdat->virt + PS2C_IER, (FSH_RX_INT_EN << 16) | FSH_RX_INT_EN);

	/*
	 * we set ps2c sending time here.
	 * 1. according to TRM datasheet PS2C cnt_clk = clk_rate
	 * 2. And these registers PS2C_TRR1,PS2C_TRR2,PS2C_TRR3
	 * 3. Their time unit is one cnt_clk cycle
	 * 1us = clk_rate / 1000000, so we get
	 * 100us = 100 * clk_rate / 1000000 = clk_rate / 10000
	 * 5us = 5 * clk_rate / 1000000 = clk_rate / 200000
	 * 1000ms = 1000000 * clk_rate / 1000000 = clk_rate
	 */

	/* 100 us */
	time_req = clk_rate / 10000;
	write32(pdat->virt + PS2C_TRR1, time_req);

	/* 10us */
	time_req = clk_rate / 200000 * 2;
	write32(pdat->virt + PS2C_TRR2, time_req);

	/* 10us */
	write32(pdat->virt + PS2C_TRR3, time_req);

	/* 1000ms timeout */
	time_req = clk_rate;
	write32(pdat->virt + PS2C_WTR, time_req);

	write32(pdat->virt + PS2C_CTRL, (TX_TIMEOUT_EN << 16) | TX_TIMEOUT_EN);

	/* enable ps2c sending timeout interrupt */
	write32(pdat->virt + PS2C_IER, (TX_TIMEOUT_INT_EN << 16) | TX_TIMEOUT_INT_EN);

	/* enable ps2c finish sending one byte interrupt  */
	write32(pdat->virt + PS2C_IER, (FSH_TX_INT_EN << 16) | FSH_TX_INT_EN);

	write32(pdat->virt + PS2C_FLT, 0x0f);

	if(!register_input(&dev, input))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_irq(pdat->irq);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ps2key_rk3288_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct ps2key_rk3288_pdata_t * pdat = (struct ps2key_rk3288_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_irq(pdat->irq);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void ps2key_rk3288_suspend(struct device_t * dev)
{
}

static void ps2key_rk3288_resume(struct device_t * dev)
{
}

static struct driver_t ps2key_rk3288 = {
	.name		= "ps2key-rk3288",
	.probe		= ps2key_rk3288_probe,
	.remove		= ps2key_rk3288_remove,
	.suspend	= ps2key_rk3288_suspend,
	.resume		= ps2key_rk3288_resume,
};

static __init void ps2key_rk3288_driver_init(void)
{
	register_driver(&ps2key_rk3288);
}

static __exit void ps2key_rk3288_driver_exit(void)
{
	unregister_driver(&ps2key_rk3288);
}

driver_initcall(ps2key_rk3288_driver_init);
driver_exitcall(ps2key_rk3288_driver_exit);
