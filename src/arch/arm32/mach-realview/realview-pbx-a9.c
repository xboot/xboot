/*
 * realview-pbx-a9.c
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
#include <realview/reg-sysctl.h>

static int mach_detect(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(REALVIEW_SYSCTL_BASE);

	if(((read32(virt + SYSCTL_ID) >> 16) & 0xfff) == 0x178)
		return 1;
	return 0;
}

static void mach_smpinit(struct machine_t * mach)
{
	extern void mmu_enable(void);
	mmu_enable();
}

static void mach_smpboot(struct machine_t * mach, void (*func)(void))
{
	uint32_t c0, c1, c2, c3;
	uint32_t f = 0xdeadbeef;

	/* Wait for all cpu cores have ready */
	do {
		c0 = read32(phys_to_virt(0x00000040));
		c1 = read32(phys_to_virt(0x00000044));
		c2 = read32(phys_to_virt(0x00000048));
		c3 = read32(phys_to_virt(0x0000004c));
	} while((c0 != f) || (c1 != f) || (c2 != f) || (c3 != f));

	/* Set boot informations */
	write32(phys_to_virt(0x0000003c), (u32_t)func);
	write32(phys_to_virt(0x00000040), 0xcafebabe);
	write32(phys_to_virt(0x00000044), 0xcafebabe);
	write32(phys_to_virt(0x00000048), 0xcafebabe);
	write32(phys_to_virt(0x0000004c), 0xcafebabe);

	/* Startup all cpu cores */
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("sev" : : : "memory");
}

static void mach_shutdown(struct machine_t * mach)
{
}

static void mach_reboot(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(REALVIEW_SYSCTL_BASE);

	write32(virt + SYSCTL_LOCK, 0xa05f);
	write32(virt + SYSCTL_RESET, read32(virt + SYSCTL_RESET) | (1 << 2));
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x10009000);
	static int initial = 0;
	int i;

	if(!initial)
	{
		u64_t clk = 24 * 1000 * 1000;
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
	return NULL;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t realview_pbx_a9 = {
	.name 		= "realview-pbx-a9",
	.desc 		= "ARM RealView Platform Baseboard Explore for Cortex-A9",
	.detect 	= mach_detect,
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

static __init void realview_pbx_a9_machine_init(void)
{
	register_machine(&realview_pbx_a9);
}

static __exit void realview_pbx_a9_machine_exit(void)
{
	unregister_machine(&realview_pbx_a9);
}

machine_initcall(realview_pbx_a9_machine_init);
machine_exitcall(realview_pbx_a9_machine_exit);
