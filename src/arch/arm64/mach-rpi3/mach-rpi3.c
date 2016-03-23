/*
 * mach-rpi3.c
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
#include <bcm2837-mbox.h>
#include <bcm2837/reg-pm.h>

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
	virtual_addr_t virt = phys_to_virt(BCM2837_PM_BASE);
	u32_t val;

	val = read32(virt + PM_RSTS);
	val |= PM_PASSWORD | PM_RSTS_RASPBERRYPI_HALT;
	write32(virt + PM_RSTS, val);

	write32(virt + PM_WDOG, PM_PASSWORD | (10 & PM_WDOG_TIME_SET));
	val = read32(virt + PM_RSTC);
	write32(virt + PM_RSTC, PM_PASSWORD | (val & PM_RSTC_WRCFG_CLR) | PM_RSTC_WRCFG_FULL_RESET);
	return TRUE;
}

static bool_t mach_reboot(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(BCM2837_PM_BASE);
	u32_t val;

	write32(virt + PM_WDOG, PM_PASSWORD | (10 & PM_WDOG_TIME_SET));
	val = read32(virt + PM_RSTC);
	write32(virt + PM_RSTC, PM_PASSWORD | (val & PM_RSTC_WRCFG_CLR) | PM_RSTC_WRCFG_FULL_RESET);
	return TRUE;
}

static bool_t mach_sleep(struct machine_t * mach)
{
	return FALSE;
}

static bool_t mach_cleanup(struct machine_t * mach)
{
	return TRUE;
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	static char uniqueid[16 + 1] = { 0 };
	uint64_t sn;

	if(bcm2837_mbox_hardware_get_serial(&sn) < 0)
		return NULL;
	sprintf(uniqueid, "%02x%02x%02x%02x%02x%02x%02x%02x",
		(sn >> 56) & 0xff, (sn >> 48) & 0xff, (sn >> 40) & 0xff, (sn >> 32) & 0xff,
		(sn >> 24) & 0xff, (sn >> 16) & 0xff, (sn >> 8) & 0xff, (sn >> 0) & 0xff);
	return uniqueid;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t rpi3 = {
	.name 		= "raspberry-pi-3-b",
	.desc 		= "Raspberry Pi 3 Module B",
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

static __init void mach_rpi3_init(void)
{
	register_machine(&rpi3);
}
machine_initcall(mach_rpi3_init);
