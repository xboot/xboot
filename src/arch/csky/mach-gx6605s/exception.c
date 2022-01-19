/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <csky.h>
#include <interrupt/interrupt.h>

void show_regs(struct regs *regs)
{
    printf("General Register:\r\n");
    printf("  PC: 0x%08x PSR: 0x%08x\r\n", regs->pc, regs->psr);
    printf("  A0: 0x%08x A1: 0x%08x A2: 0x%08x A3: 0x%08x\r\n",
            regs->a0, regs->a1, regs->a2, regs->a3);
    printf("  A4: 0x%08x A5: 0x%08x L0: 0x%08x L1: 0x%08x\r\n",
            regs->a4, regs->a5, regs->l0, regs->l1);
    printf("  L2: 0x%08x L3: 0x%08x L4: 0x%08x L5: 0x%08x\r\n",
            regs->l2, regs->l3, regs->l4, regs->l5);
    printf("  R1: 0x%08x SP: 0x%08x LR: 0x%08x GB: 0x%08x\r\n",
            regs->r1, regs->sp, regs->lr, regs->gb);

    printf("Control Register:\r\n");
    printf("  VBR: 0x%08x SS0: 0x%08x SS1: 0x%08x SS2: 0x%08x\r\n",
            mfcr("cr1"), mfcr("cr6"), mfcr("cr7"), mfcr("cr8"));
    printf("  SS3: 0x%08x SS4: 0x%08x GCR: 0x%08x GSR: 0x%08x\r\n",
            mfcr("cr9"), mfcr("cr10"), mfcr("cr11"), mfcr("cr12"));
    printf("  IDR: 0x%08x DCS: 0x%08x CWR: 0x%08x CFR: 0x%08x\r\n",
            mfcr("cr13"), mfcr("cr14"), mfcr("cr15"), mfcr("cr17"));
    printf("  CCR: 0x%08x CPR: 0x%08x PCR: 0x%08x PSR: 0x%08x\r\n",
            mfcr("cr18"), mfcr("cr19"), mfcr("cr20"), mfcr("cr21"));

    printf("MMU Register:\r\n");
    printf("  MIR: 0x%08x MRR: 0x%08x ME0: 0x%08x ME1: 0x%08x\r\n",
            cprcr("cpcr0"), cprcr("cpcr1"), cprcr("cpcr2"), cprcr("cpcr3"));
    printf("  MEH: 0x%08x MCR: 0x%08x MPR: 0x%08x MWR: 0x%08x\r\n",
            cprcr("cpcr4"), cprcr("cpcr5"), cprcr("cpcr6"), cprcr("cpcr7"));
    printf("  MCIR: 0x%08x ISPM: 0x%08x DSPM: 0x%08x\r\n",
            cprcr("cpcr8"), cprcr("cpcr9"), cprcr("cpcr10"));
    printf("  DMAE: 0x%08x DMAC: 0x%08x DMAS: 0x%08x\r\n",
            cprcr("cpcr11"), cprcr("cpcr12"), cprcr("cpcr13"));
    printf("  DMAO: 0x%08x DMAI: 0x%08x DMAL: 0x%08x\r\n",
            cprcr("cpcr14"), cprcr("cpcr15"), cprcr("cpcr16"));
    printf("  MPGD: 0x%08x SEG0: 0x%08x SEG1: 0x%08x\r\n",
            cprcr("cpcr29"), cprcr("cpcr30"), cprcr("cpcr31"));
}

void show_stack(uint32_t *sp)
{
    printf("Stack dump:\r\n");
    for (int i = 0; i < 8; i++) {
        printf("  [%p]: ", sp);
        for (int i = 0; i < 4; i++)
            printf("0x%08lx ", *sp++);
        printf("\r\n");
    }
}

void do_misaligned_access(struct regs *regs)
{
    printf("\r\npanic: misaligned access\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_access_error(struct regs *regs)
{
    printf("\r\npanic: access error\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_divide_by_zero(struct regs *regs)
{
    printf("\r\npanic: divide by zero\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_illegal_instruction(struct regs *regs)
{
    printf("\r\npanic: divide illegal instruction\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_privilege_violation(struct regs *regs)
{
    printf("\r\npanic: privilege violation\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_trace_exception(struct regs *regs)
{
    printf("\r\npanic: trace\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_breakpoint_exception(struct regs *regs)
{
    printf("\r\npanic: breakpoint\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_unrecoverable_error(struct regs *regs)
{
    printf("\r\npanic: unrecoverable\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_soft_reset(struct regs *regs)
{
    printf("\r\npanic: soft reset\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_hardware_accelerator(struct regs *regs)
{
    printf("\r\npanic: hardware accelerator\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_tlbunrecover_error(struct regs *regs)
{
    printf("\r\npanic: tlb unrecover\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_tlbmiss_error(struct regs *regs)
{
    printf("\r\npanic: tlb miss\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_tlbmodified_error(struct regs *regs)
{
    printf("\r\npanic: tlb modified\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_trap0_exception(struct regs *regs)
{
    printf("\r\npanic: trap0\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_trap1_exception(struct regs *regs)
{
    printf("\r\npanic: trap1\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_trap2_exception(struct regs *regs)
{
    printf("\r\npanic: trap2\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_trap3_exception(struct regs *regs)
{
    printf("\r\npanic: trap3\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_tlbinvalidl_error(struct regs *regs)
{
    printf("\r\npanic: hardware accelerator\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_tlbinvalids_error(struct regs *regs)
{
    printf("\r\npanic: hardware accelerator\n\r");
    show_regs(regs);
    show_stack((uint32_t *)regs->sp);
    proc_halt();
}

void do_normal_autovector_interrupt(struct regs *regs)
{
    interrupt_handle_exception(regs);
}

void do_fast_autovector_interrupt(struct regs *regs)
{
    interrupt_handle_exception(regs);
}
