/*
 * kendryte_k210.c
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
#include <riscv64.h>
#include "fpioa.h"
#include "sysclock.h"
#include "sysctl.h"
#include "uarths.h"

static int mach_detect(struct machine_t * mach)
{
	fpioa_init();
	sys_clock_init();
	uart_init();
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
	virtual_addr_t virtuart = phys_to_virt(0x38000000);
	virtual_addr_t virtcpu = phys_to_virt(0x50440080);
	int id = (csr_read(mhartid) == 0) ? 0 : 1;
	int i;

	for(i = 0; i < count; i++)
	{
		write32(virtcpu + 0x0, (id << 30) | buf[i]);
		while(read32(virtuart + 0x0) & (1 << 31));
		write32(virtuart + 0x0, buf[i]);
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
	.memmap		= mach_memmap,
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
