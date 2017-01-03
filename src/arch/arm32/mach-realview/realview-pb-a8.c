/*
 * realview-pb-a8.c
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
#include <realview/reg-sysctl.h>

static const struct mmap_t mach_map[] = {
	{"ram",  0x40000000, 0x40000000, SZ_64M, MAP_TYPE_CB},
	{"dma",  0x48000000, 0x48000000, SZ_64M, MAP_TYPE_NCNB},
	{"heap", 0x50000000, 0x50000000, SZ_128M, MAP_TYPE_CB},
	{ 0 },
};

static int mach_detect(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(REALVIEW_SYSCTL_BASE);

	if(((read32(virt + SYSCTL_ID) >> 16) & 0xfff) == 0x178)
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
	virtual_addr_t virt = phys_to_virt(REALVIEW_SYSCTL_BASE);

	write32(virt + SYSCTL_LOCK, 0xa05f);
	write32(virt + SYSCTL_RESET, read32(virt + SYSCTL_RESET) | (1 << 2));
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x10009000);
	static int initial = 0;
	int i;

	if(!initial)
	{
		u64_t clk = 24 * 1000 * 1000;
		u32_t div, rem, frac;

		div = clk / (16 * 115200);
		rem = clk % (16 * 115200);
		frac = (8 * rem / 115200) >> 1;
		frac += (8 * rem / 115200) & 1;

		write32(virt + 0x24, div);
		write32(virt + 0x28, frac);
		write32(virt + 0x2c, (0x3 << 5) | (0x0 << 3) | (0x0 << 1) | (0x1 << 4));
		write32(virt + 0x30, (1 << 0) | (1 << 8) | (1 << 9));
	}

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

static struct machine_t realview_pb_a8 = {
	.name 		= "realview-pb-a8",
	.desc 		= "ARM RealView Platform Baseboard For Cortex-A8",
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

static __init void realview_pb_a8_machine_init(void)
{
	register_machine(&realview_pb_a8);
}

static __exit void realview_pb_a8_machine_exit(void)
{
	unregister_machine(&realview_pb_a8);
}

machine_initcall(realview_pb_a8_machine_init);
machine_exitcall(realview_pb_a8_machine_exit);
