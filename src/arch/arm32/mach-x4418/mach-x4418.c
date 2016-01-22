/*
 * mach-x4418.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <cp15.h>
#include <s5p4418-rstcon.h>
#include <s5p4418/reg-sys.h>
#include <s5p4418/reg-id.h>

static const struct mmap_t mach_map[] = {
	{ 0 },
};

static bool_t mach_detect(struct machine_t * mach)
{
	return TRUE;
}

static bool_t mach_memmap(struct machine_t * mach)
{
	return TRUE;
}

static bool_t mach_shutdown(struct machine_t * mach)
{
	return FALSE;
}

static bool_t mach_reboot(struct machine_t * mach)
{
	write32(phys_to_virt(S5P4418_SYS_PWRCONT), (read32(phys_to_virt(S5P4418_SYS_PWRCONT)) & ~(0x1<<3)) | (0x1<<3));
	write32(phys_to_virt(S5P4418_SYS_PWRMODE), (read32(phys_to_virt(S5P4418_SYS_PWRMODE)) & ~(0x1<<12)) | (0x1<<12));
	return TRUE;
}

static bool_t mach_sleep(struct machine_t * mach)
{
	return FALSE;
}

static bool_t mach_cleanup(struct machine_t * mach)
{
	/* disable irq */
	irq_disable();

	/* disable fiq */
	fiq_disable();

	/* disable icache */
	icache_disable();

	/* disable dcache */
	dcache_disable();

	/* disable mmu */
	mmu_disable();

	/* disable vic */
	vic_disable();

	return TRUE;
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	static char uniqueid[16 + 1];
	u32_t ecid0, ecid1;

	s5p4418_ip_reset(RESET_ID_ECID, 0);

	ecid0 = read32(phys_to_virt(S5P4418_ID_ECID0));
	ecid1 = read32(phys_to_virt(S5P4418_ID_ECID1));
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
	.desc 		= "x4418 based on s5p4412",
	.map		= mach_map,
	.detect 	= mach_detect,
	.memmap		= mach_memmap,
	.shutdown	= mach_shutdown,
	.reboot		= mach_reboot,
	.sleep		= mach_sleep,
	.cleanup	= mach_cleanup,
	.uniqueid	= mach_uniqueid,
	.keygen		= mach_keygen,
};

static __init void mach_x4418_init(void)
{
	register_machine(&x4418);
}
pure_initcall(mach_x4418_init);
