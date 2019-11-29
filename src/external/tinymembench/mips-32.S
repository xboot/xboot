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

#if defined(__mips__) && defined(_ABIO32)

.text
.align 2
.set noreorder
.set nomips16
.set mips32

.macro asm_function function_name
    .global \function_name
    .type \function_name, @function
    .func \function_name
\function_name:
.endm

/*****************************************************************************/

/*
 * void aligned_block_fill_pf32_mips32(int64_t *dst, int64_t *src, int size)
 *
 * Fill memory block at 'dst' with a 8 byte pattern loaded from 'src'.
 * Memory block must be 32 bytes aligned and its size must be a multiple
 * of 64 bytes.
 *
 * Important: the size of cache line *must* be 32 bytes.
 */
asm_function aligned_block_fill_pf32_mips32

    .set DST,   $a0
    .set SRC,   $a1
    .set SIZE,  $a2
    .set LIMIT, $a3

    slti        $t0,    SIZE,   64
    bnez        $t0,    2f
    sra         SIZE,   SIZE,   6
    lw          $t0,    0(SRC)
    sll         SIZE,   SIZE,   6
    lw          $t1,    4(SRC)
    add         LIMIT,  DST,    SIZE
    pref        30,     0(DST)
    addi        LIMIT,  LIMIT,  -64
    b           1f
    pref        30,     32(DST)
0:
    pref        30,     64(DST)
    pref        30,     96(DST)
    addiu       DST,    DST,    64
1:
    nop
    nop
    sw          $t0,    0(DST)
    sw          $t1,    4(DST)
    sw          $t0,    8(DST)
    sw          $t1,    12(DST)
    sw          $t0,    16(DST)
    sw          $t1,    20(DST)
    sw          $t0,    24(DST)
    sw          $t1,    28(DST)
    sw          $t0,    32(DST)
    sw          $t1,    36(DST)
    sw          $t0,    40(DST)
    sw          $t1,    44(DST)
    sw          $t0,    48(DST)
    sw          $t1,    52(DST)
    sw          $t0,    56(DST)
    sw          $t1,    60(DST)
    bne         DST,    LIMIT,  0b
    nop
2:
    jr          $ra
    nop
.endfunc

/*
 * void aligned_block_copy_pf32_mips32(int64_t *dst, int64_t *src, int size)
 *
 * Copy memory block from 'src' to 'dst'. Destination block must be 32 bytes
 * aligned and its size must be a multiple of 64 bytes. Source block must
 * be 4 bytes aligned.
 *
 * Important: the size of cache line *must* be 32 bytes.
 */
asm_function aligned_block_copy_pf32_mips32

    .set DST,   $a0
    .set SRC,   $a1
    .set SIZE,  $a2
    .set LIMIT, $a3

    addi        $sp,    $sp,    -32
    sw          $s0,    0($sp)
    sw          $s1,    4($sp)
    sw          $s2,    8($sp)
    sw          $s3,    12($sp)
    sw          $s4,    16($sp)
    sw          $s5,    20($sp)
    sw          $s6,    24($sp)
    sw          $s7,    28($sp)

    slti        $v0,    SIZE,   64
    bnez        $v0,    2f
    sra         SIZE,   SIZE,   6
    sll         SIZE,   SIZE,   6
    add         LIMIT,  DST,    SIZE
    addi        LIMIT,  LIMIT,  -64
0:
    pref        4,      160(SRC)
    lw          $t0,    0(SRC)
    lw          $t1,    4(SRC)
    lw          $t2,    8(SRC)
    lw          $t3,    12(SRC)
    pref        4,      192(SRC)
    lw          $t4,    16(SRC)
    lw          $t5,    20(SRC)
    lw          $t6,    24(SRC)
    lw          $t7,    28(SRC)
    pref        30,     0(DST)
    lw          $s0,    32(SRC)
    lw          $s1,    36(SRC)
    lw          $s2,    40(SRC)
    lw          $s3,    44(SRC)
    pref        30,     32(DST)
    lw          $s4,    48(SRC)
    lw          $s5,    52(SRC)
    lw          $s6,    56(SRC)
    lw          $s7,    60(SRC)
    addiu       SRC,    SRC,    64
    sw          $t0,    0(DST)
    sw          $t1,    4(DST)
    sw          $t2,    8(DST)
    sw          $t3,    12(DST)
    sw          $t4,    16(DST)
    sw          $t5,    20(DST)
    sw          $t6,    24(DST)
    sw          $t7,    28(DST)
    sw          $s0,    32(DST)
    sw          $s1,    36(DST)
    sw          $s2,    40(DST)
    sw          $s3,    44(DST)
    sw          $s4,    48(DST)
    sw          $s5,    52(DST)
    sw          $s6,    56(DST)
    sw          $s7,    60(DST)
    bne         DST,    LIMIT,  0b
    addiu       DST,    DST,    64
2:
    lw          $s0,    0($sp)
    lw          $s1,    4($sp)
    lw          $s2,    8($sp)
    lw          $s3,    12($sp)
    lw          $s4,    16($sp)
    lw          $s5,    20($sp)
    lw          $s6,    24($sp)
    lw          $s7,    28($sp)
    jr          $ra
    addi        $sp,    $sp,    32
.endfunc

#endif
