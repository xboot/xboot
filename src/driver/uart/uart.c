/*
 * driver/uart/uart.c
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

static ssize_t uart_read_baud(struct kobj_t * kobj, void * buf, size_t size)
{
	struct uart_t * uart = (struct uart_t *)kobj->priv;
	int baud, data, parity, stop;
	uart_get(uart, &baud, &data, &parity, &stop);
	return sprintf(buf, "%d", baud);
}

static ssize_t uart_write_baud(struct kobj_t * kobj, void * buf, size_t size)
{
	struct uart_t * uart = (struct uart_t *)kobj->priv;
	int baud, data, parity, stop;
	uart_get(uart, &baud, &data, &parity, &stop);
	uart_set(uart, strtol(buf, NULL, 0), data, parity, stop);
	return size;
}

static ssize_t uart_read_data(struct kobj_t * kobj, void * buf, size_t size)
{
	struct uart_t * uart = (struct uart_t *)kobj->priv;
	int baud, data, parity, stop;
	uart_get(uart, &baud, &data, &parity, &stop);
	return sprintf(buf, "%d", data);
}

static ssize_t uart_write_data(struct kobj_t * kobj, void * buf, size_t size)
{
	struct uart_t * uart = (struct uart_t *)kobj->priv;
	int baud, data, parity, stop;
	uart_get(uart, &baud, &data, &parity, &stop);
	uart_set(uart, baud, strtol(buf, NULL, 0), parity, stop);
	return size;
}

static ssize_t uart_read_parity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct uart_t * uart = (struct uart_t *)kobj->priv;
	int baud, data, parity, stop;
	uart_get(uart, &baud, &data, &parity, &stop);
	return sprintf(buf, "%d", parity);
}

static ssize_t uart_write_parity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct uart_t * uart = (struct uart_t *)kobj->priv;
	int baud, data, parity, stop;
	uart_get(uart, &baud, &data, &parity, &stop);
	uart_set(uart, baud, data, strtol(buf, NULL, 0), stop);
	return size;
}

static ssize_t uart_read_stop(struct kobj_t * kobj, void * buf, size_t size)
{
	struct uart_t * uart = (struct uart_t *)kobj->priv;
	int baud, data, parity, stop;
	uart_get(uart, &baud, &data, &parity, &stop);
	return sprintf(buf, "%d", stop);
}

static ssize_t uart_write_stop(struct kobj_t * kobj, void * buf, size_t size)
{
	struct uart_t * uart = (struct uart_t *)kobj->priv;
	int baud, data, parity, stop;
	uart_get(uart, &baud, &data, &parity, &stop);
	uart_set(uart, baud, data, parity, strtol(buf, NULL, 0));
	return size;
}

struct uart_t * search_uart(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_UART);
	if(!dev)
		return NULL;
	return (struct uart_t *)dev->priv;
}

bool_t register_uart(struct device_t ** device, struct uart_t * uart)
{
	struct device_t * dev;

	if(!uart || !uart->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(uart->name);
	dev->type = DEVICE_TYPE_UART;
	dev->priv = uart;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "baud", uart_read_baud, uart_write_baud, uart);
	kobj_add_regular(dev->kobj, "data", uart_read_data, uart_write_data, uart);
	kobj_add_regular(dev->kobj, "parity", uart_read_parity, uart_write_parity, uart);
	kobj_add_regular(dev->kobj, "stop", uart_read_stop, uart_write_stop, uart);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_uart(struct uart_t * uart)
{
	struct device_t * dev;

	if(!uart || !uart->name)
		return FALSE;

	dev = search_device(uart->name, DEVICE_TYPE_UART);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

bool_t uart_set(struct uart_t * uart, int baud, int data, int parity, int stop)
{
	if(uart && uart->set)
		return uart->set(uart, baud, data, parity, stop);
	return FALSE;
}

bool_t uart_get(struct uart_t * uart, int * baud, int * data, int * parity, int * stop)
{
	if(uart && uart->get)
		return uart->get(uart, baud, data, parity, stop);
	return FALSE;
}

ssize_t uart_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	if(uart && uart->read)
		return uart->read(uart, buf, count);
	return 0;
}

ssize_t uart_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	if(uart && uart->write)
		return uart->write(uart, buf, count);
	return 0;
}
