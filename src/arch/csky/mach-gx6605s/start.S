/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <entry.h>

    .section    .vector, "ax"
ENTRY(_start)
    bsr         init
    bsr         init
    .long       misaligned_access               /* 0x04  Misaligned Access (The bootloader valid flag for nationalchip bootrom) */
    .long       access_error                    /* 0x08  Access Error                                                           */
    .long       divide_by_zero                  /* 0x0c  Divided by Zero                                                        */
    .long       illegal_instruction             /* 0x10  Illegal Instruction                                                    */
    .long       privilege_violation             /* 0x14  Privilege Violation                                                    */
    .long       trace_exception                 /* 0x18  Trace Exception                                                        */
    .long       breakpoint_exception            /* 0x1c  Breakpoint Exception                                                   */
    .long       unrecoverable_error             /* 0x20  Unrecoverable Error                                                    */
    .long       soft_reset                      /* 0x24  Soft Reset                                                             */
    .long       normal_autovector_interrupt     /* 0x28  Normal Autovector Interrupt                                            */
    .long       fast_autovector_interrupt + 1   /* 0x2c  Fast Interrupt Autovector (AF used)                                    */
    .long       hardware_accelerator            /* 0x30  Hardware Accelerator                                                   */
    .long       tlbunrecover_error              /* 0x34  TLB Unrecover Error                                                    */
    .long       tlbmiss_error                   /* 0x38  TLB Miss Error                                                         */
    .long       tlbmodified_error               /* 0x3c  TLB Modified Error                                                     */
    .long       trap0_exception                 /* 0x40  System trap #0                                                         */
    .long       trap1_exception                 /* 0x44  System trap #1                                                         */
    .long       trap2_exception                 /* 0x48  System trap #2                                                         */
    .long       trap3_exception                 /* 0x4c  System trap #3                                                         */
    .long       tlbinvalidl_error               /* 0x50  TLB Invalid Load Error                                                 */
    .long       tlbinvalids_error               /* 0x54  TLB Invalid Save Error                                                 */
END(_start)

    .section    .startup.text, "ax"
ENTRY(init)
    psrclr      ee, fe, ie
    psrclr      af

    lrw         l0, _ld_image_start
    lrw         l1, 0xfffff000
    and         lr, l1
    sub         l0, lr
    cmpnei      l0, 0
    bf          normal_setup

    /* flush v4 I/D caches */
    mfcr        r1, cr17
    bseti       r1, 0
    bseti       r1, 1
    bseti       r1, 4
    mtcr        r1, cr17

    /* setup caches register */
    mfcr        r1, cr18
    bseti       r1, 2
    bclri       r1, 3
    bseti       r1, 5
    bseti       r1, 6
    mtcr        r1, cr18

    /* nop cycles for stability (not necessary) */
    mov         r1, r1
    mov         r1, r1
    mov         r1, r1
    mov         r1, r1

    /* invalid tlb before setup mmu */
    cpseti      cp15
    bgeni       r1, 27
    cpwcr       r1, cpcr8
    bgeni       r1, 26
    cpwcr       r1, cpcr8

    /* set mmu pgd address */
    movi        r1, 0
    cpwcr       r1, cpcr5

    /* set page size (1MiB) */
    lrw         r1, 0xffff << 13
    cpwcr       r1, cpcr6

    /* set virtual start address */
    movi        r1, 0x00
    cpwcr       r1, cpcr4

    /* set page attribute and coresponding physical addresses */
    lrw         r1, (0x000000 << 6) | 0x1f  /* cache able */
    cpwcr       r1, cpcr2
    lrw         r1, (0x100000 << 6) | 0x1f  /* cache able */
    cpwcr       r1, cpcr3

    /* stuffing data to jtlb */
    bgeni       r1, 28
    cpwcr       r1, cpcr8

    /* setup segment regsiters */
    lrw         r1, 0x00000000 | 0x0e   /* cache able */
    cpwcr       r1, cpcr30
    lrw         r1, 0x00000000 | 0x06   /* cache unable */
    cpwcr       r1, cpcr31

    /* enable pld and mb */
    mfcr        r1, cr30
    bseti       r1, 2
    bseti       r1, 3
    mtcr        r1, cr30

    /* enable mmu and cache */
    mfcr        r1, cr18
    bseti       r1, 0
    bclri       r1, 1
    bseti       r1, 2
    bseti       r1, 3
    bseti       r1, 4
    bseti       r1, 5
    bseti       r1, 6
    mtcr        r1, cr18

preload:
    /* initialize preload got table */
    lrw         l1, __sgot_start
    lrw         l2, __sgot_end
    sub         l1, l0
    sub         l2, l0
    br          sgot_loop
1:
    ldw         r1, (l1, 0)
    sub         r1, l0
    stw         r1, (l1, 0)
    addi        l1, 4
sgot_loop:
    cmphs       l1, l2
    bf          1b

    lrw         r1, 0x104000
    mov         sp, r1
    bsr         sys_copyself
    jmpi        _start

normal_setup:
    /* flush all tlb */
    bgeni       r1, 26
    cpwcr       r1, cpcr8

bss_setup:
    /* clear the bss region */
    xor         r1, r1
    lrw         l0, __bss_start
    lrw         l1, __bss_end
    br          bss_loop
1:
    stw         r1, (l0, 0)
    addi        l0, 4
bss_loop:
    cmphs       l0, l1
    bf          1b

main:
    lrw         r1, _start
    mtcr        r1, vbr

    lrw         r1, __stack_exc_end
    mtcr        r1, exc_sp

    lrw         r1, __stack_usr_end
    mov         sp, r1

    psrset      ee, fe, ie
    jsri        xboot_main
    br          halt
END(init)

ENTRY(halt)
    stop
    br          halt
END(halt)

    .section    .text, "ax"
ENTRY(vector)
exception_return:
    REC_REGS
    rte

misaligned_access:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_misaligned_access
    br      exception_return

access_error:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_access_error
    br      exception_return

divide_by_zero:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_divide_by_zero
    br      exception_return

illegal_instruction:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_illegal_instruction
    br      exception_return

privilege_violation:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_privilege_violation
    br      exception_return

trace_exception:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_trace_exception
    br      exception_return

breakpoint_exception:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_breakpoint_exception
    br      exception_return

unrecoverable_error:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_unrecoverable_error
    br      exception_return

soft_reset:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_soft_reset
    br      exception_return

normal_autovector_interrupt:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_normal_autovector_interrupt
    br      exception_return

fast_autovector_interrupt:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_fast_autovector_interrupt
    br      exception_return

hardware_accelerator:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_hardware_accelerator
    br      exception_return

tlbunrecover_error:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_tlbunrecover_error
    br      exception_return

tlbmiss_error:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_tlbmiss_error
    br      exception_return

tlbmodified_error:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_tlbmodified_error
    br      exception_return

trap0_exception:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_trap0_exception
    br      exception_return

trap1_exception:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_trap1_exception
    br      exception_return

trap2_exception:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_trap2_exception
    br      exception_return

trap3_exception:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_trap3_exception
    br      exception_return

tlbinvalidl_error:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_tlbinvalidl_error
    br      exception_return

tlbinvalids_error:
    SAVE_REGS
    psrset  ee
    mov     a0, sp
    jsri    do_tlbinvalids_error
    br      exception_return

END(vector)
