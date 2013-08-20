/*
 * bus/realview-uart.c
 *
 * realview serial drivers, the primecell pl011 uarts.
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
#include <realview-uart.h>

static bool_t realview_uart_setup(struct uart_t * uart, enum baud_rate_t baud, enum data_bits_t data, enum parity_bits_t parity, enum stop_bits_t stop)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct realview_uart_data_t * dat = (struct realview_uart_data_t *)res->data;
	u32_t ibaud, divider, fraction;
	u32_t temp, remainder;
	u8_t data_bit_reg, parity_reg, stop_bit_reg;
	u64_t uclk;

	if(! clk_get_rate("uclk", &uclk))
		return FALSE;

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
		data_bit_reg = 0x0;
		break;
	case DATA_BITS_6:
		data_bit_reg = 0x1;
		break;
	case DATA_BITS_7:
		data_bit_reg = 0x2;
		break;
	case DATA_BITS_8:
		data_bit_reg = 0x3;
		break;
	default:
		return FALSE;
	}

	switch(parity)
	{
	case PARITY_NONE:
		parity_reg = 0x0;
		break;
	case PARITY_EVEN:
		parity_reg = 0x2;
		break;
	case PARITY_ODD:
		parity_reg = 0x1;
		break;
	default:
		return FALSE;
	}

	switch(stop)
	{
	case STOP_BITS_1:
		stop_bit_reg = 0;		break;
	case STOP_BITS_1_5:
		return -1;
	case STOP_BITS_2:
		stop_bit_reg = 1;		break;
	default:
		return -1;
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

	writel(dat->regbase + REALVIEW_UART_OFFSET_IBRD, divider);
	writel(dat->regbase + REALVIEW_UART_OFFSET_FBRD, fraction);
	writel(dat->regbase + REALVIEW_UART_OFFSET_LCRH, REALVIEW_UART_LCRH_FEN | (data_bit_reg<<5 | stop_bit_reg<<3 | parity_reg<<1));

	return TRUE;
}

static void realview_uart_init(struct uart_t * uart)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct realview_uart_data_t * dat = (struct realview_uart_data_t *)res->data;

	/* Disable everything */
	writel(dat->regbase + REALVIEW_UART_OFFSET_CR, 0x0);

	/* Configure uart */
	realview_uart_setup(uart, dat->baud, dat->data, dat->parity, dat->stop);

	/* Enable uart */
	writel(dat->regbase + REALVIEW_UART_OFFSET_CR, REALVIEW_UART_CR_UARTEN | REALVIEW_UART_CR_TXE | REALVIEW_UART_CR_RXE);
}

static void realview_uart_exit(struct uart_t * uart)
{
}

static ssize_t realview_uart_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct realview_uart_data_t * dat = (struct realview_uart_data_t *)res->data;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( !(readb(dat->regbase + REALVIEW_UART_OFFSET_FR) & REALVIEW_UART_FR_RXFE) )
			buf[i] = readb(dat->regbase + REALVIEW_UART_OFFSET_DATA);
		else
			break;
	}

	return i;
}

static ssize_t realview_uart_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct realview_uart_data_t * dat = (struct realview_uart_data_t *)res->data;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while( (readb(dat->regbase + REALVIEW_UART_OFFSET_FR) & REALVIEW_UART_FR_TXFF) );
		writeb(dat->regbase + REALVIEW_UART_OFFSET_DATA, buf[i]);
	}

	return i;
}

static bool_t realview_register_bus_uart(struct resource_t * res)
{
	struct uart_t * uart;
	char name[32 + 1];

	if(!clk_get_rate("uclk", 0))
	{
		LOG("Can't get clock source 'uclk'");
		return FALSE;
	}

	uart = malloc(sizeof(struct uart_t));
	if(!uart)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);
	uart->name = name;
	uart->init = realview_uart_init;
	uart->exit = realview_uart_exit;
	uart->read = realview_uart_read;
	uart->write = realview_uart_write;
	uart->setup = realview_uart_setup;
	uart->priv = res;

	if(!register_bus_uart(uart))
		return FALSE;
	return TRUE;
}

static bool_t realview_unregister_bus_uart(struct resource_t * res)
{
	struct uart_t * uart;
	char name[32 + 1];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	uart = search_bus_uart(name);
	if(!uart)
		return FALSE;

	return unregister_bus_uart(uart);
}

static __init void realview_bus_uart_init(void)
{
	resource_callback_with_name("uart", realview_register_bus_uart);
}

static __exit void realview_bus_uart_exit(void)
{
	resource_callback_with_name("uart", realview_unregister_bus_uart);
}

device_initcall(realview_bus_uart_init);
device_exitcall(realview_bus_uart_exit);
