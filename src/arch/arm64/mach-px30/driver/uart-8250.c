/*
 * driver/uart-8250.c
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
#include <gpio/gpio.h>
#include <uart/uart.h>

/*
 * Uart 8250 - Universal Asynchronous Receiver Transmitter
 *
 * Required properties:
 * - clock-name: uart parant clock name
 *
 * Optional properties:
 * - txd-gpio: uart txd gpio
 * - txd-gpio-config: uart txd gpio config
 * - rxd-gpio: uart rxd gpio
 * - rxd-gpio-config: uart rxd gpio config
 * - baud-rates: uart baud rates, default is 115200
 * - data-bits: uart data bits, default is 8
 * - parity-bits: uart parity bits, default is 0
 * - stop-bits: uart stop bits, default is 1
 *
 * Example:
 *   "uart-8250@0xff690000": {
 *       "clock-name": "link-uart2",
 *       "txd-gpio": 247,
 *       "txd-gpio-config": 1,
 *       "rxd-gpio": 246,
 *       "rxd-gpio-config": 1,
 *       "baud-rates": 115200,
 *       "data-bits": 8,
 *       "parity-bits": 0,
 *       "stop-bits": 1
 *   }
 */

#define UART_RBR	(0x00)
#define UART_THR	(0x00)
#define UART_DLL	(0x00)
#define UART_DLH	(0x04)
#define UART_IER	(0x04)
#define UART_IIR	(0x08)
#define UART_FCR	(0x08)
#define UART_LCR	(0x0c)
#define UART_MCR	(0x10)
#define UART_LSR	(0x14)
#define UART_MSR	(0x18)
#define UART_SCR	(0x1c)
#define UART_FAR	(0x70)
#define UART_TFR	(0x74)
#define UART_RFW	(0x78)
#define UART_USR	(0x7c)
#define UART_TFL	(0x80)
#define UART_RFL	(0x84)
#define UART_SRR	(0x88)
#define UART_SRTS	(0x8c)
#define UART_SBCR	(0x90)
#define UART_SDMAM	(0x94)
#define UART_SFE	(0x98)
#define UART_SRT	(0x9c)
#define UART_STET	(0xa0)
#define UART_HTX	(0xa4)
#define UART_DMASA	(0xa8)
#define UART_CPR	(0xf4)
#define UART_UCV	(0xf8)
#define UART_CTR	(0xfc)

struct uart_8250_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int txd;
	int txdcfg;
	int rxd;
	int rxdcfg;
	int baud;
	int data;
	int parity;
	int stop;
};

static bool_t uart_8250_set(struct uart_t * uart, int baud, int data, int parity, int stop)
{
	struct uart_8250_pdata_t * pdat = (struct uart_8250_pdata_t *)uart->priv;
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

	val = read32(pdat->virt + UART_LCR);
	val |= (1 << 7);
	write32(pdat->virt + UART_LCR, val);

	udiv = clk_get_rate(pdat->clk) / (16 * baud);
	write32(pdat->virt + UART_DLL, udiv & 0xff);
	write32(pdat->virt + UART_DLH, (udiv >> 8) & 0xff);

	val = read32(pdat->virt + UART_LCR);
	val &= ~(1 << 7);
	write32(pdat->virt + UART_LCR, val);

	val = read32(pdat->virt + UART_LCR);
	val &= ~0x1f;
	val |= (dreg << 0) | (sreg << 2) | (preg << 3);
	write32(pdat->virt + UART_LCR, val);

	return TRUE;
}

static bool_t uart_8250_get(struct uart_t * uart, int * baud, int * data, int * parity, int * stop)
{
	struct uart_8250_pdata_t * pdat = (struct uart_8250_pdata_t *)uart->priv;

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

static ssize_t uart_8250_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct uart_8250_pdata_t * pdat = (struct uart_8250_pdata_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if((read32(pdat->virt + UART_USR) & (0x1 << 3)) != 0)
			buf[i] = (u8_t)read32(pdat->virt + UART_RBR);
		else
			break;
	}
	return i;
}

static ssize_t uart_8250_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct uart_8250_pdata_t * pdat = (struct uart_8250_pdata_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while((read32(pdat->virt + UART_USR) & (0x1 << 1)) == 0);
		write32(pdat->virt + UART_THR, buf[i]);
	}
	return i;
}

static struct device_t * uart_8250_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct uart_8250_pdata_t * pdat;
	struct uart_t * uart;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct uart_8250_pdata_t));
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
	pdat->txd = dt_read_int(n, "txd-gpio", -1);
	pdat->txdcfg = dt_read_int(n, "txd-gpio-config", -1);
	pdat->rxd = dt_read_int(n, "rxd-gpio", -1);
	pdat->rxdcfg = dt_read_int(n, "rxd-gpio-config", -1);
	pdat->baud = dt_read_int(n, "baud-rates", 115200);
	pdat->data = dt_read_int(n, "data-bits", 8);
	pdat->parity = dt_read_int(n, "parity-bits", 0);
	pdat->stop = dt_read_int(n, "stop-bits", 1);

	uart->name = alloc_device_name(dt_read_name(n), -1);
	uart->set = uart_8250_set;
	uart->get = uart_8250_get;
	uart->read = uart_8250_read;
	uart->write = uart_8250_write;
	uart->priv = pdat;

	clk_enable(pdat->clk);
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

/*	write32(pdat->virt + UART_SRR, (1 << 0) | (1 << 1) | (1 << 2));
	write32(pdat->virt + UART_IER, 0x0);
	write32(pdat->virt + UART_MCR, 0x0);
	write32(pdat->virt + UART_SFE, 0x1);
	write32(pdat->virt + UART_SRT, 0x3);
	write32(pdat->virt + UART_STET, 0x1);
	uart_8250_set(uart, pdat->baud, pdat->data, pdat->parity, pdat->stop);*/

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

static void uart_8250_remove(struct device_t * dev)
{
	struct uart_t * uart = (struct uart_t *)dev->priv;
	struct uart_8250_pdata_t * pdat = (struct uart_8250_pdata_t *)uart->priv;

	if(uart && unregister_uart(uart))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(uart->name);
		free(uart->priv);
		free(uart);
	}
}

static void uart_8250_suspend(struct device_t * dev)
{
}

static void uart_8250_resume(struct device_t * dev)
{
}

static struct driver_t uart_8250 = {
	.name		= "uart-8250",
	.probe		= uart_8250_probe,
	.remove		= uart_8250_remove,
	.suspend	= uart_8250_suspend,
	.resume		= uart_8250_resume,
};

static __init void uart_8250_driver_init(void)
{
	register_driver(&uart_8250);
}

static __exit void uart_8250_driver_exit(void)
{
	unregister_driver(&uart_8250);
}

driver_initcall(uart_8250_driver_init);
driver_exitcall(uart_8250_driver_exit);
