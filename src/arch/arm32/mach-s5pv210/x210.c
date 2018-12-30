/*
 * x210.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

static int mach_detect(struct machine_t * mach)
{
	virtual_addr_t virt;

	if((read32(phys_to_virt(0xe0000000)) >> 12) != 0x43110)
		return 0;
	virt = phys_to_virt(0xe010e81c);
	write32(virt, (read32(virt) & ~0x301) | 0x301);
	return 1;
}

static void mach_memmap(struct machine_t * mach)
{
	machine_mmap(mach, "ram", 0x32000000, 0x32000000, SZ_1M * 32, MAP_TYPE_CB);
	machine_mmap(mach, "dma", 0x34000000, 0x34000000, SZ_1M * 32, MAP_TYPE_NCNB);
	machine_mmap(mach, "heap", 0x36000000, 0x36000000, SZ_1M * 160, MAP_TYPE_CB);
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
	virtual_addr_t virt = phys_to_virt(0xe010e81c);
	write32(virt, (read32(virt) & ~0x301) | 0x201);
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
	virtual_addr_t virt = phys_to_virt(0xe2900800);
	int i;

	if((read32(virt + 0x8) & (1 << 0)))
	{
		for(i = 0; i < count; i++)
		{
			while((read32(virt + 0x18) & (1 << 24)));
			write8(virt + 0x20, buf[i]);
		}
	}
	else
	{
		for(i = 0; i < count; i++)
		{
			while(!(read32(virt + 0x10) & (1 << 1)));
			write8(virt + 0x20, buf[i]);
		}
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

static struct machine_t x210 = {
	.name 		= "x210",
	.desc 		= "X210 Based On Samsung S5PV210",
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

static __init void x210_machine_init(void)
{
	register_machine(&x210);
}

static __exit void x210_machine_exit(void)
{
	unregister_machine(&x210);
}

machine_initcall(x210_machine_init);
machine_exitcall(x210_machine_exit);
