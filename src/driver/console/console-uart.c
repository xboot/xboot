/*
 * driver/console/console-uart.c
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
#include <uart/uart.h>
#include <console/console.h>

struct console_uart_pdata_t {
	struct uart_t * uart;
};

static ssize_t console_uart_read(struct console_t * console, unsigned char * buf, size_t count)
{
	struct console_uart_pdata_t * pdat = (struct console_uart_pdata_t *)console->priv;
	return pdat->uart->read(pdat->uart, (u8_t *)buf, count);
}

static ssize_t console_uart_write(struct console_t * console, const unsigned char * buf, size_t count)
{
	struct console_uart_pdata_t * pdat = (struct console_uart_pdata_t *)console->priv;
	return pdat->uart->write(pdat->uart, (const u8_t *)buf, count);
}

static struct device_t * console_uart_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct console_uart_pdata_t * pdat;
	struct console_t * console;
	struct device_t * dev;
	struct uart_t * uart = search_uart(dt_read_string(n, "uart-bus", NULL));

	if(!uart)
		return NULL;

	pdat = malloc(sizeof(struct console_uart_pdata_t));
	if(!pdat)
		return NULL;

	console = malloc(sizeof(struct console_t));
	if(!console)
	{
		free(pdat);
		return NULL;
	}

	pdat->uart = uart;

	console->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	console->read = console_uart_read;
	console->write = console_uart_write;
	console->priv = pdat;

	if(!register_console(&dev, console))
	{
		free_device_name(console->name);
		free(console->priv);
		free(console);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void console_uart_remove(struct device_t * dev)
{
	struct console_t * console = (struct console_t *)dev->priv;

	if(console && unregister_console(console))
	{
		free_device_name(console->name);
		free(console->priv);
		free(console);
	}
}

static void console_uart_suspend(struct device_t * dev)
{
}

static void console_uart_resume(struct device_t * dev)
{
}

static struct driver_t console_uart = {
	.name		= "console-uart",
	.probe		= console_uart_probe,
	.remove		= console_uart_remove,
	.suspend	= console_uart_suspend,
	.resume		= console_uart_resume,
};

static __init void console_uart_driver_init(void)
{
	register_driver(&console_uart);
}

static __exit void console_uart_driver_exit(void)
{
	unregister_driver(&console_uart);
}

driver_initcall(console_uart_driver_init);
driver_exitcall(console_uart_driver_exit);
