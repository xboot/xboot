/*
 * realview-logger.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <realview/reg-serial.h>

static void logger_uart0_init(void)
{
	writel(REALVIEW_SERIAL0_CR, 0x0);
	writel(REALVIEW_SERIAL0_CR, REALVIEW_SERIAL_CR_UARTEN |	REALVIEW_SERIAL_CR_TXE | REALVIEW_SERIAL_CR_RXE);
}

static void logger_uart0_exit(void)
{
}

static ssize_t logger_uart0_output(const u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		/* wait until there is space in the fifo */
		while( (readb(REALVIEW_SERIAL0_FR) & REALVIEW_SERIAL_FR_TXFF) );

		/* transmit a character */
		writeb(REALVIEW_SERIAL0_DATA, buf[i]);
	}
	return i;
}

static struct logger_t realview_logger = {
	.name	= "logger-uart0",
	.init	= logger_uart0_init,
	.exit	= logger_uart0_exit,
	.output	= logger_uart0_output,
};

static __init void realview_logger_init(void)
{
	if(!register_logger(&realview_logger))
		LOG("failed to register logger");
}

static __exit void realview_logger_exit(void)
{
	if(!unregister_logger(&realview_logger))
		LOG("failed to unregister logger");
}

core_initcall_sync(realview_logger_init);
core_exitcall_sync(realview_logger_exit);
