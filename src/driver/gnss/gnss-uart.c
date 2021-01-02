/*
 * driver/gnss/gnss-uart.c
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
#include <regulator/regulator.h>
#include <uart/uart.h>
#include <gnss/gnss.h>

/*
 * GNSS UART - Global Navigation Satellite System Based On Uart
 *
 * Required properties:
 * - uart-bus: uart bus name which device attached
 *
 * Optional properties:
 * - regulator-name: the gnss power regulator
 * - baud-rates: uart baud rates
 * - data-bits: uart data bits
 * - parity-bits: uart parity bits
 * - stop-bits: uart stop bits
 *
 * Example:
 *  "gnss-uart@0": {
 *      "regulator-name": "gnss-power-3v3",
 *      "uart-bus": "uart-pl011.0",
 *      "baud-rates": 9600,
 *      "data-bits": 8,
 *      "parity-bits": 0,
 *      "stop-bits": 1
 *   },
 */

struct gnss_uart_pdata_t {
	char * regulator;
	struct uart_t * uart;
	int baud;
	int data;
	int parity;
	int stop;
	int enable;
};

static void gnss_uart_enable(struct gnss_t * nav)
{
	struct gnss_uart_pdata_t * pdat = (struct gnss_uart_pdata_t *)nav->priv;

	if(pdat->enable != 1)
	{
		if(pdat->regulator)
			regulator_enable(pdat->regulator);
		uart_set(pdat->uart, pdat->baud, pdat->data, pdat->parity, pdat->stop);
		pdat->enable = 1;
	}
}

static void gnss_uart_disable(struct gnss_t * nav)
{
	struct gnss_uart_pdata_t * pdat = (struct gnss_uart_pdata_t *)nav->priv;

	if(pdat->enable != 0)
	{
		if(pdat->regulator)
			regulator_disable(pdat->regulator);
		pdat->enable = 0;
	}
}

static int gnss_uart_read(struct gnss_t * nav, void * buf, int count)
{
	struct gnss_uart_pdata_t * pdat = (struct gnss_uart_pdata_t *)nav->priv;

	return uart_read(pdat->uart, buf, count);
}

static struct device_t * gnss_uart_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gnss_uart_pdata_t * pdat;
	struct gnss_t * nav;
	struct device_t * dev;
	struct uart_t * uart = search_uart(dt_read_string(n, "uart-bus", NULL));

	if(!uart)
		return NULL;

	pdat = malloc(sizeof(struct gnss_uart_pdata_t));
	if(!pdat)
		return NULL;

	nav = malloc(sizeof(struct gnss_t));
	if(!nav)
	{
		free(pdat);
		return NULL;
	}
	memset(nav, 0, sizeof(struct gnss_t));

	pdat->regulator = strdup(dt_read_string(n, "regulator-name", NULL));
	pdat->uart = uart;
	pdat->baud = dt_read_int(n, "baud-rates", 9600);
	pdat->data = dt_read_int(n, "data-bits", 8);
	pdat->parity = dt_read_int(n, "parity-bits", 0);
	pdat->stop = dt_read_int(n, "stop-bits", 1);
	pdat->enable = -1;

	nav->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	nav->enable = gnss_uart_enable;
	nav->disable = gnss_uart_disable;
	nav->read = gnss_uart_read;
	nav->priv = pdat;

	gnss_uart_disable(nav);

	if(!(dev = register_gnss(nav, drv)))
	{
		if(pdat->regulator)
			free(pdat->regulator);
		free_device_name(nav->name);
		free(nav->priv);
		free(nav);
		return NULL;
	}
	return dev;
}

static void gnss_uart_remove(struct device_t * dev)
{
	struct gnss_t * nav = (struct gnss_t *)dev->priv;
	struct gnss_uart_pdata_t * pdat = (struct gnss_uart_pdata_t *)nav->priv;

	if(nav)
	{
		unregister_gnss(nav);
		if(pdat->regulator)
			free(pdat->regulator);
		free_device_name(nav->name);
		free(nav->priv);
		free(nav);
	}
}

static void gnss_uart_suspend(struct device_t * dev)
{
	struct gnss_t * nav = (struct gnss_t *)dev->priv;
	struct gnss_uart_pdata_t * pdat = (struct gnss_uart_pdata_t *)nav->priv;

	if((pdat->enable == 1) && pdat->regulator)
		regulator_disable(pdat->regulator);
}

static void gnss_uart_resume(struct device_t * dev)
{
	struct gnss_t * nav = (struct gnss_t *)dev->priv;
	struct gnss_uart_pdata_t * pdat = (struct gnss_uart_pdata_t *)nav->priv;

	if((pdat->enable == 1) && pdat->regulator)
		regulator_enable(pdat->regulator);
}

static struct driver_t gnss_uart = {
	.name		= "gnss-uart",
	.probe		= gnss_uart_probe,
	.remove		= gnss_uart_remove,
	.suspend	= gnss_uart_suspend,
	.resume		= gnss_uart_resume,
};

static __init void gnss_uart_driver_init(void)
{
	register_driver(&gnss_uart);
}

static __exit void gnss_uart_driver_exit(void)
{
	unregister_driver(&gnss_uart);
}

driver_initcall(gnss_uart_driver_init);
driver_exitcall(gnss_uart_driver_exit);
