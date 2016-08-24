/*
 * raspberry-pi-2.c
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
#include <bcm2836-mbox.h>
#include <bcm2836/reg-pm.h>

static const struct mmap_t mach_map[] = {
	{"ram",  0x00000000, 0x00000000, SZ_128M, MAP_TYPE_CB},
	{"dma",  0x08000000, 0x08000000, SZ_128M, MAP_TYPE_NCNB},
	{"heap", 0x10000000, 0x10000000, SZ_256M, MAP_TYPE_CB},
	{ 0 },
};

static int mach_detect(struct machine_t * mach)
{
	return 1;
}

static void mach_memmap(struct machine_t * mach)
{
}

static void mach_shutdown(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(BCM2836_PM_BASE);
	u32_t val;

	val = read32(virt + PM_RSTS);
	val |= PM_PASSWORD | PM_RSTS_RASPBERRYPI_HALT;
	write32(virt + PM_RSTS, val);

	write32(virt + PM_WDOG, PM_PASSWORD | (10 & PM_WDOG_TIME_SET));
	val = read32(virt + PM_RSTC);
	write32(virt + PM_RSTC, PM_PASSWORD | (val & PM_RSTC_WRCFG_CLR) | PM_RSTC_WRCFG_FULL_RESET);
}

static void mach_reboot(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(BCM2836_PM_BASE);
	u32_t val;

	write32(virt + PM_WDOG, PM_PASSWORD | (10 & PM_WDOG_TIME_SET));
	val = read32(virt + PM_RSTC);
	write32(virt + PM_RSTC, PM_PASSWORD | (val & PM_RSTC_WRCFG_CLR) | PM_RSTC_WRCFG_FULL_RESET);
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x3f201000);
	static int initial = 0;
	int i;

	if(!initial)
	{
		u64_t clk = 3 * 1000 * 1000;
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
	static char uniqueid[16 + 1] = { 0 };
	uint64_t sn;

	if(bcm2836_mbox_hardware_get_serial(&sn) < 0)
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

static struct machine_t raspberry_pi_2 = {
	.name 		= "raspberry-pi-2",
	.desc 		= "Raspberry Pi 2 Model B",
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

static __init void raspberry_pi_2_machine_init(void)
{
	register_machine(&raspberry_pi_2);
}

static __exit void raspberry_pi_2_machine_exit(void)
{
	unregister_machine(&raspberry_pi_2);
}

machine_initcall(raspberry_pi_2_machine_init);
machine_exitcall(raspberry_pi_2_machine_exit);
