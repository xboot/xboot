/*
 * mach-x6818.c
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
#include <s5p6818-rstcon.h>
#include <s5p6818/reg-sys.h>
#include <s5p6818/reg-id.h>

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
	return TRUE;
}

static bool_t mach_sleep(void)
{
	return FALSE;
}

static bool_t mach_cleanup(void)
{
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

	s5p6818_ip_reset(RESET_ID_ECID, 0);

	ecid0 = read32(phys_to_virt(S5P6818_ID_ECID0));
	ecid1 = read32(phys_to_virt(S5P6818_ID_ECID1));
	sprintf(uniqueid, "%02x%02x%02x%02x%02x%02x%02x%02x",
		(ecid0 >> 24) & 0xff, (ecid0 >> 16) & 0xff, (ecid0 >> 8) & 0xff, (ecid0 >> 0) & 0xff,
		(ecid1 >> 24) & 0xff, (ecid1 >> 16) & 0xff, (ecid1 >> 8) & 0xff, (ecid1 >> 0) & 0xff);
	return uniqueid;
}

static struct machine_t x6818 = {
	.name 				= "x6818",
	.desc 				= "x6818 based on s5p6818",

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

static __init void mach_x6818_init(void)
{
	if(register_machine(&x6818))
		LOG("Register machine '%s'", x6818.name);
	else
		LOG("Failed to register machine '%s'", x6818.name);
}

static __exit void mach_x6818_exit(void)
{
	if(unregister_machine(&x6818))
		LOG("Unregister machine '%s'", x6818.name);
	else
		LOG("Failed to unregister machine '%s'", x6818.name);
}

core_initcall(mach_x6818_init);
core_exitcall(mach_x6818_exit);
