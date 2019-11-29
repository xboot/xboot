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

#ifdef __arm__

.text
.fpu neon
.arch armv7a
.object_arch armv4
.arm
.p2align 2

.macro asm_function function_name
    .global \function_name
.func \function_name
\function_name:
    DST         .req r0
    SRC         .req r1
    SIZE        .req r2
.endm

/* Actually this calculates a sum of 32-bit values */
asm_function aligned_block_read_neon
    vmov.u32    q3,  #0
    vmov.u32    q12, #0
    vmov.u32    q13, #0
    vmov.u32    q14, #0
    vmov.u32    q15, #0
    bic         SIZE, SIZE, #127
0:
.rept 2
    vld1.32     {q0, q1}, [SRC]!
    vadd.u32    q15, q15, q3
    vadd.u32    q12, q12, q0
    vld1.32     {q2, q3}, [SRC]!
    vadd.u32    q13, q13, q1
    vadd.u32    q14, q14, q2
.endr
    subs        SIZE, SIZE, #128
    bgt         0b
    vadd.u32    q15, q15, q3
    vadd.u32    q15, q15, q12
    vadd.u32    q15, q15, q13
    vadd.u32    q15, q15, q14
    vadd.u32    d31, d30, d31
    vpadd.u32   d31, d31, d31
    vmov.u32    r0, d31[0]
    bx          lr
.endfunc

/* Actually this calculates a sum of 32-bit values */
asm_function aligned_block_read_pf32_neon
    vmov.u32    q3,  #0
    vmov.u32    q12, #0
    vmov.u32    q13, #0
    vmov.u32    q14, #0
    vmov.u32    q15, #0
    bic         SIZE, SIZE, #127
0:
.rept 2
    vld1.32     {q0, q1}, [SRC]!
    vadd.u32    q15, q15, q3
    pld         [SRC, #512]
    vadd.u32    q12, q12, q0
    vld1.32     {q2, q3}, [SRC]!
    vadd.u32    q13, q13, q1
    pld         [SRC, #512]
    vadd.u32    q14, q14, q2
.endr
    subs        SIZE, SIZE, #128
    bgt         0b
    vadd.u32    q15, q15, q3
    vadd.u32    q15, q15, q12
    vadd.u32    q15, q15, q13
    vadd.u32    q15, q15, q14
    vadd.u32    d31, d30, d31
    vpadd.u32   d31, d31, d31
    vmov.u32    r0, d31[0]
    bx          lr
.endfunc

/* Actually this calculates a sum of 32-bit values */
asm_function aligned_block_read_pf64_neon
    vmov.u32    q3,  #0
    vmov.u32    q12, #0
    vmov.u32    q13, #0
    vmov.u32    q14, #0
    vmov.u32    q15, #0
    bic         SIZE, SIZE, #127
0:
.rept 2
    vld1.32     {q0, q1}, [SRC]!
    vadd.u32    q15, q15, q3
    vadd.u32    q12, q12, q0
    vld1.32     {q2, q3}, [SRC]!
    vadd.u32    q13, q13, q1
    pld         [SRC, #512]
    vadd.u32    q14, q14, q2
.endr
    subs        SIZE, SIZE, #128
    bgt         0b
    vadd.u32    q15, q15, q3
    vadd.u32    q15, q15, q12
    vadd.u32    q15, q15, q13
    vadd.u32    q15, q15, q14
    vadd.u32    d31, d30, d31
    vpadd.u32   d31, d31, d31
    vmov.u32    r0, d31[0]
    bx          lr
.endfunc

/* Actually this calculates a sum of 32-bit values */
asm_function aligned_block_read2_neon
    vmov.u32    q3,  #0
    vmov.u32    q12, #0
    vmov.u32    q13, #0
    vmov.u32    q14, #0
    vmov.u32    q15, #0
    bic         SIZE, SIZE, #127
0:
.rept 2
    vld1.32     {q0, q1}, [SRC]!
    vadd.u32    q15, q15, q3
    vadd.u32    q12, q12, q0
    vld1.32     {q2, q3}, [DST]!
    vadd.u32    q13, q13, q1
    vadd.u32    q14, q14, q2
.endr
    subs        SIZE, SIZE, #128
    bgt         0b
    vadd.u32    q15, q15, q3
    vadd.u32    q15, q15, q12
    vadd.u32    q15, q15, q13
    vadd.u32    q15, q15, q14
    vadd.u32    d31, d30, d31
    vpadd.u32   d31, d31, d31
    vmov.u32    r0, d31[0]
    bx          lr
.endfunc

/* Actually this calculates a sum of 32-bit values */
asm_function aligned_block_read2_pf32_neon
    vmov.u32    q3,  #0
    vmov.u32    q12, #0
    vmov.u32    q13, #0
    vmov.u32    q14, #0
    vmov.u32    q15, #0
    bic         SIZE, SIZE, #127
0:
.rept 2
    vld1.32     {q0, q1}, [SRC]!
    vadd.u32    q15, q15, q3
    pld         [SRC, #512]
    vadd.u32    q12, q12, q0
    vld1.32     {q2, q3}, [DST]!
    vadd.u32    q13, q13, q1
    pld         [DST, #512]
    vadd.u32    q14, q14, q2
.endr
    subs        SIZE, SIZE, #128
    bgt         0b
    vadd.u32    q15, q15, q3
    vadd.u32    q15, q15, q12
    vadd.u32    q15, q15, q13
    vadd.u32    q15, q15, q14
    vadd.u32    d31, d30, d31
    vpadd.u32   d31, d31, d31
    vmov.u32    r0, d31[0]
    bx          lr
.endfunc

/* Actually this calculates a sum of 32-bit values */
asm_function aligned_block_read2_pf64_neon
    vmov.u32    q3,  #0
    vmov.u32    q12, #0
    vmov.u32    q13, #0
    vmov.u32    q14, #0
    vmov.u32    q15, #0
    bic         SIZE, SIZE, #127
0:
.irp PTR, SRC, DST
    vld1.32     {q0, q1}, [\PTR]!
    vadd.u32    q15, q15, q3
    vadd.u32    q12, q12, q0
    vld1.32     {q2, q3}, [\PTR]!
    vadd.u32    q13, q13, q1
    pld         [\PTR, #512]
    vadd.u32    q14, q14, q2
.endr
    subs        SIZE, SIZE, #128
    bgt         0b
    vadd.u32    q15, q15, q3
    vadd.u32    q15, q15, q12
    vadd.u32    q15, q15, q13
    vadd.u32    q15, q15, q14
    vadd.u32    d31, d30, d31
    vpadd.u32   d31, d31, d31
    vmov.u32    r0, d31[0]
    bx          lr
.endfunc

asm_function aligned_block_copy_neon
0:
    vld1.8      {d0, d1, d2, d3}, [SRC]!
    vst1.8      {d0, d1, d2, d3}, [DST, :256]!
    subs        SIZE, SIZE, #32
    bgt         0b
    bx          lr
.endfunc

asm_function aligned_block_copy_unrolled_neon
    vpush       {d8-d15}
    bic         SIZE, SIZE, #127
0:
    vld1.64     {q0, q1}, [SRC]!
    vld1.64     {q2, q3}, [SRC]!
    vld1.64     {q4, q5}, [SRC]!
    vld1.64     {q6, q7}, [SRC]!
    vst1.64     {q0, q1}, [DST, :256]!
    vst1.64     {q2, q3}, [DST, :256]!
    vst1.64     {q4, q5}, [DST, :256]!
    vst1.64     {q6, q7}, [DST, :256]!
    subs        SIZE, SIZE, #128
    bgt         0b
    vpop        {d8-d15}
    bx          lr
.endfunc

asm_function aligned_block_copy_pf32_neon
0:
    pld         [SRC, #256]
    vld1.8      {d0, d1, d2, d3}, [SRC]!
    vst1.8      {d0, d1, d2, d3}, [DST, :256]!
    subs        SIZE, SIZE, #32
    bgt         0b
    bx          lr
.endfunc

asm_function aligned_block_copy_unrolled_pf32_neon
    vpush       {d8-d15}
    bic         SIZE, SIZE, #127
0:
    vld1.64     {q0, q1}, [SRC]!
    pld         [SRC, #256]
    pld         [SRC, #256 + 32]
    vld1.64     {q2, q3}, [SRC]!
    pld         [SRC, #512]
    pld         [SRC, #512 + 32]
    vld1.64     {q4, q5}, [SRC]!
    pld         [SRC, #256]
    pld         [SRC, #256 + 32]
    vld1.64     {q6, q7}, [SRC]!
    pld         [SRC, #512]
    pld         [SRC, #512 + 32]
    vst1.64     {q0, q1}, [DST, :256]!
    vst1.64     {q2, q3}, [DST, :256]!
    vst1.64     {q4, q5}, [DST, :256]!
    vst1.64     {q6, q7}, [DST, :256]!
    subs        SIZE, SIZE, #128
    bgt         0b
    vpop        {d8-d15}
    bx          lr
.endfunc

asm_function aligned_block_copy_pf64_neon
0:
    pld         [SRC, #256]
    vld1.8      {d0, d1, d2, d3}, [SRC]!
    vst1.8      {d0, d1, d2, d3}, [DST, :256]!
    vld1.8      {d0, d1, d2, d3}, [SRC]!
    vst1.8      {d0, d1, d2, d3}, [DST, :256]!
    subs        SIZE, SIZE, #64
    bgt         0b
    bx          lr
.endfunc

asm_function aligned_block_copy_unrolled_pf64_neon
    vpush       {d8-d15}
    bic         SIZE, SIZE, #127
0:
    vld1.64     {q0, q1}, [SRC]!
    pld         [SRC, #256]
    vld1.64     {q2, q3}, [SRC]!
    pld         [SRC, #512]
    vld1.64     {q4, q5}, [SRC]!
    pld         [SRC, #256]
    vld1.64     {q6, q7}, [SRC]!
    pld         [SRC, #512]
    vst1.64     {q0, q1}, [DST, :256]!
    vst1.64     {q2, q3}, [DST, :256]!
    vst1.64     {q4, q5}, [DST, :256]!
    vst1.64     {q6, q7}, [DST, :256]!
    subs        SIZE, SIZE, #128
    bgt         0b
    vpop        {d8-d15}
    bx          lr
.endfunc

asm_function aligned_block_copy_backwards_neon
    add         SRC, SRC, SIZE
    add         DST, DST, SIZE
    sub         SRC, SRC, #32
    sub         DST, DST, #32
    mov         r3, #-32
0:
    vld1.8      {d0, d1, d2, d3}, [SRC], r3
    vst1.8      {d0, d1, d2, d3}, [DST, :256], r3
    subs        SIZE, SIZE, #32
    bgt         0b
    bx          lr
.endfunc

asm_function aligned_block_copy_backwards_pf32_neon
    add         SRC, SRC, SIZE
    add         DST, DST, SIZE
    sub         SRC, SRC, #32
    sub         DST, DST, #32
    mov         r3, #-32
0:
    pld         [SRC, #-256]
    vld1.8      {d0, d1, d2, d3}, [SRC], r3
    vst1.8      {d0, d1, d2, d3}, [DST, :256], r3
    subs        SIZE, SIZE, #32
    bgt         0b
    bx          lr
.endfunc

asm_function aligned_block_copy_backwards_pf64_neon
    add         SRC, SRC, SIZE
    add         DST, DST, SIZE
    sub         SRC, SRC, #32
    sub         DST, DST, #32
    mov         r3, #-32
0:
    pld         [SRC, #-256]
    vld1.8      {d0, d1, d2, d3}, [SRC], r3
    vst1.8      {d0, d1, d2, d3}, [DST, :256], r3
    vld1.8      {d0, d1, d2, d3}, [SRC], r3
    vst1.8      {d0, d1, d2, d3}, [DST, :256], r3
    subs        SIZE, SIZE, #64
    bgt         0b
    bx          lr
.endfunc

asm_function aligned_block_fill_neon
    vld1.8      {d0, d1, d2, d3}, [SRC]!
0:
    vst1.8      {d0, d1, d2, d3}, [DST, :256]!
    vst1.8      {d0, d1, d2, d3}, [DST, :256]!
    subs        SIZE, SIZE, #64
    bgt         0b
    bx          lr
.endfunc

asm_function aligned_block_fill_backwards_neon
    add         SRC, SRC, SIZE
    add         DST, DST, SIZE
    sub         SRC, SRC, #32
    sub         DST, DST, #32
    mov         r3, #-32
0:
    vst1.8      {d0, d1, d2, d3}, [DST, :256], r3
    subs        SIZE, SIZE, #32
    bgt         0b
    bx          lr
.endfunc

/* some code for older ARM processors */

asm_function aligned_block_fill_stm4_armv4
    push        {r4-r12, lr}
    ldmia       SRC!, {r4-r11}
0:
    stmia       DST!, {r4-r7}
    stmia       DST!, {r8-r11}
    stmia       DST!, {r4-r7}
    stmia       DST!, {r8-r11}
    subs        SIZE, SIZE, #64
    bgt         0b
    pop         {r4-r12, pc}
.endfunc

asm_function aligned_block_fill_stm8_armv4
    push        {r4-r12, lr}
    ldmia       SRC!, {r4-r11}
0:
    stmia       DST!, {r4-r11}
    stmia       DST!, {r4-r11}
    subs        SIZE, SIZE, #64
    bgt         0b
    pop         {r4-r12, pc}
.endfunc

asm_function aligned_block_fill_strd_armv5te
    push        {r4-r12, lr}
    ldmia       SRC!, {r4-r11}
0:
    strd        r4,  r5,  [DST], #8
    strd        r6,  r7,  [DST], #8
    strd        r8,  r9,  [DST], #8
    strd        r10, r11, [DST], #8
    strd        r4,  r5,  [DST], #8
    strd        r6,  r7,  [DST], #8
    strd        r8,  r9,  [DST], #8
    strd        r10, r11, [DST], #8
    subs        SIZE, SIZE, #64
    bgt         0b
    pop         {r4-r12, pc}
.endfunc

asm_function aligned_block_copy_incr_armv5te
    push        {r4-r12, lr}
0:
    subs        SIZE, SIZE, #64
    ldmia       SRC!, {r4-r11}
    pld         [SRC, #256]
    stmia       DST!, {r4-r7}
    stmia       DST!, {r8-r11}
    ldmia       SRC!, {r4-r11}
    pld         [SRC, #256]
    stmia       DST!, {r4-r7}
    stmia       DST!, {r8-r11}
    bgt         0b
    pop         {r4-r12, pc}
.endfunc

asm_function aligned_block_copy_wrap_armv5te
    push        {r4-r12, lr}
0:
    subs        SIZE, SIZE, #64
    ldmia       SRC!, {r4-r11}
    pld         [SRC, #(256 - 4)]
    stmia       DST!, {r4-r7}
    stmia       DST!, {r8-r11}
    ldmia       SRC!, {r4-r11}
    pld         [SRC, #(256 - 4)]
    stmia       DST!, {r4-r7}
    stmia       DST!, {r8-r11}
    bgt         0b
    pop         {r4-r12, pc}
.endfunc

#endif
