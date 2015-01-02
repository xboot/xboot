/*
 * bus/s5pv210-uart.c
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
#include <s5pv210-uart.h>

static bool_t s5pv210_uart_setup(struct uart_t * uart, enum baud_rate_t baud, enum data_bits_t data, enum parity_bits_t parity, enum stop_bits_t stop)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct s5pv210_uart_data_t * dat = (struct s5pv210_uart_data_t *)res->data;
	const u32_t udivslot_code[16] = {0x0000, 0x0080, 0x0808, 0x0888,
									 0x2222, 0x4924, 0x4a52, 0x54aa,
									 0x5555, 0xd555, 0xd5d5, 0xddd5,
									 0xdddd, 0xdfdd, 0xdfdf, 0xffdf};
	u32_t ibaud, baud_div_reg, baud_divslot_reg;
	u8_t data_bit_reg, parity_reg, stop_bit_reg;
	u64_t pclk;

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
		data_bit_reg = 0x0;		break;
	case DATA_BITS_6:
		data_bit_reg = 0x1;		break;
	case DATA_BITS_7:
		data_bit_reg = 0x2;		break;
	case DATA_BITS_8:
		data_bit_reg = 0x3;		break;
	default:
		return FALSE;
	}

	switch(parity)
	{
	case PARITY_NONE:
		parity_reg = 0x0;		break;
	case PARITY_EVEN:
		parity_reg = 0x5;		break;
	case PARITY_ODD:
		parity_reg = 0x4;		break;
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
		return FALSE;
	}

	pclk = clk_get_rate("psys-pclk");
	if(!pclk)
		return FALSE;

	baud_div_reg = (u32_t)((pclk / (ibaud * 16)) ) - 1;
	baud_divslot_reg = udivslot_code[( (u32_t)((pclk % (ibaud*16)) / ibaud) ) & 0xf];

	writel(dat->regbase + S5PV210_UBRDIV, baud_div_reg);
	writel(dat->regbase + S5PV210_UDIVSLOT, baud_divslot_reg);
	writel(dat->regbase + S5PV210_ULCON, (data_bit_reg<<0 | stop_bit_reg <<2 | parity_reg<<3));

	return TRUE;
}

static void s5pv210_uart_init(struct uart_t * uart)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct s5pv210_uart_data_t * dat = (struct s5pv210_uart_data_t *)res->data;

	switch(res->id)
	{
	case 0:
		clk_enable("psys-pclk");

		/* Configure GPA01, GPA00 for TXD0, RXD0 and pull up */
		gpio_set_cfg(S5PV210_GPA0(1), 0x2);
		gpio_set_cfg(S5PV210_GPA0(0), 0x2);
		gpio_set_pull(S5PV210_GPA0(1), GPIO_PULL_UP);
		gpio_set_pull(S5PV210_GPA0(0), GPIO_PULL_UP);

		writel(dat->regbase + S5PV210_UCON, 0x00000005);
		writel(dat->regbase + S5PV210_UFCON, 0x00000777);
		writel(dat->regbase + S5PV210_UMON, 0x00000000);
		break;

	case 1:
		clk_enable("psys-pclk");

		/* Configure GPA05, GPA04 for TXD1, RXD1 */
		gpio_set_cfg(S5PV210_GPA0(5), 0x2);
		gpio_set_cfg(S5PV210_GPA0(4), 0x2);
		gpio_set_pull(S5PV210_GPA0(5), GPIO_PULL_UP);
		gpio_set_pull(S5PV210_GPA0(4), GPIO_PULL_UP);

		writel(dat->regbase + S5PV210_UCON, 0x00000005);
		writel(dat->regbase + S5PV210_UFCON, 0x00000777);
		writel(dat->regbase + S5PV210_UMON, 0x00000000);
		break;

	case 2:
		clk_enable("psys-pclk");

		/* Configure GPA11, GPA10 for TXD2, RXD2 */
		gpio_set_cfg(S5PV210_GPA1(1), 0x2);
		gpio_set_cfg(S5PV210_GPA1(0), 0x2);
		gpio_set_pull(S5PV210_GPA1(1), GPIO_PULL_UP);
		gpio_set_pull(S5PV210_GPA1(0), GPIO_PULL_UP);

		writel(dat->regbase + S5PV210_UCON, 0x00000005);
		writel(dat->regbase + S5PV210_UFCON, 0x00000777);
		break;

	case 3:
		clk_enable("psys-pclk");

		/* Configure GPA13, GPA12 for TXD3, RXD3 */
		gpio_set_cfg(S5PV210_GPA1(3), 0x2);
		gpio_set_cfg(S5PV210_GPA1(2), 0x2);
		gpio_set_pull(S5PV210_GPA1(3), GPIO_PULL_UP);
		gpio_set_pull(S5PV210_GPA1(2), GPIO_PULL_UP);

		writel(dat->regbase + S5PV210_UCON, 0x00000005);
		writel(dat->regbase + S5PV210_UFCON, 0x00000777);
		break;

	default:
		return;
	}

	s5pv210_uart_setup(uart, dat->baud, dat->data, dat->parity, dat->stop);
}

static void s5pv210_uart_exit(struct uart_t * uart)
{
	struct resource_t * res = (struct resource_t *)uart->priv;

	switch(res->id)
	{
	case 0:
		clk_disable("psys-pclk");
		break;
	case 1:
		clk_disable("psys-pclk");
		break;
	case 2:
		clk_disable("psys-pclk");
		break;
	case 3:
		clk_disable("psys-pclk");
		break;
	default:
		break;
	}
}

static ssize_t s5pv210_uart_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct s5pv210_uart_data_t * dat = (struct s5pv210_uart_data_t *)res->data;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( (readl(dat->regbase + S5PV210_UTRSTAT) & S5PV210_UTRSTAT_RXDR) )
			buf[i] = readb(dat->regbase + S5PV210_URXH);
		else
			break;
	}

	return i;
}

static ssize_t s5pv210_uart_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct s5pv210_uart_data_t * dat = (struct s5pv210_uart_data_t *)res->data;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while( !(readl(dat->regbase + S5PV210_UTRSTAT) & S5PV210_UTRSTAT_TXFE) );
		writeb(dat->regbase + S5PV210_UTXH, buf[i]);
	}

	return i;
}

static bool_t s5pv210_register_bus_uart(struct resource_t * res)
{
	struct uart_t * uart;
	char name[64];

	uart = malloc(sizeof(struct uart_t));
	if(!uart)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	uart->name = strdup(name);
	uart->init = s5pv210_uart_init;
	uart->exit = s5pv210_uart_exit;
	uart->read = s5pv210_uart_read;
	uart->write = s5pv210_uart_write;
	uart->setup = s5pv210_uart_setup;
	uart->priv = res;

	if(register_bus_uart(uart))
		return TRUE;

	free(uart->name);
	free(uart);
	return FALSE;
}

static bool_t s5pv210_unregister_bus_uart(struct resource_t * res)
{
	struct uart_t * uart;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	uart = search_bus_uart(name);
	if(!uart)
		return FALSE;

	if(!unregister_bus_uart(uart))
		return FALSE;

	free(uart->name);
	free(uart);
	return TRUE;
}

static __init void s5pv210_bus_uart_init(void)
{
	resource_for_each_with_name("s5pv210-uart", s5pv210_register_bus_uart);
}

static __exit void s5pv210_bus_uart_exit(void)
{
	resource_for_each_with_name("s5pv210-uart", s5pv210_unregister_bus_uart);
}

bus_initcall(s5pv210_bus_uart_init);
bus_exitcall(s5pv210_bus_uart_exit);
