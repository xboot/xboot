/*
 * x3128.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <rk3128/reg-cru.h>
#include <rk3128/reg-grf.h>
#include <rk3128/reg-pmu.h>

static int mach_detect(struct machine_t * mach)
{
	return 1;
}

static void mach_smpinit(struct machine_t * mach)
{
}

static void mach_smpboot(struct machine_t * mach, void (*func)(void))
{
	uint32_t c0, c1, c2, c3;
	uint32_t f = 0xdeadbeef;

	/* Wait for all cpu cores have ready */
	do {
		c0 = read32(phys_to_virt(0x10080010));
		c1 = read32(phys_to_virt(0x10080014));
		c2 = read32(phys_to_virt(0x10080018));
		c3 = read32(phys_to_virt(0x1008001c));
	} while((c0 != f) || (c1 != f) || (c2 != f) || (c3 != f));

	/* Set boot informations */
	write32(phys_to_virt(0x1008000c), (u32_t)func);
	write32(phys_to_virt(0x10080010), 0xcafebabe);
	write32(phys_to_virt(0x10080014), 0xcafebabe);
	write32(phys_to_virt(0x10080018), 0xcafebabe);
	write32(phys_to_virt(0x1008001c), 0xcafebabe);

	/* Startup all cpu cores */
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("sev" : : : "memory");
}

static void mach_shutdown(struct machine_t * mach)
{
}

static void mach_reboot(struct machine_t * mach)
{
	write32(phys_to_virt(RK3128_PMU_BASE) + PMU_SYS_REG0, 0x0);
	write32(phys_to_virt(RK3128_CRU_BASE) + CRU_GLB_SRST_FST_VALUE, 0xfdb9);
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x20068000);
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

static struct machine_t x3128 = {
	.name 		= "x3128",
	.desc 		= "X3128 Based On RK3128 SOC",
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

static __init void x3128_machine_init(void)
{
	register_machine(&x3128);
}

static __exit void x3128_machine_exit(void)
{
	unregister_machine(&x3128);
}

machine_initcall(x3128_machine_init);
machine_exitcall(x3128_machine_exit);
