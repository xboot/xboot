/*
 * raspberry-pi-3.c
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
#include <bcm2837-mbox.h>

static const struct mmap_t mach_map[] = {
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
	virtual_addr_t virt = phys_to_virt(0x3f201000);
	static int initial = 0;
	int i;

	if(!initial)
	{
		u64_t clk = bcm2837_mbox_clock_get_rate(MBOX_CLOCK_ID_UART);
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

static struct machine_t raspberry_pi_3 = {
	.name 		= "raspberry-pi-3",
	.desc 		= "Raspberry Pi 3 Model B",
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

static __init void raspberry_pi_3_machine_init(void)
{
	register_machine(&raspberry_pi_3);
}

static __exit void raspberry_pi_3_machine_exit(void)
{
	unregister_machine(&raspberry_pi_3);
}

machine_initcall(raspberry_pi_3_machine_init);
machine_exitcall(raspberry_pi_3_machine_exit);
