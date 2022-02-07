/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __CSKY_SETJMP_H__
#define __CSKY_SETJMP_H__

#define JMP_BUF_SP  0
#define JMP_BUF_L0  1
#define JMP_BUF_L1  2
#define JMP_BUF_L2  3
#define JMP_BUF_L3  4
#define JMP_BUF_L4  5
#define JMP_BUF_L5  6
#define JMP_BUF_GB  7
#define JMP_BUF_LR  8
#define JMP_BUF_MAX 9

#ifndef __ASSEMBLY__

#ifdef __cplusplus
extern "C" {
#endif

struct __jmp_buf {
    unsigned long __jmp_buf[JMP_BUF_MAX];
};

typedef struct __jmp_buf jmp_buf[1];

int setjmp(jmp_buf);
void longjmp(jmp_buf, int);

#ifdef __cplusplus
}
#endif

#endif  /* __ASSEMBLY__ */
#endif 	/* __CSKY_SETJMP_H__ */
