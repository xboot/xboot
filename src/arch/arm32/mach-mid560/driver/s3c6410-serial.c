/*
 * driver/s3c6410-serial.c
 *
 * s3c6410 on chip serial drivers.
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
#include <serial/serial.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/ioctl.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/resource.h>
#include <s3c6410/reg-serial.h>
#include <s3c6410/reg-gpio.h>


/*
 * default serial parameter.
 */
static struct serial_parameter uart_param[4] = {
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
static struct serial_info uart_info[4] = {
	[0] = {
		.name			= "uart0",
		.desc			= "s3c6410 onchip serial 0",
		.parameter		= &uart_param[0],
	},
	[1] = {
		.name			= "uart1",
		.desc			= "s3c6410 onchip serial 1",
		.parameter		= &uart_param[1],
	},
	[2] = {
		.name			= "uart2",
		.desc			= "s3c6410 onchip serial 2",
		.parameter		= &uart_param[2],
	},
	[3] = {
		.name			= "uart3",
		.desc			= "s3c6410 onchip serial 3",
		.parameter		= &uart_param[3],
	}

};

/*
 * common function for ioctl.
 */
static int s3c6410_ioctl(u32_t ch, int cmd, void * arg)
{
	const u32_t udivslot_code[16] = {0x0000, 0x0080, 0x0808, 0x0888,
									 0x2222, 0x4924, 0x4a52, 0x54aa,
									 0x5555, 0xd555, 0xd5d5, 0xddd5,
									 0xdddd, 0xdfdd, 0xdfdf, 0xffdf};
	u32_t baud, baud_div_reg, baud_divslot_reg;
	u8_t data_bit_reg, parity_reg, stop_bit_reg;
	u64_t uclk1;
	struct serial_parameter param;

	if((ch < 0) || (ch > 3))
		return -1;

	memcpy(&param, &uart_param[ch], sizeof(struct serial_parameter));

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
		parity_reg = 0x5;		break;
	case PARITY_ODD:
		parity_reg = 0x4;		break;
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

	if(clk_get_rate("uclk1", &uclk1))
	{
		baud_div_reg = (u32_t)(div64(uclk1, (baud * 16)) ) - 1;
		baud_divslot_reg = udivslot_code[( (u32_t)div64(mod64(uclk1, (baud*16)), baud) ) & 0xf];
	}
	else
		return -1;

	switch(ch)
	{
	case 0:
		writel(S3C6410_UBRDIV0, baud_div_reg);
		writel(S3C6410_UDIVSLOT0, baud_divslot_reg);
		writel(S3C6410_ULCON0, (data_bit_reg<<0 | stop_bit_reg <<2 | parity_reg<<3));
		break;
	case 1:
		writel(S3C6410_UBRDIV1, baud_div_reg);
		writel(S3C6410_UDIVSLOT1, baud_divslot_reg);
		writel(S3C6410_ULCON1, (data_bit_reg<<0 | stop_bit_reg <<2 | parity_reg<<3));
		break;
	case 2:
		writel(S3C6410_UBRDIV2, baud_div_reg);
		writel(S3C6410_UDIVSLOT2, baud_divslot_reg);
		writel(S3C6410_ULCON2, (data_bit_reg<<0 | stop_bit_reg <<2 | parity_reg<<3));
		break;
	case 3:
		writel(S3C6410_UBRDIV3, baud_div_reg);
		writel(S3C6410_UDIVSLOT3, baud_divslot_reg);
		writel(S3C6410_ULCON3, (data_bit_reg<<0 | stop_bit_reg <<2 | parity_reg<<3));
		break;
	default:
		return -1;
	}

	memcpy(&uart_param[ch], &param, sizeof(struct serial_parameter));

	return 0;
}

/* uart 0 */
static void s3c6410_uart0_init(void)
{
	/* configure GPA1, GPA0 for TXD0, RXD0 */
	writel(S3C6410_GPACON, (readl(S3C6410_GPACON) & ~(0xf<<0 | 0x0f<<4)) | (0x2<<0 | 0x2<<4));

	/* pull up GPA1 and GPA0 */
	writel(S3C6410_GPAPUD, (readl(S3C6410_GPAPUD) & ~(0x3<<0 | 0x03<<2)) | (0x2<<0 | 0x2<<2));

	/* configure clk source (uclk1), mode, etc */
	writel(S3C6410_UCON0, 0x00000c05);
	writel(S3C6410_UFCON0, 0x00000000);
	writel(S3C6410_UMON0, 0x00000000);

	/* configure uart parameter */
	s3c6410_ioctl( 0, IOCTL_WR_SERIAL_BAUD_RATE, (void *)(&(uart_param[0].baud_rate)) );
	s3c6410_ioctl( 0, IOCTL_WR_SERIAL_DATA_BITS, (void *)(&(uart_param[0].data_bit)) );
	s3c6410_ioctl( 0, IOCTL_WR_SERIAL_PARITY_BIT, (void *)(&(uart_param[0].parity)) );
	s3c6410_ioctl( 0, IOCTL_WR_SERIAL_STOP_BITS, (void *)(&(uart_param[0].stop_bit)) );
}

static void s3c6410_uart0_exit(void)
{
	return;
}

static ssize_t s3c6410_uart0_read(u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( (readl(S3C6410_UTRSTAT0) & S3C6410_UTRSTAT_RXDR) )
			buf[i] = readb(S3C6410_URXH0);
		else
			break;
	}
	return i;
}

static ssize_t s3c6410_uart0_write(const u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		/* wait for transmit buffer & shifter register empty */
		while( !(readl(S3C6410_UTRSTAT0) & S3C6410_UTRSTAT_TXE) );

		/* transmit a character */
		writeb(S3C6410_UTXH0, buf[i]);
	}
	return i;
}

static int s3c6410_uart0_ioctl(int cmd, void * arg)
{
	return (s3c6410_ioctl(0, cmd, arg));
}

/* uart 1 */
static void s3c6410_uart1_init(void)
{
	/* configure GPA5, GPA4 for TXD1, RXD1 */
	writel(S3C6410_GPACON, (readl(S3C6410_GPACON) & ~(0xf<<16 | 0x0f<<20)) | (0x2<<16 | 0x2<<20));

	/* pull up GPA5 and GPA4 */
	writel(S3C6410_GPAPUD, (readl(S3C6410_GPAPUD) & ~(0x3<<8 | 0x03<<10)) | (0x2<<8 | 0x2<<10));

	/* configure clk source (uclk1), mode, etc */
	writel(S3C6410_UCON1, 0x00000c05);
	writel(S3C6410_UFCON1, 0x00000000);
	writel(S3C6410_UMON1, 0x00000000);

	/* configure uart parameter */
	s3c6410_ioctl( 1, IOCTL_WR_SERIAL_BAUD_RATE, (void *)(&(uart_param[1].baud_rate)) );
	s3c6410_ioctl( 1, IOCTL_WR_SERIAL_DATA_BITS, (void *)(&(uart_param[1].data_bit)) );
	s3c6410_ioctl( 1, IOCTL_WR_SERIAL_PARITY_BIT, (void *)(&(uart_param[1].parity)) );
	s3c6410_ioctl( 1, IOCTL_WR_SERIAL_STOP_BITS, (void *)(&(uart_param[1].stop_bit)) );
}

static void s3c6410_uart1_exit(void)
{
	return;
}

static ssize_t s3c6410_uart1_read(u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( (readl(S3C6410_UTRSTAT1) & S3C6410_UTRSTAT_RXDR) )
			buf[i] = readb(S3C6410_URXH1);
		else
			break;
	}
	return i;
}

static ssize_t s3c6410_uart1_write(const u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		/* wait for transmit buffer & shifter register empty */
		while( !(readl(S3C6410_UTRSTAT1) & S3C6410_UTRSTAT_TXE) );

		/* transmit a character */
		writeb(S3C6410_UTXH1, buf[i]);
	}
	return i;
}

static int s3c6410_uart1_ioctl(int cmd, void * arg)
{
	return (s3c6410_ioctl(1, cmd, arg));
}

/* uart 2 */
static void s3c6410_uart2_init(void)
{
	/* configure GPB1, GPB0 for TXD2, RXD2 */
	writel(S3C6410_GPBCON, (readl(S3C6410_GPBCON) & ~(0xf<<0 | 0x0f<<4)) | (0x2<<0 | 0x2<<4));

	/* pull up GPB1 and GPB0 */
	writel(S3C6410_GPBPUD, (readl(S3C6410_GPBPUD) & ~(0x3<<0 | 0x03<<2)) | (0x2<<0 | 0x2<<2));

	/* configure clk source (uclk1), etc */
	writel(S3C6410_UCON2, 0x00000c05);
	writel(S3C6410_UFCON2, 0x00000000);

	/* configure uart parameter */
	s3c6410_ioctl( 2, IOCTL_WR_SERIAL_BAUD_RATE, (void *)(&(uart_param[2].baud_rate)) );
	s3c6410_ioctl( 2, IOCTL_WR_SERIAL_DATA_BITS, (void *)(&(uart_param[2].data_bit)) );
	s3c6410_ioctl( 2, IOCTL_WR_SERIAL_PARITY_BIT, (void *)(&(uart_param[2].parity)) );
	s3c6410_ioctl( 2, IOCTL_WR_SERIAL_STOP_BITS, (void *)(&(uart_param[2].stop_bit)) );
}

static void s3c6410_uart2_exit(void)
{
	return;
}

static ssize_t s3c6410_uart2_read(u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( (readl(S3C6410_UTRSTAT2) & S3C6410_UTRSTAT_RXDR) )
			buf[i] = readb(S3C6410_URXH2);
		else
			break;
	}
	return i;
}

static ssize_t s3c6410_uart2_write(const u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		/* wait for transmit buffer & shifter register empty */
		while( !(readl(S3C6410_UTRSTAT2) & S3C6410_UTRSTAT_TXE) );

		/* transmit a character */
		writeb(S3C6410_UTXH2, buf[i]);
	}
	return i;
}

static int s3c6410_uart2_ioctl(int cmd, void * arg)
{
	return (s3c6410_ioctl(2, cmd, arg));
}

/* uart 3 */
static void s3c6410_uart3_init(void)
{
	/* configure GPB3, GPB2 for TXD3, RXD3 */
	writel(S3C6410_GPBCON, (readl(S3C6410_GPBCON) & ~(0xf<<8 | 0x0f<<12)) | (0x2<<8 | 0x2<<12));

	/* pull up GPB3 and GPB2 */
	writel(S3C6410_GPBPUD, (readl(S3C6410_GPBPUD) & ~(0x3<<4 | 0x03<<6)) | (0x2<<4 | 0x2<<6));

	/* configure clk source (uclk1), etc */
	writel(S3C6410_UCON3, 0x00000c05);
	writel(S3C6410_UFCON3, 0x00000000);

	/* configure uart parameter */
	s3c6410_ioctl( 3, IOCTL_WR_SERIAL_BAUD_RATE, (void *)(&(uart_param[3].baud_rate)) );
	s3c6410_ioctl( 3, IOCTL_WR_SERIAL_DATA_BITS, (void *)(&(uart_param[3].data_bit)) );
	s3c6410_ioctl( 3, IOCTL_WR_SERIAL_PARITY_BIT, (void *)(&(uart_param[3].parity)) );
	s3c6410_ioctl( 3, IOCTL_WR_SERIAL_STOP_BITS, (void *)(&(uart_param[3].stop_bit)) );
}

static void s3c6410_uart3_exit(void)
{
	return;
}

static ssize_t s3c6410_uart3_read(u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( (readl(S3C6410_UTRSTAT3) & S3C6410_UTRSTAT_RXDR) )
			buf[i] = readb(S3C6410_URXH3);
		else
			break;
	}
	return i;
}

static ssize_t s3c6410_uart3_write(const u8_t * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		/* wait for transmit buffer & shifter register empty */
		while( !(readl(S3C6410_UTRSTAT3) & S3C6410_UTRSTAT_TXE) );

		/* transmit a character */
		writeb(S3C6410_UTXH3, buf[i]);
	}
	return i;
}

static int s3c6410_uart3_ioctl(int cmd, void * arg)
{
	return (s3c6410_ioctl(3, cmd, arg));
}

static struct serial_driver s3c6410_uart_driver[4] = {
	[0] = {
		.info	= &uart_info[0],
		.init	= s3c6410_uart0_init,
		.exit	= s3c6410_uart0_exit,
		.read	= s3c6410_uart0_read,
		.write	= s3c6410_uart0_write,
		.ioctl	= s3c6410_uart0_ioctl,
	},
	[1] = {
		.info	= &uart_info[1],
		.init	= s3c6410_uart1_init,
		.exit	= s3c6410_uart1_exit,
		.read	= s3c6410_uart1_read,
		.write	= s3c6410_uart1_write,
		.ioctl	= s3c6410_uart1_ioctl,
	},
	[2] = {
		.info	= &uart_info[2],
		.init	= s3c6410_uart2_init,
		.exit	= s3c6410_uart2_exit,
		.read	= s3c6410_uart2_read,
		.write	= s3c6410_uart2_write,
		.ioctl	= s3c6410_uart2_ioctl,
	},
	[3] = {
		.info	= &uart_info[3],
		.init	= s3c6410_uart3_init,
		.exit	= s3c6410_uart3_exit,
		.read	= s3c6410_uart3_read,
		.write	= s3c6410_uart3_write,
		.ioctl	= s3c6410_uart3_ioctl,
	}
};

static __init void s3c6410_serial_dev_init(void)
{
	struct serial_parameter * param;
	u32_t i;

	if(!clk_get_rate("uclk1", 0))
	{
		LOG_E("can't get the clock of \'uclk1\'");
		return;
	}

	/* register serial driver */
	for(i = 0; i < ARRAY_SIZE(s3c6410_uart_driver); i++)
	{
		param = (struct serial_parameter *)resource_get_data(s3c6410_uart_driver[i].info->name);
		if(param)
			memcpy(s3c6410_uart_driver[i].info->parameter, param, sizeof(struct serial_parameter));
		else
			LOG_W("can't get the resource of \'%s\', use default parameter", s3c6410_uart_driver[i].info->name);

		if(!register_serial(&s3c6410_uart_driver[i]))
			LOG_E("failed to register serial driver '%s'", s3c6410_uart_driver[i].info->name);
	}
}

static __exit void s3c6410_serial_dev_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(s3c6410_uart_driver); i++)
	{
		if(!unregister_serial(&s3c6410_uart_driver[i]))
			LOG_E("failed to unregister serial driver '%s'", s3c6410_uart_driver[i].info->name);
	}
}

device_initcall(s3c6410_serial_dev_init);
device_exitcall(s3c6410_serial_dev_exit);
