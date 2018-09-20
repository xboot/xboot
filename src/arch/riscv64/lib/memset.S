#include <linkage.h>

	.global memset
	.type memset, %function
	.align 4
memset:
	move t0, a0
	sltiu a3, a2, 16
	bnez a3, 4f
	addi a3, t0, SZREG - 1
	andi a3, a3, ~(SZREG - 1)
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
	andi a4, a2, ~(SZREG - 1)
	add a3, t0, a4
	andi a4, a4, 31 * SZREG
	beqz a4, 3f
	neg a4, a4
	addi a4, a4, 32 * SZREG
	sub t0, t0, a4
	la a5, 3f
#if __riscv_xlen == 64
	srli a4, a4, 1
#endif
	add a5, a5, a4
	jr a5
3:	REG_S a1,          0(t0)
	REG_S a1,      SZREG(t0)
	REG_S a1,  2 * SZREG(t0)
	REG_S a1,  3 * SZREG(t0)
	REG_S a1,  4 * SZREG(t0)
	REG_S a1,  5 * SZREG(t0)
	REG_S a1,  6 * SZREG(t0)
	REG_S a1,  7 * SZREG(t0)
	REG_S a1,  8 * SZREG(t0)
	REG_S a1,  9 * SZREG(t0)
	REG_S a1, 10 * SZREG(t0)
	REG_S a1, 11 * SZREG(t0)
	REG_S a1, 12 * SZREG(t0)
	REG_S a1, 13 * SZREG(t0)
	REG_S a1, 14 * SZREG(t0)
	REG_S a1, 15 * SZREG(t0)
	REG_S a1, 16 * SZREG(t0)
	REG_S a1, 17 * SZREG(t0)
	REG_S a1, 18 * SZREG(t0)
	REG_S a1, 19 * SZREG(t0)
	REG_S a1, 20 * SZREG(t0)
	REG_S a1, 21 * SZREG(t0)
	REG_S a1, 22 * SZREG(t0)
	REG_S a1, 23 * SZREG(t0)
	REG_S a1, 24 * SZREG(t0)
	REG_S a1, 25 * SZREG(t0)
	REG_S a1, 26 * SZREG(t0)
	REG_S a1, 27 * SZREG(t0)
	REG_S a1, 28 * SZREG(t0)
	REG_S a1, 29 * SZREG(t0)
	REG_S a1, 30 * SZREG(t0)
	REG_S a1, 31 * SZREG(t0)
	addi t0, t0, 32 * SZREG
	bltu t0, a3, 3b
	andi a2, a2, SZREG - 1
4:	beqz a2, 6f
	add a3, t0, a2
5:	sb a1, 0(t0)
	addi t0, t0, 1
	bltu t0, a3, 5b
6:	ret
