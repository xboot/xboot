/*
 * bcm2836-aux.c
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

#include <bcm2836-aux.h>

void bcm2836_aux_enable(int id)
{
	virtual_addr_t virt = phys_to_virt(BCM2836_AUX_BASE);
	u32_t val;

	val = read32(virt + AUX_ENB);
	val &= ~(0x1 << id);
	val |= 1 << id;
	write32(virt + AUX_ENB, val);
}

void bcm2836_aux_disable(int id)
{
	virtual_addr_t virt = phys_to_virt(BCM2836_AUX_BASE);
	u32_t val;

	val = read32(virt + AUX_ENB);
	val &= ~(0x1 << id);
	val |= 0 << id;
	write32(virt + AUX_ENB, val);
}

int bcm2836_aux_irq_status(int id)
{
	virtual_addr_t virt = phys_to_virt(BCM2836_AUX_BASE);
	return (read32(virt + AUX_IRQ) & (0x1 << id)) ? 1 : 0;
}
