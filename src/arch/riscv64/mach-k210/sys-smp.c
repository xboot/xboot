/*
 * sys-smp.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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

struct smp_boot_entry_t {
	void (*func)(void);
	atomic_t atomic;
};
struct smp_boot_entry_t __smp_boot_entry[CONFIG_MAX_SMP_CPUS];

void sys_smp_secondary_startup(int cpu)
{
	struct smp_boot_entry_t * e = &__smp_boot_entry[0];

	if(cpu < 0 || cpu >= CONFIG_MAX_SMP_CPUS)
		return;

	e[cpu].func = NULL;
	atomic_set(&e[cpu].atomic, 0);

	while(1)
	{
		while(atomic_cmpxchg(&e[cpu].atomic, 1, 0) != 1)
		{
		}
		if(e[cpu].func)
			e[cpu].func();
	}
}

void sys_smp_secondary_boot(void (*func)(void))
{
	struct smp_boot_entry_t * e = &__smp_boot_entry[0];
	int i;

	for(i = 0; i < CONFIG_MAX_SMP_CPUS; i++)
	{
		e[i].func = func;
		atomic_cmpxchg(&e[i].atomic, 0, 1);
	}
}
