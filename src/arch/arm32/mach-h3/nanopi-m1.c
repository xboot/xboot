/*
 * nanopi-m1.c
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
#include <mmu.h>

static const struct mmap_t mach_map[] = {
	{"ram",  0x40000000, 0x40000000, SZ_128M, MAP_TYPE_CB},
	{"dma",  0x48000000, 0x48000000, SZ_128M, MAP_TYPE_NCNB},
	{"heap", 0x50000000, 0x50000000, SZ_256M, MAP_TYPE_CB},
	{ 0 },
};

static u32_t sram_read_id(virtual_addr_t virt)
{
	u32_t id;

	write32(virt, read32(virt) | (1 << 15));
	id = read32(virt) >> 16;
	write32(virt, read32(virt) & ~(1 << 15));
	return id;
}

static u32_t sid_read_key(virtual_addr_t virt, int offset)
{
	u32_t val;

	val = read32(virt + 0x40);
	val &= ~(((0x1ff) << 16) | 0x3);
	val |= (offset << 16);
	write32(virt + 0x40, val);

	val &= ~(((0xff) << 8) | 0x3);
	val |= (0xac << 8) | 0x2;
	write32(virt + 0x40, val);

	while(read32(virt + 0x40) & (1 << 1));
	val &= ~(((0x1ff) << 16) | ((0xff) << 8) | 0x3);
	write32(virt + 0x40, val);

	return read32(virt + 0x60);
}

static int mach_detect(struct machine_t * mach)
{
	u32_t id;
	u32_t key;

	id = sram_read_id(phys_to_virt(0x01c00024));
	if(id == 0x1680)
	{
		key = sid_read_key(phys_to_virt(0x01c14000), 0 * 4) & 0xff;
		if(key == 0x00 || key == 0x81 || key == 0x58)
		{
			return 1;
		}
	}
	return 0;
}

static void mach_memmap(struct machine_t * mach)
{
	mmu_setup(mach->map);
}

static void mach_shutdown(struct machine_t * mach)
{
}

static void mach_reboot(struct machine_t * mach)
{
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x01c28000);
	int i;

	for(i = 0; i < count; i++)
	{
		while((read32(virt + 0x14) & (0x1 << 6)) == 0);
		write32(virt + 0x00, buf[i]);
	}
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	static char uniqueid[32 + 3 + 1] = { 0 };
	virtual_addr_t virt = phys_to_virt(0x01c14000);
	u32_t sid0, sid1, sid2, sid3;

	sid0 = sid_read_key(virt, 0 * 4);
	sid1 = sid_read_key(virt, 1 * 4);
	sid2 = sid_read_key(virt, 2 * 4);
	sid3 = sid_read_key(virt, 3 * 4);
	snprintf(uniqueid, sizeof(uniqueid), "%08x:%08x:%08x:%08x",sid0, sid1, sid2, sid3);
	return uniqueid;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t nanopi_m1 = {
	.name 		= "nanopi-m1",
	.desc 		= "NanoPi M1 Based On Allwinner H3 SOC",
	.map		= mach_map,
	.detect 	= mach_detect,
	.memmap		= mach_memmap,
	.shutdown	= mach_shutdown,
	.reboot		= mach_reboot,
	.sleep		= mach_sleep,
	.cleanup	= mach_cleanup,
	.logger		= mach_logger,
	.uniqueid	= mach_uniqueid,
	.keygen		= mach_keygen,
};

static __init void nanopi_m1_machine_init(void)
{
	register_machine(&nanopi_m1);
}

static __exit void nanopi_m1_machine_exit(void)
{
	unregister_machine(&nanopi_m1);
}

machine_initcall(nanopi_m1_machine_init);
machine_exitcall(nanopi_m1_machine_exit);
