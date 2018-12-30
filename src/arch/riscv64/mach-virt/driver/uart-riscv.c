/*
 * driver/uart-riscv.c
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
#include <clk/clk.h>
#include <reset/reset.h>
#include <gpio/gpio.h>
#include <uart/uart.h>

enum {
	UART_RBR	= 0x00, /* Receive Buffer Register */
	UART_THR	= 0x00, /* Transmit Hold Register */
	UART_IER	= 0x01, /* Interrupt Enable Register */
	UART_DLL	= 0x00, /* Divisor LSB (LCR_DLAB) */
	UART_DLM	= 0x01, /* Divisor MSB (LCR_DLAB) */
	UART_FCR	= 0x02, /* FIFO Control Register */
	UART_LCR	= 0x03, /* Line Control Register */
	UART_MCR	= 0x04, /* Modem Control Register */
	UART_LSR	= 0x05, /* Line Status Register */
	UART_MSR	= 0x06, /* Modem Status Register */
	UART_SCR	= 0x07, /* Scratch Register */
};

struct uart_riscv_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
	int txd;
	int txdcfg;
	int rxd;
	int rxdcfg;
	int baud;
	int data;
	int parity;
	int stop;
};

static bool_t uart_riscv_set(struct uart_t * uart, int baud, int data, int parity, int stop)
{
	struct uart_riscv_pdata_t * pdat = (struct uart_riscv_pdata_t *)uart->priv;
	u8_t dreg, preg, sreg;
	u32_t val, udiv;

	if(baud < 0)
		return FALSE;
	if((data < 5) || (data > 8))
		return FALSE;
	if((parity < 0) || (parity > 2))
		return FALSE;
	if((stop < 0) || (stop > 2))
		return FALSE;

	switch(data)
	{
	case 5:	/* Data bits = 5 */
		dreg = 0x0;
		break;
	case 6:	/* Data bits = 6 */
		dreg = 0x1;
		break;
	case 7:	/* Data bits = 7 */
		dreg = 0x2;
		break;
	case 8:	/* Data bits = 8 */
		dreg = 0x3;
		break;
	default:
		return FALSE;
	}

	switch(parity)
	{
	case 0:	/* Parity none */
		preg = 0x0;
		break;
	case 1:	/* Parity odd */
		preg = 0x1;
		break;
	case 2:	/* Parity even */
		preg = 0x3;
		break;
	default:
		return FALSE;
	}

	switch(stop)
	{
	case 1:	/* Stop bits = 1 */
		sreg = 0;
		break;
	case 2:	/* Stop bits = 2 */
		sreg = 1;
		break;
	case 0:	/* Stop bits = 1.5 */
	default:
		return FALSE;
	}

	pdat->baud = baud;
	pdat->data = data;
	pdat->parity = parity;
	pdat->stop = stop;

	val = read8(pdat->virt + UART_LCR);
	val |= (1 << 7);
	write8(pdat->virt + UART_LCR, val);

	udiv = clk_get_rate(pdat->clk) / (16 * baud);
	write8(pdat->virt + UART_DLL, udiv & 0xff);
	write8(pdat->virt + UART_DLM, (udiv >> 8) & 0xff);

	val = read8(pdat->virt + UART_LCR);
	val &= ~(1 << 7);
	write8(pdat->virt + UART_LCR, val);

	val = read8(pdat->virt + UART_LCR);
	val &= ~0x1f;
	val |= (dreg << 0) | (sreg << 2) | (preg << 3);
	write8(pdat->virt + UART_LCR, val);

	return TRUE;
}

static bool_t uart_riscv_get(struct uart_t * uart, int * baud, int * data, int * parity, int * stop)
{
	struct uart_riscv_pdata_t * pdat = (struct uart_riscv_pdata_t *)uart->priv;

	if(baud)
		*baud = pdat->baud;
	if(data)
		*data = pdat->data;
	if(parity)
		*parity = pdat->parity;
	if(stop)
		*stop = pdat->stop;
	return TRUE;
}

static ssize_t uart_riscv_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct uart_riscv_pdata_t * pdat = (struct uart_riscv_pdata_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if((read8(pdat->virt + UART_LSR) & (0x1 << 0)) != 0)
			buf[i] = (u8_t)read8(pdat->virt + UART_RBR);
		else
			break;
	}
	return i;
}

static ssize_t uart_riscv_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct uart_riscv_pdata_t * pdat = (struct uart_riscv_pdata_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while((read8(pdat->virt + UART_LSR) & (1 << 6)) == 0);
		write8(pdat->virt + UART_THR, buf[i]);
	}
	return i;
}

static struct device_t * uart_riscv_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct uart_riscv_pdata_t * pdat;
	struct uart_t * uart;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct uart_riscv_pdata_t));
	if(!pdat)
		return NULL;

	uart = malloc(sizeof(struct uart_t));
	if(!uart)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->txd = dt_read_int(n, "txd-gpio", -1);
	pdat->txdcfg = dt_read_int(n, "txd-gpio-config", -1);
	pdat->rxd = dt_read_int(n, "rxd-gpio", -1);
	pdat->rxdcfg = dt_read_int(n, "rxd-gpio-config", -1);
	pdat->baud = dt_read_int(n, "baud-rates", 115200);
	pdat->data = dt_read_int(n, "data-bits", 8);
	pdat->parity = dt_read_int(n, "parity-bits", 0);
	pdat->stop = dt_read_int(n, "stop-bits", 1);

	uart->name = alloc_device_name(dt_read_name(n), -1);
	uart->set = uart_riscv_set;
	uart->get = uart_riscv_get;
	uart->read = uart_riscv_read;
	uart->write = uart_riscv_write;
	uart->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	if(pdat->txd >= 0)
	{
		if(pdat->txdcfg >= 0)
			gpio_set_cfg(pdat->txd, pdat->txdcfg);
		gpio_set_pull(pdat->txd, GPIO_PULL_UP);
	}
	if(pdat->rxd >= 0)
	{
		if(pdat->rxdcfg >= 0)
			gpio_set_cfg(pdat->rxd, pdat->rxdcfg);
		gpio_set_pull(pdat->rxd, GPIO_PULL_UP);
	}

	write8(pdat->virt + UART_IER, 0x0);
	write8(pdat->virt + UART_FCR, 0xf7);
	write8(pdat->virt + UART_MCR, 0x0);
	uart_riscv_set(uart, pdat->baud, pdat->data, pdat->parity, pdat->stop);

	if(!register_uart(&dev, uart))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(uart->name);
		free(uart->priv);
		free(uart);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void uart_riscv_remove(struct device_t * dev)
{
	struct uart_t * uart = (struct uart_t *)dev->priv;
	struct uart_riscv_pdata_t * pdat = (struct uart_riscv_pdata_t *)uart->priv;

	if(uart && unregister_uart(uart))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(uart->name);
		free(uart->priv);
		free(uart);
	}
}

static void uart_riscv_suspend(struct device_t * dev)
{
}

static void uart_riscv_resume(struct device_t * dev)
{
}

static struct driver_t uart_riscv = {
	.name		= "uart-riscv",
	.probe		= uart_riscv_probe,
	.remove		= uart_riscv_remove,
	.suspend	= uart_riscv_suspend,
	.resume		= uart_riscv_resume,
};

static __init void uart_riscv_driver_init(void)
{
	register_driver(&uart_riscv);
}

static __exit void uart_riscv_driver_exit(void)
{
	unregister_driver(&uart_riscv);
}

driver_initcall(uart_riscv_driver_init);
driver_exitcall(uart_riscv_driver_exit);
