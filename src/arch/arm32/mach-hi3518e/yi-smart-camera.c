/*
 * yi-smart-camera.c
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

#include <xboot.h>
#include <mmu.h>

static const struct mmap_t mach_map[] = {
	{"ram",  0x80000000, 0x80000000, SZ_1M * 8, MAP_TYPE_CB},
	{"dma",  0x80800000, 0x80800000, SZ_1M * 8, MAP_TYPE_NCNB},
	{"heap", 0x81000000, 0x81000000, SZ_1M * 16, MAP_TYPE_CB},
	{ 0 },
};

static int mach_detect(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(0x20050000);
	uint32_t id = (((read32(virt + 0xeec) & 0xff) << 24) |
				((read32(virt + 0xee8) & 0xff) << 16) |
				((read32(virt + 0xee4) & 0xff) <<  8) |
				((read32(virt + 0xee0) & 0xff) <<  0));

	if(id == 0x35180100)
		return 1;
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
	virtual_addr_t virt = phys_to_virt(0x20050000);

	write32(virt + 0x44, 0x1acce551);
	write32(virt + 0x04, 0x00000001);
	write32(virt + 0x44, 0x00000000);
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x20080000);
	int i;

	for(i = 0; i < count; i++)
	{
		while((read8(virt + 0x18) & (0x1 << 5)));
		write8(virt + 0x00, buf[i]);
	}
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	return NULL;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t yi_smart_camera = {
	.name 		= "yi-smart-camera",
	.desc 		= "XiaoMi YI Smart Camera Based On Hi3518e",
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

static __init void yi_smart_camera_machine_init(void)
{
	register_machine(&yi_smart_camera);
}

static __exit void yi_smart_camera_machine_exit(void)
{
	unregister_machine(&yi_smart_camera);
}

machine_initcall(yi_smart_camera_machine_init);
machine_exitcall(yi_smart_camera_machine_exit);
