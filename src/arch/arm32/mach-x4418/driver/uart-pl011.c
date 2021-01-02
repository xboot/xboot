/*
 * driver/uart-pl011.c
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
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <uart/uart.h>

/*
 * PrimeCell PL011 - Universal Asynchronous Receiver Transmitter
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
 *   "uart-pl011@0x10009000": {
 *       "clock-name": "uclk",
 *       "txd-gpio": -1,
 *       "txd-gpio-config": -1,
 *       "rxd-gpio": -1,
 *       "rxd-gpio-config": -1,
 *       "baud-rates": 115200,
 *       "data-bits": 8,
 *       "parity-bits": 0,
 *       "stop-bits": 1
 *   }
 */

enum {
	UART_DATA	= 0x00,
	UART_RSR	= 0x04,
	UART_FR		= 0x18,
	UART_ILPR	= 0x20,
	UART_IBRD	= 0x24,
	UART_FBRD	= 0x28,
	UART_LCRH	= 0x2c,
	UART_CR		= 0x30,
	UART_IFLS	= 0x34,
	UART_IMSC	= 0x38,
	UART_RIS	= 0x3c,
	UART_MIS	= 0x40,
	UART_ICR	= 0x44,
	UART_DMACR	= 0x48,
};

struct uart_pl011_pdata_t {
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

static bool_t uart_pl011_set(struct uart_t * uart, int baud, int data, int parity, int stop)
{
	struct uart_pl011_pdata_t * pdat = (struct uart_pl011_pdata_t *)uart->priv;
	u32_t divider, remainder, fraction, val;
	u8_t dreg, preg, sreg;
	u64_t uclk;

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

	/*
	 * IBRD = UART_CLK / (16 * BAUD_RATE)
	 * FBRD = ROUND((64 * MOD(UART_CLK, (16 * BAUD_RATE))) / (16 * BAUD_RATE))
	 */
	uclk = clk_get_rate(pdat->clk);
	divider = uclk / (16 * baud);
	remainder = uclk % (16 * baud);
	fraction = (8 * remainder / baud) >> 1;
	fraction += (8 * remainder / baud) & 1;

	write32(pdat->virt + UART_IBRD, divider);
	write32(pdat->virt + UART_FBRD, fraction);
	val = read32(pdat->virt + UART_LCRH);
	val &= ~0x6e;
	val |= (dreg << 5) | (sreg << 3) | (preg << 1);
	write32(pdat->virt + UART_LCRH, val);

	return TRUE;
}

static bool_t uart_pl011_get(struct uart_t * uart, int * baud, int * data, int * parity, int * stop)
{
	struct uart_pl011_pdata_t * pdat = (struct uart_pl011_pdata_t *)uart->priv;

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

static ssize_t uart_pl011_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct uart_pl011_pdata_t * pdat = (struct uart_pl011_pdata_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if(!(read8(pdat->virt + UART_FR) & (0x1 << 4)))
			buf[i] = read8(pdat->virt + UART_DATA);
		else
			break;
	}
	return i;
}

static ssize_t uart_pl011_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct uart_pl011_pdata_t * pdat = (struct uart_pl011_pdata_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while((read8(pdat->virt + UART_FR) & (0x1 << 5)));
		write8(pdat->virt + UART_DATA, buf[i]);
	}
	return i;
}

static struct device_t * uart_pl011_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct uart_pl011_pdata_t * pdat;
	struct uart_t * uart;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x011)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct uart_pl011_pdata_t));
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
	uart->set = uart_pl011_set;
	uart->get = uart_pl011_get;
	uart->read = uart_pl011_read;
	uart->write = uart_pl011_write;
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
	write32(pdat->virt + UART_LCRH, read32(pdat->virt + UART_LCRH) | (1 << 4));
	write32(pdat->virt + UART_CR, (1 << 0) | (1 << 8) | (1 << 9));
	uart_pl011_set(uart, pdat->baud, pdat->data, pdat->parity, pdat->stop);

	if(!(dev = register_uart(uart, drv)))
	{
		write32(pdat->virt + UART_CR, 0x0);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(uart->name);
		free(uart->priv);
		free(uart);
		return NULL;
	}
	return dev;
}

static void uart_pl011_remove(struct device_t * dev)
{
	struct uart_t * uart = (struct uart_t *)dev->priv;
	struct uart_pl011_pdata_t * pdat = (struct uart_pl011_pdata_t *)uart->priv;

	if(uart)
	{
		unregister_uart(uart);
		write32(pdat->virt + UART_CR, 0x0);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(uart->name);
		free(uart->priv);
		free(uart);
	}
}

static void uart_pl011_suspend(struct device_t * dev)
{
}

static void uart_pl011_resume(struct device_t * dev)
{
}

static struct driver_t uart_pl011 = {
	.name		= "uart-pl011",
	.probe		= uart_pl011_probe,
	.remove		= uart_pl011_remove,
	.suspend	= uart_pl011_suspend,
	.resume		= uart_pl011_resume,
};

static __init void uart_pl011_driver_init(void)
{
	register_driver(&uart_pl011);
}

static __exit void uart_pl011_driver_exit(void)
{
	unregister_driver(&uart_pl011);
}

driver_initcall(uart_pl011_driver_init);
driver_exitcall(uart_pl011_driver_exit);
