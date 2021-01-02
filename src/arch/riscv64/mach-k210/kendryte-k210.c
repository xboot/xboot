/*
 * kendryte_k210.c
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
#include <k210/reg-sysctl.h>

enum power_bank_t
{
	POWER_BANK0	= 0,
	POWER_BANK1	= 1,
	POWER_BANK2	= 2,
	POWER_BANK3	= 3,
	POWER_BANK4	= 4,
	POWER_BANK5	= 5,
	POWER_BANK6	= 6,
	POWER_BANK7	= 7,
};

enum power_mode_t
{
	POWER_V33	= 0,
	POWER_V18	= 1,
};

static void k210_set_power_bank(enum power_bank_t bank, enum power_mode_t mode)
{
	virtual_addr_t virt = phys_to_virt(K210_SYSCTL_BASE);

	if(mode)
		write32(virt + SYSCTL_POWER_SEL, read32(virt + SYSCTL_POWER_SEL) | (0x1 << bank));
	else
		write32(virt + SYSCTL_POWER_SEL, read32(virt + SYSCTL_POWER_SEL) & ~(0x1 << bank));
}

static void k210_set_spi0_dvp_jamlink(int enable)
{
	virtual_addr_t virt = phys_to_virt(K210_SYSCTL_BASE);

	if(enable)
		write32(virt + SYSCTL_MISC, read32(virt + SYSCTL_MISC) | (0x1 << 10));
	else
		write32(virt + SYSCTL_MISC, read32(virt + SYSCTL_MISC) & ~(0x1 << 10));
}

static int mach_detect(struct machine_t * mach)
{
	/* Group A, IO0 - IO17 */
	k210_set_power_bank(POWER_BANK0, POWER_V33);
	k210_set_power_bank(POWER_BANK1, POWER_V33);
	k210_set_power_bank(POWER_BANK2, POWER_V33);
	/* Group B, IO18 - IO35 */
	k210_set_power_bank(POWER_BANK3, POWER_V33);
	k210_set_power_bank(POWER_BANK4, POWER_V33);
	k210_set_power_bank(POWER_BANK5, POWER_V33);
	/* Group C, IO36 - IO47 */
	k210_set_power_bank(POWER_BANK6, POWER_V18);
	k210_set_power_bank(POWER_BANK7, POWER_V18);

	/* Select jamlink of spi0 and dvp data */
	k210_set_spi0_dvp_jamlink(1);

	return 1;
}

static void mach_smpinit(struct machine_t * mach)
{
}

static void mach_smpboot(struct machine_t * mach, void (*func)(void))
{
	extern void sys_smp_secondary_boot(void (*func)(void));
	sys_smp_secondary_boot(func);
}

static void mach_shutdown(struct machine_t * mach)
{
}

static void mach_reboot(struct machine_t * mach)
{
	write32(phys_to_virt(K210_SYSCTL_BASE) + SYSCTL_SOFT_RESET, 0x1);
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x38000000);
	int i;

	for(i = 0; i < count; i++)
	{
		while(read32(virt + 0x0) & (1 << 31));
		write32(virt + 0x0, buf[i]);
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

static struct machine_t kendryte_k210 = {
	.name 		= "kendryte-k210",
	.desc 		= "Kendryte k210 development board",
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

static __init void kendryte_k210_machine_init(void)
{
	register_machine(&kendryte_k210);
}

static __exit void kendryte_k210_machine_exit(void)
{
	unregister_machine(&kendryte_k210);
}

machine_initcall(kendryte_k210_machine_init);
machine_exitcall(kendryte_k210_machine_exit);
