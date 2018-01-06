/*
 * x3288.c
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
#include <rk3288/reg-grf.h>

static const struct mmap_t mach_map[] = {
	{"ram",  0x60000000, 0x60000000, SZ_128M, MAP_TYPE_CB},
	{"dma",  0x68000000, 0x68000000, SZ_128M, MAP_TYPE_NCNB},
	{"heap", 0x70000000, 0x70000000, SZ_256M, MAP_TYPE_CB},
	{ 0 },
};

static int mach_detect(struct machine_t * mach)
{
	u32_t val;

	/*
	 * Select pwm solution, using rk_pwm.
	 */
	val = (1 << (16 + 0)) | (1 << 0);
	write32(phys_to_virt(RK3288_GRF_BASE + GRF_SOC_CON2), val);

	/*
	 * Select voltage lcdc(vop) iodomain 3.3V.
	 */
	val = (1 << (16 + 0)) | (0 << 0);
	write32(phys_to_virt(RK3288_GRF_BASE + GRF_IO_VSEL), val);

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
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0xff690000);
	int i;

	for(i = 0; i < count; i++)
	{
		while((read32(virt + 0x7c) & (0x1 << 1)) == 0);
		write32(virt + 0x00, buf[i]);
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

static struct machine_t x3288 = {
	.name 		= "x3288",
	.desc 		= "X3288 Based On RK3288 SOC",
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

static __init void x3288_machine_init(void)
{
	register_machine(&x3288);
}

static __exit void x3288_machine_exit(void)
{
	unregister_machine(&x3288);
}

machine_initcall(x3288_machine_init);
machine_exitcall(x3288_machine_exit);
