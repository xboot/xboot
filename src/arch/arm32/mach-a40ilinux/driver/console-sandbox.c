/*
 * driver/console-sandbox.c
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
#include <console/console.h>
#include <sandbox.h>

static ssize_t console_sandbox_read(struct console_t * console, unsigned char * buf, size_t count)
{
	return sandbox_stdio_read((void *)buf, count);
}

static ssize_t console_sandbox_write(struct console_t * console, const unsigned char * buf, size_t count)
{
	return sandbox_stdio_write((void *)buf, count);
}

static struct device_t * console_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct console_t * console;
	struct device_t * dev;

	console = malloc(sizeof(struct console_t));
	if(!console)
		return NULL;

	console->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	console->read = console_sandbox_read;
	console->write = console_sandbox_write;
	console->priv = NULL;

	if(!(dev = register_console(console, drv)))
	{
		free_device_name(console->name);
		free(console);
		return NULL;
	}
	return dev;
}

static void console_sandbox_remove(struct device_t * dev)
{
	struct console_t * console = (struct console_t *)dev->priv;

	if(console)
	{
		unregister_console(console);
		free_device_name(console->name);
		free(console);
	}
}

static void console_sandbox_suspend(struct device_t * dev)
{
}

static void console_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t console_sandbox = {
	.name		= "console-sandbox",
	.probe		= console_sandbox_probe,
	.remove		= console_sandbox_remove,
	.suspend	= console_sandbox_suspend,
	.resume		= console_sandbox_resume,
};

static __init void console_sandbox_driver_init(void)
{
	register_driver(&console_sandbox);
}

static __exit void console_sandbox_driver_exit(void)
{
	unregister_driver(&console_sandbox);
}

driver_initcall(console_sandbox_driver_init);
driver_exitcall(console_sandbox_driver_exit);
