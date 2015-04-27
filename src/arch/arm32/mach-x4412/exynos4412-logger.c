/*
 * exynos4412-logger.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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
#include <exynos4412/reg-gpio.h>
#include <exynos4412/reg-uart.h>

static void logger_uart3_init(void)
{
}

static void logger_uart3_exit(void)
{
}

static ssize_t logger_uart3_output(const char * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while( !(read32(EXYNOS4412_UART3_BASE + EXYNOS4412_UTRSTAT) & EXYNOS4412_UTRSTAT_TXFE) );
		write8(EXYNOS4412_UART3_BASE + EXYNOS4412_UTXH, buf[i]);
	}
	return i;
}

static struct logger_t exynos4412_logger = {
	.name	= "logger-uart3",
	.init	= logger_uart3_init,
	.exit	= logger_uart3_exit,
	.output	= logger_uart3_output,
};

static __init void exynos4412_logger_init(void)
{
	if(register_logger(&exynos4412_logger))
		LOG("Register logger '%s'", exynos4412_logger.name);
	else
		LOG("Failed to register logger '%s'", exynos4412_logger.name);
}

static __exit void exynos4412_logger_exit(void)
{
	if(unregister_logger(&exynos4412_logger))
		LOG("Unregister logger '%s'", exynos4412_logger.name);
	else
		LOG("Failed to unregister logger '%s'", exynos4412_logger.name);
}

pure_initcall(exynos4412_logger_init);
pure_exitcall(exynos4412_logger_exit);
