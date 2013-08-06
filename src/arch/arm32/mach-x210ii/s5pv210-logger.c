/*
 * s5pv210-logger.c
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
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-serial.h>

static void logger_uart2_init(void)
{
}

static void logger_uart2_exit(void)
{
}

static ssize_t logger_uart2_output(const char * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while( !(readl(S5PV210_UTRSTAT2) & S5PV210_UTRSTAT_TXE) );
		writeb(S5PV210_UTXH2, buf[i]);
	}
	return i;
}

static struct logger_t s5pv210_logger = {
	.name	= "logger-uart2",
	.init	= logger_uart2_init,
	.exit	= logger_uart2_exit,
	.output	= logger_uart2_output,
};

static __init void s5pv210_logger_init(void)
{
	if(register_logger(&s5pv210_logger))
		LOG("Register logger '%s'", s5pv210_logger.name);
	else
		LOG("Failed to register logger '%s'", s5pv210_logger.name);
}

static __exit void s5pv210_logger_exit(void)
{
	if(unregister_logger(&s5pv210_logger))
		LOG("Unregister logger '%s'", s5pv210_logger.name);
	else
		LOG("Failed to unregister logger '%s'", s5pv210_logger.name);
}

pure_initcall_sync(s5pv210_logger_init);
pure_exitcall_sync(s5pv210_logger_exit);
