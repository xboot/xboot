#include <linkage.h>

	.global memset
	.type memset, %function
	.align 3
memset:
	move t0, a0
	sltiu a3, a2, 16
	bnez a3, 4f
	addi a3, t0, REGSZ - 1
	andi a3, a3, ~(REGSZ - 1)
	beq a3, t0, 2f
	sub a4, a3, t0
1:	sb a1, 0(t0)
	addi t0, t0, 1
	bltu t0, a3, 1b
	sub a2, a2, a4
2:	andi a1, a1, 0xff
	slli a3, a1, 8
	or a1, a3, a1
	slli a3, a1, 16
	or a1, a3, a1
#if __riscv_xlen == 64
	slli a3, a1, 32
	or a1, a3, a1
#endif
	andi a4, a2, ~(REGSZ - 1)
	add a3, t0, a4
	andi a4, a4, 31 * REGSZ
	beqz a4, 3f
	neg a4, a4
	addi a4, a4, 32 * REGSZ
	sub t0, t0, a4
	la a5, 3f
#if __riscv_xlen == 64
	srli a4, a4, 1
#endif
	add a5, a5, a4
	jr a5
3:	SREG a1,          0(t0)
	SREG a1,      REGSZ(t0)
	SREG a1,  2 * REGSZ(t0)
	SREG a1,  3 * REGSZ(t0)
	SREG a1,  4 * REGSZ(t0)
	SREG a1,  5 * REGSZ(t0)
	SREG a1,  6 * REGSZ(t0)
	SREG a1,  7 * REGSZ(t0)
	SREG a1,  8 * REGSZ(t0)
	SREG a1,  9 * REGSZ(t0)
	SREG a1, 10 * REGSZ(t0)
	SREG a1, 11 * REGSZ(t0)
	SREG a1, 12 * REGSZ(t0)
	SREG a1, 13 * REGSZ(t0)
	SREG a1, 14 * REGSZ(t0)
	SREG a1, 15 * REGSZ(t0)
	SREG a1, 16 * REGSZ(t0)
	SREG a1, 17 * REGSZ(t0)
	SREG a1, 18 * REGSZ(t0)
	SREG a1, 19 * REGSZ(t0)
	SREG a1, 20 * REGSZ(t0)
	SREG a1, 21 * REGSZ(t0)
	SREG a1, 22 * REGSZ(t0)
	SREG a1, 23 * REGSZ(t0)
	SREG a1, 24 * REGSZ(t0)
	SREG a1, 25 * REGSZ(t0)
	SREG a1, 26 * REGSZ(t0)
	SREG a1, 27 * REGSZ(t0)
	SREG a1, 28 * REGSZ(t0)
	SREG a1, 29 * REGSZ(t0)
	SREG a1, 30 * REGSZ(t0)
	SREG a1, 31 * REGSZ(t0)
	addi t0, t0, 32 * REGSZ
	bltu t0, a3, 3b
	andi a2, a2, REGSZ - 1
4:	beqz a2, 6f
	add a3, t0, a2
5:	sb a1, 0(t0)
	addi t0, t0, 1
	bltu t0, a3, 5b
6:	ret
