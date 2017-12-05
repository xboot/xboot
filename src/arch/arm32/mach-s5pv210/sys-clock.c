/*
 * sys-clock.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <s5pv210/reg-clk.h>

#define DIV0_VAL	((0 << 0) | (4 << 4) | (4 << 8) | (1 << 12) | (3 << 16) | (1 << 20) | (4 << 24) | (1 << 28))
#define DIV1_VAL	((1 << 16) | (1 << 12) | (1 << 8) | (1 << 4))
#define DIV2_VAL	((1 << 0))

#define APLL_VAL	((1 << 31) | (0x7d << 16) | (0x3 << 8) | (0x1))
#define MPLL_VAL	((1 << 31) | (0x29b << 16) | (0xc << 8) | (0x1))
#define EPLL_VAL	((1 << 31) | (0x60 << 16) | (0x6 << 8) | (0x2))
#define VPLL_VAL	((1 << 31) | (0x6c << 16) | (0x6 << 8) | (0x3))

static inline void sdelay(int loops)
{
	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0"(loops));
}

void sys_clock_init(void)
{
	write32(S5PV210_CLK_BASE + CLK_APLL_LOCK, 0xfff);
	write32(S5PV210_CLK_BASE + CLK_MPLL_LOCK, 0xfff);
	write32(S5PV210_CLK_BASE + CLK_EPLL_LOCK, 0xfff);
	write32(S5PV210_CLK_BASE + CLK_VPLL_LOCK, 0xfff);

	write32(S5PV210_CLK_BASE + CLK_DIV0, DIV0_VAL);
	write32(S5PV210_CLK_BASE + CLK_DIV1, DIV1_VAL);
	write32(S5PV210_CLK_BASE + CLK_DIV2, DIV2_VAL);

	write32(S5PV210_CLK_BASE + CLK_APLL_CON0, APLL_VAL);
	write32(S5PV210_CLK_BASE + CLK_MPLL_CON, MPLL_VAL);
	write32(S5PV210_CLK_BASE + CLK_EPLL_CON0, EPLL_VAL);
	write32(S5PV210_CLK_BASE + CLK_VPLL_CON, VPLL_VAL);

	sdelay(50000);
}
