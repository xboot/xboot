/*
 * driver/uart-bcm2837-aux.c
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
#include <bcm2837-aux.h>

/*
 * Mini Uart - Universal Asynchronous Receiver Transmitter
 *
 * Required properties:
 * - clock: uart parant clock name
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
 *   "uart-bcm2837-aux@0x3f215040": {
 *       "clock": "core-clk",
 *       "txd-gpio": 14,
 *       "txd-gpio-config": 5,
 *       "rxd-gpio": 15,
 *       "rxd-gpio-config": 5,
 *       "baud-rates": 115200,
 *       "data-bits": 8,
 *       "parity-bits": 0,
 *       "stop-bits": 1
 *   }
 */

enum {
	UART_IO			= 0x00,
	UART_IER		= 0x04,
	UART_IIR 		= 0x08,
	UART_LCR 		= 0x0c,
	UART_MCR 		= 0x10,
	UART_LSR 		= 0x14,
	UART_MSR 		= 0x18,
	UART_SCRATCH	= 0x1c,
	UART_CNTL		= 0x20,
	UART_STAT 		= 0x24,
	UART_BAUD 		= 0x28,
};

struct uart_bcm2837_aux_pdata_t {
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

static bool_t uart_bcm2837_aux_set(struct uart_t * uart, int baud, int data, int parity, int stop)
{
	struct uart_bcm2837_aux_pdata_t * pdat = (struct uart_bcm2837_aux_pdata_t *)uart->priv;
	u64_t uclk;
	u32_t divider;
	u8_t dreg;

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
	case 7:	/* Data bits = 7 */
		dreg = 0x2;
		break;
	case 8:	/* Data bits = 8 */
		dreg = 0x3;
		break;
	case 5:	/* Data bits = 5 */
	case 6:	/* Data bits = 6 */
	default:
		return FALSE;
	}

	switch(parity)
	{
	case 0:	/* Parity none */
		break;
	case 1:	/* Parity odd */
	case 2:	/* Parity even */
	default:
		return FALSE;
	}

	switch(stop)
	{
	case 1:	/* Stop bits = 1 */
		break;
	case 2:	/* Stop bits = 2 */
	case 0:	/* Stop bits = 1.5 */
	default:
		return FALSE;
	}

	pdat->baud = baud;
	pdat->data = data;
	pdat->parity = parity;
	pdat->stop = stop;

	uclk = clk_get_rate(pdat->clk);
	divider = uclk / (8 * baud) - 1;
	write32(pdat->virt + UART_LCR, (read32(pdat->virt + UART_LCR) & ~(0x3 << 0)) | (dreg << 0));
	write32(pdat->virt + UART_BAUD, divider);

	return TRUE;
}

static bool_t uart_bcm2837_aux_get(struct uart_t * uart, int * baud, int * data, int * parity, int * stop)
{
	struct uart_bcm2837_aux_pdata_t * pdat = (struct uart_bcm2837_aux_pdata_t *)uart->priv;

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

static ssize_t uart_bcm2837_aux_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct uart_bcm2837_aux_pdata_t * pdat = (struct uart_bcm2837_aux_pdata_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if((read8(pdat->virt + UART_LSR) & 0x01))
			buf[i] = read8(pdat->virt + UART_IO);
		else
			break;
	}
	return i;
}

static ssize_t uart_bcm2837_aux_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct uart_bcm2837_aux_pdata_t * pdat = (struct uart_bcm2837_aux_pdata_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while(!(read8(pdat->virt + UART_LSR) & 0x20));
		write8(pdat->virt + UART_IO, buf[i]);
	}
	return i;
}

static struct device_t * uart_bcm2837_aux_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct uart_bcm2837_aux_pdata_t * pdat;
	struct uart_t * uart;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct uart_bcm2837_aux_pdata_t));
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
	uart->set = uart_bcm2837_aux_set;
	uart->get = uart_bcm2837_aux_get;
	uart->read = uart_bcm2837_aux_read;
	uart->write = uart_bcm2837_aux_write;
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
	bcm2837_aux_enable(AUX_ID_UART);
	write32(pdat->virt + UART_CNTL, 0);
	write32(pdat->virt + UART_IER, 0);
	write32(pdat->virt + UART_IIR, 0xc6);
	write32(pdat->virt + UART_LCR, 3);
	write32(pdat->virt + UART_MCR, 0);
	write32(pdat->virt + UART_CNTL, 0x3);
	uart_bcm2837_aux_set(uart, pdat->baud, pdat->data, pdat->parity, pdat->stop);

	if(!register_uart(&dev, uart))
	{
		bcm2837_aux_disable(AUX_ID_UART);
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

static void uart_bcm2837_aux_remove(struct device_t * dev)
{
	struct uart_t * uart = (struct uart_t *)dev->priv;
	struct uart_bcm2837_aux_pdata_t * pdat = (struct uart_bcm2837_aux_pdata_t *)uart->priv;

	if(uart && unregister_uart(uart))
	{
		bcm2837_aux_disable(AUX_ID_UART);
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(uart->name);
		free(uart->priv);
		free(uart);
	}
}

static void uart_bcm2837_aux_suspend(struct device_t * dev)
{
}

static void uart_bcm2837_aux_resume(struct device_t * dev)
{
}

static struct driver_t uart_bcm2837_aux = {
	.name		= "uart-bcm2837-aux",
	.probe		= uart_bcm2837_aux_probe,
	.remove		= uart_bcm2837_aux_remove,
	.suspend	= uart_bcm2837_aux_suspend,
	.resume		= uart_bcm2837_aux_resume,
};

static __init void uart_bcm2837_aux_driver_init(void)
{
	register_driver(&uart_bcm2837_aux);
}

static __exit void uart_bcm2837_aux_driver_exit(void)
{
	unregister_driver(&uart_bcm2837_aux);
}

driver_initcall(uart_bcm2837_aux_driver_init);
driver_exitcall(uart_bcm2837_aux_driver_exit);
