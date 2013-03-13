/*
 * mpad-pmu.c
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
#include <stddef.h>
#include <io.h>
#include <mpad-pmu.h>
#include <s5pv210/reg-gpio.h>

static void iic_delay(u32_t loop)
{
	u32_t i = 0;

	while(loop--)
	{
		i++;
		i++;
		i++;
	}
}

static void iic_set_scl(u8_t flag)
{
	if(flag)
		writel(S5PV210_GPD1DAT, (readl(S5PV210_GPD1DAT) & ~(0x1<<1)) | (0x1<<1));
	else
		writel(S5PV210_GPD1DAT, (readl(S5PV210_GPD1DAT) & ~(0x1<<1)) | (0x0<<1));
}

static void iic_set_sda(u8_t flag)
{
	if(flag)
		writel(S5PV210_GPD1DAT, (readl(S5PV210_GPD1DAT) & ~(0x1<<0)) | (0x1<<0));
	else
		writel(S5PV210_GPD1DAT, (readl(S5PV210_GPD1DAT) & ~(0x1<<0)) | (0x0<<0));
}

static void iic_sda_dir(u8_t flag)
{
	/*
	 * 0: input
	 * 1: output
	 */
	if(flag)
		writel(S5PV210_GPD1CON, (readl(S5PV210_GPD1CON) & ~(0xf<<0)) | (0x1<<0));
	else
		writel(S5PV210_GPD1CON, (readl(S5PV210_GPD1CON) & ~(0xf<<0)) | (0x0<<0));
}

static u8_t iic_get_sda(void)
{
	writel(S5PV210_GPD1CON, (readl(S5PV210_GPD1CON) & ~(0xf<<0)) | (0x0<<0));

	if(readl(S5PV210_GPD1DAT) & (0x1<<0))
		return 1;
	else
		return 0;
}

static void iic_init(void)
{
	/*
	 * iic clk - output high level and pull up
	 */
	writel(S5PV210_GPD1CON, (readl(S5PV210_GPD1CON) & ~(0xf<<4)) | (0x1<<4));
	writel(S5PV210_GPD1PUD, (readl(S5PV210_GPD1PUD) & ~(0x3<<2)) | (0x2<<2));
	writel(S5PV210_GPD1DAT, (readl(S5PV210_GPD1DAT) & ~(0x1<<1)) | (0x1<<1));

	/*
	 * iic data - output high level and pull up
	 */
	writel(S5PV210_GPD1CON, (readl(S5PV210_GPD1CON) & ~(0xf<<0)) | (0x1<<0));
	writel(S5PV210_GPD1PUD, (readl(S5PV210_GPD1PUD) & ~(0x3<<0)) | (0x2<<0));
	writel(S5PV210_GPD1DAT, (readl(S5PV210_GPD1DAT) & ~(0x1<<0)) | (0x1<<0));

	iic_set_sda(1);
	iic_set_scl(1);
	iic_delay(10);
}

static void iic_start(void)
{
	iic_set_sda(1);
	iic_set_scl(1);
	iic_delay(5);

	iic_set_sda(0);
	iic_delay(10);
	iic_set_scl(0);
	iic_delay(10);
}

static void iic_stop(void)
{
	iic_set_scl(0);
	iic_set_sda(0);
	iic_delay(5);

	iic_set_scl(1);
	iic_delay(10);
	iic_set_sda(1);
	iic_delay(10);
}

static void iic_send_ack(void)
{
	iic_set_sda(0);
	iic_delay(10);
	iic_set_scl(0);
	iic_delay(10);

	iic_set_scl(1);
	iic_delay(30);
	iic_set_scl(0);
	iic_delay(20);
}

static void iic_send_not_ack(void)
{
	iic_set_sda(1);
	iic_delay(10);
	iic_set_scl(0);
	iic_delay(10);

	iic_set_scl(1);
	iic_delay(30);
	iic_set_scl(0);
	iic_delay(20);
}

static u8_t iic_recv_ack(void)
{
	u8_t ret = 0;
	u8_t i;

	iic_sda_dir(0);

	iic_set_scl(0);
	iic_delay(20);
	iic_set_scl(1);
	iic_delay(20);

	for(i = 0; i < 5; i++)
	{
		if(! iic_get_sda())
		{
			ret = 1;
			break;
		}
	}
	iic_delay(10);

	iic_set_scl(0);
	iic_delay(10);

	iic_set_sda(1);
	iic_sda_dir(1);

	return ret;
}

static u8_t iic_send_byte(u8_t c)
{
	u8_t i;

	for(i = 0; i < 8; i++)
	{
		iic_set_scl(0);
		iic_delay(10);
		iic_set_sda((c & (0x80 >> i)) ? 1 : 0);
		iic_delay(10);
		iic_set_scl(1);
		iic_delay(30);
		iic_set_scl(0);
		iic_delay(10);
	}

	return iic_recv_ack();
}

static u8_t iic_recv_byte(void)
{
	u8_t i;
	u8_t c = 0;

	iic_sda_dir(0);

	for (i = 0; i < 8; i++)
	{
		iic_set_scl(0);
		iic_delay(10);
		iic_set_scl(1);
		iic_delay(20);
		c |= (iic_get_sda() << (7 - i));
		iic_delay(10);
		iic_set_scl(0);
		iic_delay(20);
	}

	iic_sda_dir(1);
	return c;
}

static u8_t iic_write_byte(u8_t slave, u8_t reg, u8_t val)
{
	iic_start();

	if(!iic_send_byte(slave << 0x1))
		return 0;

	if(!iic_send_byte(reg))
		return 0;

	if(!iic_send_byte(val))
		return 0;

	iic_stop();
	return 1;
}

static u8_t iic_read_byte(u8_t slave, u8_t reg,  u8_t * val)
{
	iic_start();

	if (!iic_send_byte(slave << 0x1))
		return 0;

	if (!iic_send_byte(reg))
		return 0;

	iic_stop();
	iic_start();

	if (!iic_send_byte((slave << 0x1) | 0x1))
		return 0;

	*val = iic_recv_byte();

	iic_send_ack();

	iic_stop();
	return 1;
}

static u8_t iic_read_nbyte(u8_t slave, u8_t reg, u8_t * buf, u32_t cnt)
{
	u32_t i;

	iic_start();

	if (!iic_send_byte(slave << 0x1))
		return 0;

	if (!iic_send_byte(reg))
		return 0;

	iic_stop();
	iic_start();

	if (!iic_send_byte((slave << 0x1) | 0x1))
		return 0;

	for(i = 0; i < cnt; i++)
	{
		buf[i] = iic_recv_byte();
		if (i == cnt - 1)
			break;
		else
			iic_send_ack();
	}

	iic_send_not_ack();
	iic_stop();

	return cnt;
}

bool_t pmu_init(void)
{
	u8_t val;

	iic_init();

	if(pmu_read(0xb8, &val))
		pmu_write(0xb8, (val & 0x7f) | 0x80);

	if(pmu_read(0x82, &val))
		pmu_write(0x82, (val & 0xff) | 0xff);

	if(pmu_read(0x84, &val))
		pmu_write(0x84, (val & 0x3f) | 0xc0);

	if(pmu_read(0x31, &val))
		pmu_write(0x31, (val & 0xf8) | 0x00);

	if(pmu_read(0x33, &val))
		pmu_write(0x33, (val & 0xf0) | 0x0f);

	return TRUE;
}

bool_t pmu_write(u8_t reg, u8_t val)
{
	if(iic_write_byte(0x34, reg, val) == 0)
		return FALSE;
	return TRUE;
}

bool_t pmu_read(u8_t reg, u8_t * val)
{
	if(iic_read_byte(0x34, reg, val) == 0)
		return FALSE;
	return TRUE;
}

bool_t pmu_nread(u8_t reg, u8_t * buf, u32_t cnt)
{
	if(iic_read_nbyte(0x34, reg, buf, cnt) == 0)
		return FALSE;
	return TRUE;
}
