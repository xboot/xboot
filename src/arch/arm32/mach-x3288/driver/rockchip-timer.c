/*
 * driver/rockchip-timer.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <clk/clk.h>
#include <rockchip-timer.h>

enum {
	TIMER_LOAD_COUNT0		= 0x00,
	TIMER_LOAD_COUNT1		= 0x04,
	TIMER_CURRENT_VALUE0	= 0x08,
	TIMER_CURRENT_VALUE1	= 0x0c,
	TIMER_CONTROL_REG		= 0x10,
	TIMER_INT_STATUS		= 0x18,
};

void rockchip_timer_start(virtual_addr_t virt, int irqon, int oneshot)
{
	u32_t val = ((irqon ? 1 : 0) << 2) | ((oneshot ? 1 : 0) << 1) | (1 << 0);
	write32(virt + TIMER_CONTROL_REG, val);
}

void rockchip_timer_stop(virtual_addr_t virt)
{
	write32(virt + TIMER_CONTROL_REG, 0);
}

void rockchip_timer_count(virtual_addr_t virt, u64_t cnt)
{
	u32_t upper = (u32_t)((cnt >> 32) & 0xffffffff);
	u32_t lower = (u32_t)((cnt >> 0) & 0xffffffff);

	write32(virt + TIMER_LOAD_COUNT1, upper);
	write32(virt + TIMER_LOAD_COUNT0, lower);
}

u32_t rockchip_timer_read32(virtual_addr_t virt)
{
	return read32(virt + TIMER_CURRENT_VALUE0);
}

u64_t rockchip_timer_read64(virtual_addr_t virt)
{
	u32_t upper, lower;

	do {
		upper = read32(virt + TIMER_CURRENT_VALUE1);
		lower = read32(virt + TIMER_CURRENT_VALUE0);
	} while (upper != read32(virt + TIMER_CURRENT_VALUE1));

	return ((u64_t)upper << 32) | lower;
}

void rockchip_timer_irq_clear(virtual_addr_t virt)
{
	write32(virt + TIMER_INT_STATUS, 0x1);
}
