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

static bool_t mach_detect(void)
{
	return TRUE;
}

static bool_t mach_poweron(void)
{
	return TRUE;
}

static bool_t mach_poweroff(void)
{
	return FALSE;
}

static bool_t mach_reboot(void)
{
	write32(phys_to_virt(S5P4418_SYS_PWRCONT), (read32(phys_to_virt(S5P4418_SYS_PWRCONT)) & ~(0x1<<3)) | (0x1<<3));
	write32(phys_to_virt(S5P4418_SYS_PWRMODE), (read32(phys_to_virt(S5P4418_SYS_PWRMODE)) & ~(0x1<<12)) | (0x1<<12));

	return TRUE;
}

static bool_t mach_sleep(void)
{
	return FALSE;
}

static bool_t mach_cleanup(void)
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

static bool_t mach_keygen(const void * msg, int len, void * key)
{
	return FALSE;
}

const char * mach_uniqueid(void)
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

static struct machine_t x4418 = {
	.name 				= "x4418",
	.desc 				= "x4418 based on s5p4412",

	.banks = {
		[0] = {
			.start		= 0x40000000,
			.size		= SZ_512M,
		},

		[1] = {
			.start		= 0x60000000,
			.size		= SZ_512M,
		},

		[2] = {
			.start		= 0,
			.size		= 0,
		},
	},

	.detect 			= mach_detect,
	.poweron			= mach_poweron,
	.poweroff			= mach_poweroff,
	.reboot				= mach_reboot,
	.sleep				= mach_sleep,
	.cleanup			= mach_cleanup,
	.keygen				= mach_keygen,
	.uniqueid			= mach_uniqueid,
};

static __init void mach_x4418_init(void)
{
	if(register_machine(&x4418))
		LOG("Register machine '%s'", x4418.name);
	else
		LOG("Failed to register machine '%s'", x4418.name);
}

static __exit void mach_x4418_exit(void)
{
	if(unregister_machine(&x4418))
		LOG("Unregister machine '%s'", x4418.name);
	else
		LOG("Failed to unregister machine '%s'", x4418.name);
}

core_initcall(mach_x4418_init);
core_exitcall(mach_x4418_exit);
