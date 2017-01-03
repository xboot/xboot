/*
 * rk3399-rstcon.c
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
#include <rk3399/reg-cru.h>
#include <rk3399/reg-pmu-cru.h>
#include <rk3399-srst.h>

void rk3399_soft_reset(int id)
{
	virtual_addr_t virt;
	int bank = id / 16;
	int offset = id % 16;

	if(bank < 21)
		virt = RK3399_CRU_BASE + CRU_SOFTRSTS_CON(bank);
	else if(bank < 23)
		virt = RK3399_PMU_CRU_BASE + PMU_CRU_SOFTRSTS_CON(bank - 21);
	else
		return;

	write32(virt, ((1 << offset) << 16) | (1 << offset));
	udelay(10);
	write32(virt, ((1 << offset) << 16) | (0 << offset));
}
