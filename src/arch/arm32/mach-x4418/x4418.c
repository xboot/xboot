/*
 * x4418.c
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
#include <mmu.h>
#include <s5p4418-rstcon.h>
#include <s5p4418/reg-sys.h>
#include <s5p4418/reg-id.h>

static const struct mmap_t mach_map[] = {
	{"ram",  0x40000000, 0x40000000, SZ_128M, MAP_TYPE_CB},
	{"dma",  0x48000000, 0x48000000, SZ_128M, MAP_TYPE_NCNB},
	{"heap", 0x50000000, 0x50000000, SZ_256M, MAP_TYPE_CB},
	{ 0 },
};

static int mach_detect(struct machine_t * mach)
{
	return 1;
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
	write32(phys_to_virt(S5P4418_SYS_PWRCONT), (read32(phys_to_virt(S5P4418_SYS_PWRCONT)) & ~(0x1<<3)) | (0x1<<3));
	write32(phys_to_virt(S5P4418_SYS_PWRMODE), (read32(phys_to_virt(S5P4418_SYS_PWRMODE)) & ~(0x1<<12)) | (0x1<<12));
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	static char uniqueid[16 + 1] = { 0 };
	virtual_addr_t virt = phys_to_virt(S5P4418_ID_BASE);
	u32_t ecid0, ecid1;

	s5p4418_ip_reset(RESET_ID_ECID, 0);

	ecid0 = read32(virt + ID_ECID0);
	ecid1 = read32(virt + ID_ECID1);
	sprintf(uniqueid, "%02x%02x%02x%02x%02x%02x%02x%02x",
		(ecid0 >> 24) & 0xff, (ecid0 >> 16) & 0xff, (ecid0 >> 8) & 0xff, (ecid0 >> 0) & 0xff,
		(ecid1 >> 24) & 0xff, (ecid1 >> 16) & 0xff, (ecid1 >> 8) & 0xff, (ecid1 >> 0) & 0xff);
	return uniqueid;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t x4418 = {
	.name 		= "x4418",
	.desc 		= "X4418 Based On S5P4412",
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

static __init void x4418_machine_init(void)
{
	register_machine(&x4418);
}

static __exit void x4418_machine_exit(void)
{
	unregister_machine(&x4418);
}

machine_initcall(x4418_machine_init);
machine_exitcall(x4418_machine_exit);
