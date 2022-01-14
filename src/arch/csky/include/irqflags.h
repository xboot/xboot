/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __CSKY_IRQFLAGS_H__
#define __CSKY_IRQFLAGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <csky.h>

static inline void arch_local_irq_enable(void)
{
	asm volatile("psrset ee, ie\n":::"memory");
}

static inline void arch_local_irq_disable(void)
{
	asm volatile("psrclr ie\n":::"memory");
}

static inline irq_flags_t arch_local_irq_save(void)
{
	unsigned long flags;

	flags = mfcr("psr");
	asm volatile("psrclr ie\n":::"memory");
	return flags;
}

static inline void arch_local_irq_restore(irq_flags_t flags)
{
	mtcr("psr", flags);
}

#define local_irq_enable()			do { arch_local_irq_enable(); } while(0)
#define local_irq_disable()			do { arch_local_irq_disable(); } while(0)
#define local_irq_save(flags)		do { flags = arch_local_irq_save(); } while(0)
#define local_irq_restore(flags)	do { arch_local_irq_restore(flags); } while(0)

#ifdef __cplusplus
}
#endif

#endif /* __CSKY_IRQFLAGS_H__ */
