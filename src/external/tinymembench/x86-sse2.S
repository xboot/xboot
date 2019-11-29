/*
 * Copyright © 2011 Siarhei Siamashka <siarhei.siamashka@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#if defined(__i386__) || defined(__amd64__)

.intel_syntax noprefix
.text

#define PREFETCH_DISTANCE 256

.macro asm_function_helper function_name
    .global \function_name
.func \function_name
\function_name:
#ifdef __amd64__
    .set DST,  rdi
    .set SRC,  rsi
    .set SIZE, rdx
#else
    mov  eax,  [esp + 4]
    mov  ecx,  [esp + 8]
    mov  edx,  [esp + 12]
    .set DST,  eax
    .set SRC,  ecx
    .set SIZE, edx
#endif
.endm

.macro asm_function function_name
#ifdef _WIN32
    asm_function_helper _\function_name
#else
    asm_function_helper \function_name
#endif
.endm

/*****************************************************************************/

asm_function aligned_block_copy_sse2
0:
    movdqa      xmm0,       [SRC + 0]
    movdqa      xmm1,       [SRC + 16]
    movdqa      xmm2,       [SRC + 32]
    movdqa      xmm3,       [SRC + 48]
    movdqa      [DST + 0],  xmm0
    movdqa      [DST + 16], xmm1
    movdqa      [DST + 32], xmm2
    movdqa      [DST + 48], xmm3
    add         SRC,        64
    add         DST,        64
    sub         SIZE, 64
    jg          0b
    ret
.endfunc

asm_function aligned_block_copy_nt_sse2
0:
    movdqa      xmm0,       [SRC + 0]
    movdqa      xmm1,       [SRC + 16]
    movdqa      xmm2,       [SRC + 32]
    movdqa      xmm3,       [SRC + 48]
    movntdq     [DST + 0],  xmm0
    movntdq     [DST + 16], xmm1
    movntdq     [DST + 32], xmm2
    movntdq     [DST + 48], xmm3
    add         SRC,        64
    add         DST,        64
    sub         SIZE, 64
    jg          0b
    ret
.endfunc

asm_function aligned_block_copy_pf32_sse2
0:
    prefetchnta [SRC + PREFETCH_DISTANCE]
    prefetchnta [SRC + PREFETCH_DISTANCE + 32]
    movdqa      xmm0,       [SRC + 0]
    movdqa      xmm1,       [SRC + 16]
    movdqa      xmm2,       [SRC + 32]
    movdqa      xmm3,       [SRC + 48]
    movdqa      [DST + 0],  xmm0
    movdqa      [DST + 16], xmm1
    movdqa      [DST + 32], xmm2
    movdqa      [DST + 48], xmm3
    add         SRC,        64
    add         DST,        64
    sub         SIZE,       64
    jg          0b
    ret
.endfunc

asm_function aligned_block_copy_nt_pf32_sse2
0:
    prefetchnta [SRC + PREFETCH_DISTANCE]
    prefetchnta [SRC + PREFETCH_DISTANCE + 32]
    movdqa      xmm0,       [SRC + 0]
    movdqa      xmm1,       [SRC + 16]
    movdqa      xmm2,       [SRC + 32]
    movdqa      xmm3,       [SRC + 48]
    movntdq     [DST + 0],  xmm0
    movntdq     [DST + 16], xmm1
    movntdq     [DST + 32], xmm2
    movntdq     [DST + 48], xmm3
    add         SRC,        64
    add         DST,        64
    sub         SIZE,       64
    jg          0b
    ret
.endfunc

asm_function aligned_block_copy_pf64_sse2
0:
    prefetchnta [SRC + PREFETCH_DISTANCE]
    movdqa      xmm0,       [SRC + 0]
    movdqa      xmm1,       [SRC + 16]
    movdqa      xmm2,       [SRC + 32]
    movdqa      xmm3,       [SRC + 48]
    movdqa      [DST + 0],  xmm0
    movdqa      [DST + 16], xmm1
    movdqa      [DST + 32], xmm2
    movdqa      [DST + 48], xmm3
    add         SRC,        64
    add         DST,        64
    sub         SIZE,       64
    jg          0b
    ret
.endfunc

asm_function aligned_block_copy_nt_pf64_sse2
0:
    prefetchnta [SRC + PREFETCH_DISTANCE]
    movdqa      xmm0,       [SRC + 0]
    movdqa      xmm1,       [SRC + 16]
    movdqa      xmm2,       [SRC + 32]
    movdqa      xmm3,       [SRC + 48]
    movntdq     [DST + 0],  xmm0
    movntdq     [DST + 16], xmm1
    movntdq     [DST + 32], xmm2
    movntdq     [DST + 48], xmm3
    add         SRC,        64
    add         DST,        64
    sub         SIZE,       64
    jg          0b
    ret
.endfunc

asm_function aligned_block_fill_sse2
    movdqa      xmm0,       [SRC + 0]
0:
    movdqa      [DST + 0],  xmm0
    movdqa      [DST + 16], xmm0
    movdqa      [DST + 32], xmm0
    movdqa      [DST + 48], xmm0
    add         DST,        64
    sub         SIZE,       64
    jg          0b
    ret
.endfunc

asm_function aligned_block_fill_nt_sse2
    movdqa      xmm0,       [SRC + 0]
0:
    movntdq     [DST + 0],  xmm0
    movntdq     [DST + 16], xmm0
    movntdq     [DST + 32], xmm0
    movntdq     [DST + 48], xmm0
    add         DST,        64
    sub         SIZE,       64
    jg          0b
    ret
.endfunc

/*****************************************************************************/

#endif
