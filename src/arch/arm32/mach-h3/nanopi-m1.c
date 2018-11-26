/*
 * nanopi-m1.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <mmu.h>

static u32_t sram_read_id(virtual_addr_t virt)
{
	u32_t id;

	write32(virt, read32(virt) | (1 << 15));
	id = read32(virt) >> 16;
	write32(virt, read32(virt) & ~(1 << 15));
	return id;
}

static u32_t sid_read_key(virtual_addr_t virt, int offset)
{
	u32_t val;

	val = read32(virt + 0x40);
	val &= ~(((0x1ff) << 16) | 0x3);
	val |= (offset << 16);
	write32(virt + 0x40, val);

	val &= ~(((0xff) << 8) | 0x3);
	val |= (0xac << 8) | 0x2;
	write32(virt + 0x40, val);

	while(read32(virt + 0x40) & (1 << 1));
	val &= ~(((0x1ff) << 16) | ((0xff) << 8) | 0x3);
	write32(virt + 0x40, val);

	return read32(virt + 0x60);
}

static int mach_detect(struct machine_t * mach)
{
	u32_t id;
	u32_t key;

	id = sram_read_id(phys_to_virt(0x01c00024));
	if(id == 0x1680)
	{
		key = sid_read_key(phys_to_virt(0x01c14000), 0 * 4) & 0xff;
		if(key == 0x00 || key == 0x81 || key == 0x58)
		{
			return 1;
		}
	}
	return 0;
}

static void mach_memmap(struct machine_t * mach)
{
	machine_mmap(mach, "ram", 0x40000000, 0x40000000, SZ_128M, MAP_TYPE_CB);
	machine_mmap(mach, "dma", 0x48000000, 0x48000000, SZ_128M, MAP_TYPE_NCNB);
	machine_mmap(mach, "heap", 0x50000000, 0x50000000, SZ_256M, MAP_TYPE_CB);
	mmu_setup(mach);
}

static void mach_smpinit(struct machine_t * mach, int cpu)
{
}

static void mach_smpboot(struct machine_t * mach, int cpu, void (*func)(int cpu))
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
	virtual_addr_t virt = phys_to_virt(0x01c28000);
	int i;

	for(i = 0; i < count; i++)
	{
		while((read32(virt + 0x14) & (0x1 << 6)) == 0);
		write32(virt + 0x00, buf[i]);
	}
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	static char uniqueid[32 + 3 + 1] = { 0 };
	virtual_addr_t virt = phys_to_virt(0x01c14000);
	u32_t sid0, sid1, sid2, sid3;

	sid0 = sid_read_key(virt, 0 * 4);
	sid1 = sid_read_key(virt, 1 * 4);
	sid2 = sid_read_key(virt, 2 * 4);
	sid3 = sid_read_key(virt, 3 * 4);
	snprintf(uniqueid, sizeof(uniqueid), "%08x:%08x:%08x:%08x",sid0, sid1, sid2, sid3);
	return uniqueid;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t nanopi_m1 = {
	.name 		= "nanopi-m1",
	.desc 		= "NanoPi M1 Based On Allwinner H3 SOC",
	.detect 	= mach_detect,
	.memmap		= mach_memmap,
	.smpinit	= mach_smpinit,
	.smpboot	= mach_smpboot,
	.shutdown	= mach_shutdown,
	.reboot		= mach_reboot,
	.sleep		= mach_sleep,
	.cleanup	= mach_cleanup,
	.logger		= mach_logger,
	.uniqueid	= mach_uniqueid,
	.keygen		= mach_keygen,
};

static __init void nanopi_m1_machine_init(void)
{
	register_machine(&nanopi_m1);
}

static __exit void nanopi_m1_machine_exit(void)
{
	unregister_machine(&nanopi_m1);
}

machine_initcall(nanopi_m1_machine_init);
machine_exitcall(nanopi_m1_machine_exit);
