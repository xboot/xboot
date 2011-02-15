/*
 * mpad-pmu.c
 *
 * advanced pmu (act8937) for samsung s5pc100, s5pc110 and s5pv210 processors
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <io.h>
#include <mpad-pmu.h>
#include <s5pv210/reg-gpio.h>

static void iic_delay(void)
{
	x_u32 loop = 100;
	x_u32 base;

	while(loop--)
	{
		base += loop;
	}
}

static void iic_set_scl(x_u8 flag)
{
	writel(S5PV210_GPD1CON, (readl(S5PV210_GPD1CON) & ~(0xf<<4)) | (0x1<<4));
	writel(S5PV210_GPD1PUD, (readl(S5PV210_GPD1PUD) & ~(0x3<<2)) | (0x2<<2));

	if(flag)
		writel(S5PV210_GPD1DAT, (readl(S5PV210_GPD1DAT) & ~(0x1<<1)) | (0x1<<1));
	else
		writel(S5PV210_GPD1DAT, (readl(S5PV210_GPD1DAT) & ~(0x1<<1)) | (0x0<<1));
}

static void iic_set_sda(x_u8 flag)
{
	writel(S5PV210_GPD1CON, (readl(S5PV210_GPD1CON) & ~(0xf<<0)) | (0x1<<0));
	writel(S5PV210_GPD1PUD, (readl(S5PV210_GPD1PUD) & ~(0x3<<0)) | (0x2<<0));

	if(flag)
		writel(S5PV210_GPD1DAT, (readl(S5PV210_GPD1DAT) & ~(0x1<<0)) | (0x1<<0));
	else
		writel(S5PV210_GPD1DAT, (readl(S5PV210_GPD1DAT) & ~(0x1<<0)) | (0x0<<0));
}

static x_u8 iic_get_sda(void)
{
	writel(S5PV210_GPD1CON, (readl(S5PV210_GPD1CON) & ~(0xf<<0)) | (0x0<<0));
	writel(S5PV210_GPD1PUD, (readl(S5PV210_GPD1PUD) & ~(0x3<<0)) | (0x2<<0));

	if(readl(S5PV210_GPD1DAT) & (0x1<<0))
		return 1;
	else
		return 0;
}

static void iic_start(void)
{
	iic_set_sda(1);
	iic_set_scl(1);
	iic_delay();
	iic_set_sda(0);
	iic_delay();
	iic_set_scl(0);
}

static void iic_stop(void)
{
	iic_set_scl(0);
	iic_set_sda(0);
	iic_delay();
	iic_set_scl(1);
	iic_delay();
	iic_set_sda(1);
}

static x_u8 iic_wait_ack(void)
{
	x_u8 time = 10;

	iic_set_sda(1);
	iic_delay();
	iic_set_scl(1);
	iic_delay();

	while(iic_get_sda())
	{
		time--;
		if (!time)
		{
			iic_stop();
			return 0;
		}
		iic_delay();
	}

	iic_set_scl(0);
	return 1;
}

static void iic_send_ack(void)
{
	iic_set_sda(0);
	iic_delay();
	iic_set_scl(1);
	iic_delay();
	iic_set_scl(0);
}

static void iic_send_not_ack(void)
{
	iic_set_sda(1);
	iic_delay();
	iic_set_scl(1);
	iic_delay();
	iic_set_scl(0);
}

static void iic_send_byte(x_u8 ch)
{
	x_u8 i = 8;

	while(i--)
	{
		iic_set_scl(0);
		iic_delay();

		if(ch & 0x80)
			iic_set_sda(1);
		else
			iic_set_sda(0);
		ch <<= 1;
		iic_delay();
		iic_set_scl(1);
		iic_delay();
	}
	iic_set_scl(0);
}

static x_u8 iic_recv_byte(void)
{
	x_u8 i = 8;
	x_u8 data = 0;

	iic_set_sda(1);
	while(i--)
	{
		data <<= 1;
		iic_set_scl(0);
		iic_delay();
		iic_set_scl(1);
		iic_delay();
		data |= iic_get_sda();
	}
	iic_set_scl(0);

	return data;
}

x_s32 iic_write_byte(x_u8 slave, x_u8 reg, x_u8 value)
{
	iic_start();

	iic_send_byte(slave << 0x1);
	if(! iic_wait_ack())
		return 0;

	iic_send_byte(reg);
	if(! iic_wait_ack())
		return 0;

	iic_send_byte(value);
	if(! iic_wait_ack())
		return 0;

	iic_stop();

	return 1;
}

x_s32 iic_write_nbyte(x_u8 slave, x_u8 reg, x_u8 * buf, x_s32 count)
{
	x_s32 i;

	iic_start();

	iic_send_byte(slave << 0x1);
	if(! iic_wait_ack())
		return 0;

	iic_send_byte(reg);
	if(! iic_wait_ack())
		return 0;

	for(i = 0; i < count; i++)
	{
		iic_send_byte(buf[i]);
		if(! iic_wait_ack())
			return i;
	}

	iic_stop();

	return i;
}

x_s32 iic_read_byte(x_u8 slave, x_u8 reg,  x_u8 * value)
{
	iic_start();

	iic_send_byte(slave << 0x1);
	if(! iic_wait_ack())
		return 0;

	iic_send_byte(reg);
	if(! iic_wait_ack())
		return 0;

	iic_start();

	iic_send_byte((slave << 0x1) | 0x01);
	if(! iic_wait_ack())
		return 0;

	*value = iic_recv_byte();
	iic_send_not_ack();

	iic_stop();

	return 1;
}

x_s32 iic_read_nbyte(x_u8 slave, x_u8 reg, x_u8 * buf, x_s32 count)
{
	x_s32 i;

	iic_start();

	iic_send_byte(slave << 0x1);
	if(! iic_wait_ack())
		return 0;

	iic_send_byte(reg);
	if(! iic_wait_ack())
		return 0;

	iic_start();

	iic_send_byte((slave << 0x1) | 0x01);
	if(! iic_wait_ack())
		return 0;

	for(i = 0; i < count; i++)
	{
		buf[i] = iic_recv_byte();
		if(i == count - 1)
			break;
		else
			iic_send_ack();
	}

	iic_send_not_ack();
	iic_stop();

	return count;
}

x_bool pmu_write(x_u8 reg, x_u8 value)
{
	if(iic_write_byte(0x5b, reg, value) == 0)
		return FALSE;
	return TRUE;
}

x_bool pmu_read(x_u8 reg, x_u8 * value)
{
	if(iic_read_byte(0x5b, reg, value) == 0)
		return FALSE;
	return TRUE;
}
