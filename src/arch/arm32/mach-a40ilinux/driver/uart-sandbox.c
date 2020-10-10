/*
 * driver/uart-sandbox.c
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
#include <uart/uart.h>
#include <sandbox.h>

struct uart_sandbox_pdata_t {
	int fd;
	int baud;
	int data;
	int parity;
	int stop;
};

static bool_t uart_sandbox_set(struct uart_t * uart, int baud, int data, int parity, int stop)
{
	struct uart_sandbox_pdata_t * pdat = (struct uart_sandbox_pdata_t *)uart->priv;

	if(baud < 0)
		return FALSE;
	if((data < 5) || (data > 8))
		return FALSE;
	if((parity < 0) || (parity > 2))
		return FALSE;
	if((stop < 0) || (stop > 2))
		return FALSE;

	pdat->baud = baud;
	pdat->data = data;
	pdat->parity = parity;
	pdat->stop = stop;
	sandbox_uart_set(pdat->fd, pdat->baud, pdat->data, pdat->parity, pdat->stop);

	return TRUE;
}

static bool_t uart_sandbox_get(struct uart_t * uart, int * baud, int * data, int * parity, int * stop)
{
	struct uart_sandbox_pdata_t * pdat = (struct uart_sandbox_pdata_t *)uart->priv;

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

static ssize_t uart_sandbox_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct uart_sandbox_pdata_t * pdat = (struct uart_sandbox_pdata_t *)uart->priv;
	return sandbox_uart_read(pdat->fd, buf ,count);
}

static ssize_t uart_sandbox_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct uart_sandbox_pdata_t * pdat = (struct uart_sandbox_pdata_t *)uart->priv;
	return sandbox_uart_write(pdat->fd, buf, count);
}

static struct device_t * uart_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct uart_sandbox_pdata_t * pdat;
	struct uart_t * uart;
	struct device_t * dev;
	int fd = sandbox_uart_open(dt_read_string(n, "device", NULL));

	if(fd < 0)
		return NULL;

	pdat = malloc(sizeof(struct uart_sandbox_pdata_t));
	if(!pdat)
		return NULL;

	uart = malloc(sizeof(struct uart_t));
	if(!uart)
	{
		free(pdat);
		return NULL;
	}

	pdat->fd = fd;
	pdat->baud = dt_read_int(n, "baud-rates", 115200);
	pdat->data = dt_read_int(n, "data-bits", 8);
	pdat->parity = dt_read_int(n, "parity-bits", 0);
	pdat->stop = dt_read_int(n, "stop-bits", 1);

	uart->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	uart->set = uart_sandbox_set;
	uart->get = uart_sandbox_get;
	uart->read = uart_sandbox_read;
	uart->write = uart_sandbox_write;
	uart->priv = pdat;
	uart_sandbox_set(uart, pdat->baud, pdat->data, pdat->parity, pdat->stop);

	if(!(dev = register_uart(uart, drv)))
	{
		sandbox_uart_close(pdat->fd);
		free_device_name(uart->name);
		free(uart->priv);
		free(uart);
		return NULL;
	}
	return dev;
}

static void uart_sandbox_remove(struct device_t * dev)
{
	struct uart_t * uart = (struct uart_t *)dev->priv;
	struct uart_sandbox_pdata_t * pdat = (struct uart_sandbox_pdata_t *)uart->priv;

	if(uart)
	{
		unregister_uart(uart);
		sandbox_uart_close(pdat->fd);
		free_device_name(uart->name);
		free(uart->priv);
		free(uart);
	}
}

static void uart_sandbox_suspend(struct device_t * dev)
{
}

static void uart_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t uart_sandbox = {
	.name		= "uart-sandbox",
	.probe		= uart_sandbox_probe,
	.remove		= uart_sandbox_remove,
	.suspend	= uart_sandbox_suspend,
	.resume		= uart_sandbox_resume,
};

static __init void uart_sandbox_driver_init(void)
{
	register_driver(&uart_sandbox);
}

static __exit void uart_sandbox_driver_exit(void)
{
	unregister_driver(&uart_sandbox);
}

driver_initcall(uart_sandbox_driver_init);
driver_exitcall(uart_sandbox_driver_exit);
