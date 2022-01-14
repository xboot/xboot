/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __CSKY_LINKAGE_H__
#define __CSKY_LINKAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ALIGN		.align 4
#define ALIGN_STR	".align 4"

#define ENTRY(name) \
	.globl name; \
	ALIGN; \
	name:

#define WEAK(name) \
	.weak name; \
	name:

#define END(name) \
	.size name, .-name

#define ENDPROC(name) \
	.type name, %function; \
	END(name)

#ifdef __cplusplus
}
#endif

#endif /* __CSKY_LINKAGE_H__ */
