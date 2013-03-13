/*
 * driver/realview-serial.c
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
#include <types.h>
#include <string.h>
#include <div64.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/clk.h>
#include <xboot/ioctl.h>
#include <xboot/printk.h>
#include <xboot/resource.h>
#include <serial/serial.h>
#include <realview/reg-serial.h>


/*
 * default serial parameter.
 */
static struct serial_parameter serial_param[4] = {
	[0] = {
		.baud_rate		= B115200,
		.data_bit		= DATA_BITS_8,
		.parity			= PARITY_NONE,
		.stop_bit		= STOP_BITS_1,
	},
	[1] = {
		.baud_rate		= B115200,
		.data_bit		= DATA_BITS_8,
		.parity			= PARITY_NONE,
		.stop_bit		= STOP_BITS_1,
	},
	[2] = {
		.baud_rate		= B115200,
		.data_bit		= DATA_BITS_8,
		.parity			= PARITY_NONE,
		.stop_bit		= STOP_BITS_1,
	},
	[3] = {
		.baud_rate		= B115200,
		.data_bit		= DATA_BITS_8,
		.parity			= PARITY_NONE,
		.stop_bit		= STOP_BITS_1,
	}
};

/*
 * serial information.
 */
static struct serial_info serial_info[4] = {
	[0] = {
		.name			= "uart0",
		.desc			= "realview serial 0",
		.parameter		= &serial_param[0],
	},
	[1] = {
		.name			= "uart1",
		.desc			= "realview serial 1",
		.parameter		= &serial_param[1],
	},
	[2] = {
		.name			= "uart2",
		.desc			= "realview serial 2",
		.parameter		= &serial_param[2],
	},
	[3] = {
		.name			= "uart3",
		.desc			= "realview serial 3",
		.parameter		= &serial_param[3],
	}
};

/*
 * common function for ioctl.
 */
static int realview_serial_ioctl(u32_t ch, int cmd, void * arg)
{
	u32_t baud, divider, fraction;
	u32_t temp, remainder;
	u8_t data_bit_reg, parity_reg, stop_bit_reg;
	u64_t uclk;
	struct serial_parameter param;

	if((ch < 0) || (ch > 3))
		return -1;

	memcpy(&param, &serial_param[ch], sizeof(struct serial_parameter));

	switch(cmd)
	{
	case IOCTL_WR_SERIAL_BAUD_RATE:
		param.baud_rate = *((enum SERIAL_BAUD_RATE *)arg);
		break;

	case IOCTL_WR_SERIAL_DATA_BITS:
		param.data_bit = *((enum SERIAL_DATA_BITS *)arg);
		break;

	case IOCTL_WR_SERIAL_PARITY_BIT:
		param.parity = *((enum SERIAL_PARITY_BIT *)arg);
		break;

	case IOCTL_WR_SERIAL_STOP_BITS:
		param.stop_bit = *((enum SERIAL_STOP_BITS *)arg);
		break;

	case IOCTL_RD_SERIAL_BAUD_RATE:
		*((enum SERIAL_BAUD_RATE *)arg) = param.baud_rate;
		return 0;

	case IOCTL_RD_SERIAL_DATA_BITS:
		*((enum SERIAL_DATA_BITS *)arg) = param.data_bit;
		return 0;

	case IOCTL_RD_SERIAL_PARITY_BIT:
		*((enum SERIAL_PARITY_BIT *)arg) = param.parity;
		return 0;

	case IOCTL_RD_SERIAL_STOP_BITS:
		*((enum SERIAL_STOP_BITS *)arg) = param.stop_bit;
		return 0;

	default:
		return -1;
	}

	switch(param.baud_rate)
	{
	case B50:
		baud = 50;				break;
	case B75:
		baud = 75;				break;
	case B110:
		baud = 110;				break;
	case B134:
		baud = 134;				break;
	case B200:
		baud = 200;				break;
	case B300:
		baud = 300;				break;
	case B600:
		baud = 600;				break;
	case B1200:
		baud = 1200;			break;
	case B1800:
		baud = 1800;			break;
	case B2400:
		baud = 2400;			break;
	case B4800:
		baud = 4800;			break;
	case B9600:
		baud = 9600;			break;
	case B19200:
		baud = 19200;			break;
	case B38400:
		baud = 38400;			break;
	case B57600:
		baud = 57600;			break;
	case B76800:
		baud = 76800;			break;
	case B115200:
		baud = 115200;			break;
	case B230400:
		baud = 230400;			break;
	case B380400:
		baud = 380400;			break;
	case B460800:
		baud = 460800;			break;
	case B921600:
		baud = 921600;			break;
	default:
		return -1;
	}

	switch(param.data_bit)
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
		return -1;
	}

	switch(param.parity)
	{
	case PARITY_NONE:
		parity_reg = 0x0;		break;
	case PARITY_EVEN:
		parity_reg = 0x2;		break;
	case PARITY_ODD:
		parity_reg = 0x1;		break;
	default:
		return -1;
	}

	switch(param.stop_bit)
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

	if(! clk_get_rate("uclk", &uclk))
		return -1;

	/*
	 * IBRD = UART_CLK / (16 * BAUD_RATE)
	 * FBRD = ROUND((64 * MOD(UART_CLK, (16 * BAUD_RATE))) / (16 * BAUD_RATE))
	 */
	temp = 16 * baud;
	divider = (u32_t)div64(uclk, temp);
	remainder = (u32_t)mod64(uclk, temp);
	temp = (8 * remainder) / baud;
	fraction = (temp >> 1) + (temp & 1);

	switch(ch)
	{
	case 0:
		writel(REALVIEW_SERIAL0_IBRD, divider);
		writel(REALVIEW_SERIAL0_FBRD, fraction);
		writel(REALVIEW_SERIAL0_LCRH, REALVIEW_SERIAL_LCRH_FEN | (data_bit_reg<<5 | stop_bit_reg<<3 | parity_reg<<1));
		break;
	case 1:
		writel(REALVIEW_SERIAL1_IBRD, divider);
		writel(REALVIEW_SERIAL1_FBRD, fraction);
		writel(REALVIEW_SERIAL1_LCRH, REALVIEW_SERIAL_LCRH_FEN | (data_bit_reg<<5 | stop_bit_reg<<3 | parity_reg<<1));
		break;
	case 2:
		writel(REALVIEW_SERIAL2_IBRD, divider);
		writel(REALVIEW_SERIAL2_FBRD, fraction);
		writel(REALVIEW_SERIAL2_LCRH, REALVIEW_SERIAL_LCRH_FEN | (data_bit_reg<<5 | stop_bit_reg<<3 | parity_reg<<1));
		break;
	case 3:
		writel(REALVIEW_SERIAL3_IBRD, divider);
		writel(REALVIEW_SERIAL3_FBRD, fraction);
		writel(REALVIEW_SERIAL3_LCRH, REALVIEW_SERIAL_LCRH_FEN | (data_bit_reg<<5 | stop_bit_reg<<3 | parity_reg<<1));
		break;
	default:
		return -1;
	}

	memcpy(&serial_param[ch], &param, sizeof(struct serial_parameter));

	return 0;
}

/* serial 0 */
static void realview_serial0_init(void)
{
	/* disable everything */
	writel(REALVIEW_SERIAL0_CR, 0x0);

	/* configure uart parameter */
	realview_serial_ioctl( 0, IOCTL_WR_SERIAL_BAUD_RATE, (void *)(&(serial_param[0].baud_rate)) );
	realview_serial_ioctl( 0, IOCTL_WR_SERIAL_DATA_BITS, (void *)(&(serial_param[0].data_bit)) );
	realview_serial_ioctl( 0, IOCTL_WR_SERIAL_PARITY_BIT, (void *)(&(serial_param[0].parity)) );
	realview_serial_ioctl( 0, IOCTL_WR_SERIAL_STOP_BITS, (void *)(&(serial_param[0].stop_bit)) );

	/* enable the serial */
	writel(REALVIEW_SERIAL0_CR, REALVIEW_SERIAL_CR_UARTEN |	REALVIEW_SERIAL_CR_TXE | REALVIEW_SERIAL_CR_RXE);
}

static void realview_serial0_exit(void)
{
	return;
}

static ssize_t realview_serial0_read(u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( !(readb(REALVIEW_SERIAL0_FR) & REALVIEW_SERIAL_FR_RXFE) )
			buf[i] = readb(REALVIEW_SERIAL0_DATA);
		else
			break;
	}
	return i;
}

static ssize_t realview_serial0_write(const u8_t * buf, size_t count)
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

static int realview_serial0_ioctl(int cmd, void * arg)
{
	return (realview_serial_ioctl(0, cmd, arg));
}

/* serial 1 */
static void realview_serial1_init(void)
{
	/* disable everything */
	writel(REALVIEW_SERIAL1_CR, 0x0);

	/* configure uart parameter */
	realview_serial_ioctl( 1, IOCTL_WR_SERIAL_BAUD_RATE, (void *)(&(serial_param[1].baud_rate)) );
	realview_serial_ioctl( 1, IOCTL_WR_SERIAL_DATA_BITS, (void *)(&(serial_param[1].data_bit)) );
	realview_serial_ioctl( 1, IOCTL_WR_SERIAL_PARITY_BIT, (void *)(&(serial_param[1].parity)) );
	realview_serial_ioctl( 1, IOCTL_WR_SERIAL_STOP_BITS, (void *)(&(serial_param[1].stop_bit)) );

	/* enable the serial */
	writel(REALVIEW_SERIAL1_CR, REALVIEW_SERIAL_CR_UARTEN |	REALVIEW_SERIAL_CR_TXE | REALVIEW_SERIAL_CR_RXE);
}

static void realview_serial1_exit(void)
{
	return;
}

static ssize_t realview_serial1_read(u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( !(readb(REALVIEW_SERIAL1_FR) & REALVIEW_SERIAL_FR_RXFE) )
			buf[i] = readb(REALVIEW_SERIAL1_DATA);
		else
			break;
	}
	return i;
}

static ssize_t realview_serial1_write(const u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		/* wait until there is space in the fifo */
		while( (readb(REALVIEW_SERIAL1_FR) & REALVIEW_SERIAL_FR_TXFF) );

		/* transmit a character */
		writeb(REALVIEW_SERIAL1_DATA, buf[i]);
	}
	return i;
}

static int realview_serial1_ioctl(int cmd, void * arg)
{
	return (realview_serial_ioctl(1, cmd, arg));
}

/* serial 2 */
static void realview_serial2_init(void)
{
	/* disable everything */
	writel(REALVIEW_SERIAL2_CR, 0x0);

	/* configure uart parameter */
	realview_serial_ioctl( 2, IOCTL_WR_SERIAL_BAUD_RATE, (void *)(&(serial_param[2].baud_rate)) );
	realview_serial_ioctl( 2, IOCTL_WR_SERIAL_DATA_BITS, (void *)(&(serial_param[2].data_bit)) );
	realview_serial_ioctl( 2, IOCTL_WR_SERIAL_PARITY_BIT, (void *)(&(serial_param[2].parity)) );
	realview_serial_ioctl( 2, IOCTL_WR_SERIAL_STOP_BITS, (void *)(&(serial_param[2].stop_bit)) );

	/* enable the serial */
	writel(REALVIEW_SERIAL2_CR, REALVIEW_SERIAL_CR_UARTEN |	REALVIEW_SERIAL_CR_TXE | REALVIEW_SERIAL_CR_RXE);
}

static void realview_serial2_exit(void)
{
	return;
}

static ssize_t realview_serial2_read(u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( !(readb(REALVIEW_SERIAL2_FR) & REALVIEW_SERIAL_FR_RXFE) )
			buf[i] = readb(REALVIEW_SERIAL2_DATA);
		else
			break;
	}
	return i;
}

static ssize_t realview_serial2_write(const u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		/* wait until there is space in the fifo */
		while( (readb(REALVIEW_SERIAL2_FR) & REALVIEW_SERIAL_FR_TXFF) );

		/* transmit a character */
		writeb(REALVIEW_SERIAL2_DATA, buf[i]);
	}
	return i;
}

static int realview_serial2_ioctl(int cmd, void * arg)
{
	return (realview_serial_ioctl(2, cmd, arg));
}

/* serial 3 */
static void realview_serial3_init(void)
{
	/* disable everything */
	writel(REALVIEW_SERIAL3_CR, 0x0);

	/* configure uart parameter */
	realview_serial_ioctl( 3, IOCTL_WR_SERIAL_BAUD_RATE, (void *)(&(serial_param[3].baud_rate)) );
	realview_serial_ioctl( 3, IOCTL_WR_SERIAL_DATA_BITS, (void *)(&(serial_param[3].data_bit)) );
	realview_serial_ioctl( 3, IOCTL_WR_SERIAL_PARITY_BIT, (void *)(&(serial_param[3].parity)) );
	realview_serial_ioctl( 3, IOCTL_WR_SERIAL_STOP_BITS, (void *)(&(serial_param[3].stop_bit)) );

	/* enable the serial */
	writel(REALVIEW_SERIAL3_CR, REALVIEW_SERIAL_CR_UARTEN |	REALVIEW_SERIAL_CR_TXE | REALVIEW_SERIAL_CR_RXE);
}

static void realview_serial3_exit(void)
{
	return;
}

static ssize_t realview_serial3_read(u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( !(readb(REALVIEW_SERIAL3_FR) & REALVIEW_SERIAL_FR_RXFE) )
			buf[i] = readb(REALVIEW_SERIAL3_DATA);
		else
			break;
	}
	return i;
}

static ssize_t realview_serial3_write(const u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		/* wait until there is space in the fifo */
		while( (readb(REALVIEW_SERIAL3_FR) & REALVIEW_SERIAL_FR_TXFF) );

		/* transmit a character */
		writeb(REALVIEW_SERIAL3_DATA, buf[i]);
	}
	return i;
}

static int realview_serial3_ioctl(int cmd, void * arg)
{
	return (realview_serial_ioctl(3, cmd, arg));
}

static struct serial_driver realview_serial_driver[4] = {
	[0] = {
		.info	= &serial_info[0],
		.init	= realview_serial0_init,
		.exit	= realview_serial0_exit,
		.read	= realview_serial0_read,
		.write	= realview_serial0_write,
		.ioctl	= realview_serial0_ioctl,
	},
	[1] = {
		.info	= &serial_info[1],
		.init	= realview_serial1_init,
		.exit	= realview_serial1_exit,
		.read	= realview_serial1_read,
		.write	= realview_serial1_write,
		.ioctl	= realview_serial1_ioctl,
	},
	[2] = {
		.info	= &serial_info[2],
		.init	= realview_serial2_init,
		.exit	= realview_serial2_exit,
		.read	= realview_serial2_read,
		.write	= realview_serial2_write,
		.ioctl	= realview_serial2_ioctl,
	},
	[3] = {
		.info	= &serial_info[3],
		.init	= realview_serial3_init,
		.exit	= realview_serial3_exit,
		.read	= realview_serial3_read,
		.write	= realview_serial3_write,
		.ioctl	= realview_serial3_ioctl,
	}
};

static __init void realview_serial_dev_init(void)
{
	struct serial_parameter * param;
	u32_t i;

	if(!clk_get_rate("uclk", 0))
	{
		LOG_E("can't get the clock of \'uclk\'");
		return;
	}

	/* register serial driver */
	for(i = 0; i < ARRAY_SIZE(realview_serial_driver); i++)
	{
		param = (struct serial_parameter *)resource_get_data(realview_serial_driver[i].info->name);
		if(param)
			memcpy(realview_serial_driver[i].info->parameter, param, sizeof(struct serial_parameter));
		else
			LOG_W("can't get the resource of \'%s\', use default parameter", realview_serial_driver[i].info->name);

		if(!register_serial(&realview_serial_driver[i]))
			LOG_E("failed to register serial driver '%s'", realview_serial_driver[i].info->name);
	}
}

static __exit void realview_serial_dev_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(realview_serial_driver); i++)
	{
		if(!unregister_serial(&realview_serial_driver[i]))
			LOG_E("failed to unregister serial driver '%s'", realview_serial_driver[i].info->name);
	}
}

device_initcall(realview_serial_dev_init);
device_exitcall(realview_serial_dev_exit);
