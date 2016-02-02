/*
 * bcm2836-mbox.c
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
#include <bcm2836/reg-mbox.h>
#include <bcm2836-mbox.h>

void bcm2836_mbox_write(int channel, int value)
{
	virtual_addr_t virt = phys_to_virt(BCM2836_MBOX_BASE);

    value = (value & ~0xf) | (channel & 0xf);
	while((read32(virt + MBOX_STATUS) & (1 << 31)) != 0);
	write32(virt + MBOX_WRITE, value);
}

int bcm2836_mbox_read(int channel)
{
	virtual_addr_t virt = phys_to_virt(BCM2836_MBOX_BASE);
    int value;

    channel &= 0xf;
    do {
    	while((read32(virt + MBOX_STATUS) & (1 << 30)) != 0);
    	value = read32(virt + MBOX_READ);
    } while ((value & 0xf) != channel);

    return value >> 4;
}
