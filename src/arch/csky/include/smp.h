/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __CSKY_SMP_H__
#define __CSKY_SMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xconfigs.h>
#include <csky.h>

static inline int smp_processor_id(void)
{
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* __CSKY_SMP_H__ */
