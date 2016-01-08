/*
 * bcm2836-logger.c
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
#include <bus/uart.h>
#include <bcm2836-gpio.h>
#include <bcm2836/reg-gpio.h>
#include <bcm2836/reg-uart.h>

static bool_t bcm2836_uart_setup(enum baud_rate_t baud, enum data_bits_t data, enum parity_bits_t parity, enum stop_bits_t stop)
{
	u32_t ibaud, divider, fraction;
	u32_t temp, remainder;
	u8_t dreg, preg, sreg;
	u64_t uclk = 3 * 1000 * 1000;

	switch(baud)
	{
	case B50:
		ibaud = 50;
		break;
	case B75:
		ibaud = 75;
		break;
	case B110:
		ibaud = 110;
		break;
	case B134:
		ibaud = 134;
		break;
	case B200:
		ibaud = 200;
		break;
	case B300:
		ibaud = 300;
		break;
	case B600:
		ibaud = 600;
		break;
	case B1200:
		ibaud = 1200;
		break;
	case B1800:
		ibaud = 1800;
		break;
	case B2400:
		ibaud = 2400;
		break;
	case B4800:
		ibaud = 4800;
		break;
	case B9600:
		ibaud = 9600;
		break;
	case B19200:
		ibaud = 19200;
		break;
	case B38400:
		ibaud = 38400;
		break;
	case B57600:
		ibaud = 57600;
		break;
	case B76800:
		ibaud = 76800;
		break;
	case B115200:
		ibaud = 115200;
		break;
	case B230400:
		ibaud = 230400;
		break;
	case B380400:
		ibaud = 380400;
		break;
	case B460800:
		ibaud = 460800;
		break;
	case B921600:
		ibaud = 921600;
		break;
	default:
		return FALSE;
	}

	switch(data)
	{
	case DATA_BITS_5:
		dreg = 0x0;
		break;
	case DATA_BITS_6:
		dreg = 0x1;
		break;
	case DATA_BITS_7:
		dreg = 0x2;
		break;
	case DATA_BITS_8:
		dreg = 0x3;
		break;
	default:
		return FALSE;
	}

	switch(parity)
	{
	case PARITY_NONE:
		preg = 0x0;
		break;
	case PARITY_EVEN:
		preg = 0x2;
		break;
	case PARITY_ODD:
		preg = 0x1;
		break;
	default:
		return FALSE;
	}

	switch(stop)
	{
	case STOP_BITS_1:
		sreg = 0;
		break;
	case STOP_BITS_2:
		sreg = 1;
		break;
	case STOP_BITS_1_5:
	default:
		return FALSE;
	}

	/*
	 * IBRD = UART_CLK / (16 * BAUD_RATE)
	 * FBRD = ROUND((64 * MOD(UART_CLK, (16 * BAUD_RATE))) / (16 * BAUD_RATE))
	 */
	temp = 16 * ibaud;
	divider = (u32_t)(uclk / temp);
	remainder = (u32_t)(uclk % temp);
	temp = (8 * remainder) / ibaud;
	fraction = (temp >> 1) + (temp & 1);

	write32(phys_to_virt(BCM2836_UART0_BASE + UART_IBRD), divider);
	write32(phys_to_virt(BCM2836_UART0_BASE + UART_FBRD), fraction);
	write32(phys_to_virt(BCM2836_UART0_BASE + UART_LCRH), (1 << 4) | (dreg<<5 | sreg<<3 | preg<<1));

	return TRUE;
}

void debug_init(void)
{
	int offset;
	int bank;
	int field;
	int cfg;
	u32_t val;

	offset = BCM2836_GPIO(14);
	cfg = 0x4;
	bank = offset / 10;
	field = (offset - 10 * bank) * 3;
	val = read32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_FSEL(bank)));
	val &= ~(0x7 << field);
	val |= cfg << field;
	write32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_FSEL(bank)), val);


	offset = BCM2836_GPIO(15);
	cfg = 0x4;
	bank = offset / 10;
	field = (offset - 10 * bank) * 3;
	val = read32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_FSEL(bank)));
	val &= ~(0x7 << field);
	val |= cfg << field;
	write32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_FSEL(bank)), val);


	offset = BCM2836_GPIO(14);
	bank = offset / 32;
	field = (offset - 32 * bank);
	write32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_UD(0)), 2);
	udelay(5);
	write32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_UDCLK(bank)), 1 << field);
	udelay(5);
	write32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_UD(0)), 0);
	write32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_UDCLK(bank)), 0 << field);

	offset = BCM2836_GPIO(15);
	bank = offset / 32;
	field = (offset - 32 * bank);
	write32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_UD(0)), 2);
	udelay(5);
	write32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_UDCLK(bank)), 1 << field);
	udelay(5);
	write32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_UD(0)), 0);
	write32(phys_to_virt(BCM2836_GPIO_BASE + GPIO_UDCLK(bank)), 0 << field);

	write32(phys_to_virt(BCM2836_UART0_BASE + UART_CR), 0x0);
	bcm2836_uart_setup(B115200, DATA_BITS_8, PARITY_NONE, STOP_BITS_1);
	write32(phys_to_virt(BCM2836_UART0_BASE + UART_CR), (1 << 0) | (1 << 8) | (1 << 9));
}

int debug_put(const char * buf, int count)
{
	int i;

	for(i = 0; i < count; i++)
	{
		while( (read8(phys_to_virt(BCM2836_UART0_BASE + UART_FR)) & UART_FR_TXFF) );
		write8(phys_to_virt(BCM2836_UART0_BASE + UART_DATA), buf[i]);
	}
	return i;
}

int debug_put_string(const char * buf)
{
	return debug_put(buf, strlen(buf));
}

static void logger_uart0_init(void)
{
	write32(phys_to_virt(BCM2836_UART0_BASE + UART_CR), (1 << 0) | (1 << 8) | (1 << 9));
}

static void logger_uart0_exit(void)
{
}

static ssize_t logger_uart0_output(const char * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while( (read8(phys_to_virt(BCM2836_UART0_BASE + UART_FR)) & UART_FR_TXFF) );
		write8(phys_to_virt(BCM2836_UART0_BASE + UART_DATA), buf[i]);
	}
	return i;
}

static struct logger_t bcm2836_logger = {
	.name	= "logger-uart0",
	.init	= logger_uart0_init,
	.exit	= logger_uart0_exit,
	.output	= logger_uart0_output,
};

static __init void bcm2836_logger_init(void)
{
	if(register_logger(&bcm2836_logger))
		LOG("Register logger '%s'", bcm2836_logger.name);
	else
		LOG("Failed to register logger '%s'", bcm2836_logger.name);
}

static __exit void bcm2836_logger_exit(void)
{
	if(unregister_logger(&bcm2836_logger))
		LOG("Unregister logger '%s'", bcm2836_logger.name);
	else
		LOG("Failed to unregister logger '%s'", bcm2836_logger.name);
}

pure_initcall(bcm2836_logger_init);
pure_exitcall(bcm2836_logger_exit);
