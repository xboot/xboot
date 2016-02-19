/*
 * s5p6818-rstcon.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
#include <s5p6818/reg-sys.h>
#include <s5p6818-rstcon.h>

static void s5p6818_ip_setrst(int id, int reset)
{
	virtual_addr_t virt = phys_to_virt(S5P6818_SYS_IP_RSTCON0);
	u32_t val;

	if(id < 32)
		virt = phys_to_virt(S5P6818_SYS_IP_RSTCON0);
	else if(id < 64)
		virt = phys_to_virt(S5P6818_SYS_IP_RSTCON1);
	else if(id < 96)
		virt = phys_to_virt(S5P6818_SYS_IP_RSTCON2);
	else
		return;

	val = read32(virt);
	val &= ~(0x1 << (id & 0x1f));
	val |= (reset ? 1 : 0) << (id & 0x1f);
	write32(virt, val);
}

static int s5p6818_ip_getrst(int id)
{
	virtual_addr_t virt = phys_to_virt(S5P6818_SYS_IP_RSTCON0);
	u32_t val;

	if(id < 32)
		virt = phys_to_virt(S5P6818_SYS_IP_RSTCON0);
	else if(id < 64)
		virt = phys_to_virt(S5P6818_SYS_IP_RSTCON1);
	else if(id < 96)
		virt = phys_to_virt(S5P6818_SYS_IP_RSTCON2);
	else
		return 1;

	val = read32(virt);
	return (val >> (id & 0x1f)) & 0x1;
}

void s5p6818_ip_reset(int id, int force)
{
	if(force || !s5p6818_ip_getrst(id))
	{
		s5p6818_ip_setrst(id, 0);
		udelay(10);
		s5p6818_ip_setrst(id, 1);
	}
}
