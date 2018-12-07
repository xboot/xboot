#include <linkage.h>

	.global memcpy
	.type memcpy, %function
	.align 3
memcpy:
	move t6, a0
	sltiu a3, a2, 128
	bnez a3, 4f
	andi a3, t6, REGSZ - 1
	andi a4, a1, REGSZ - 1
	bne a3, a4, 4f
	beqz a3, 2f
	andi a3, a1, ~(REGSZ - 1)
	addi a3, a3, REGSZ
	sub a4, a3, a1
1:	lb a5, 0(a1)
	addi a1, a1, 1
	sb a5, 0(t6)
	addi t6, t6, 1
	bltu a1, a3, 1b
	sub a2, a2, a4
2:	andi a4, a2, ~((16 * REGSZ) - 1)
	beqz a4, 4f
	add a3, a1, a4
3:	LREG a4,          0(a1)
	LREG a5,      REGSZ(a1)
	LREG a6,  2 * REGSZ(a1)
	LREG a7,  3 * REGSZ(a1)
	LREG t0,  4 * REGSZ(a1)
	LREG t1,  5 * REGSZ(a1)
	LREG t2,  6 * REGSZ(a1)
	LREG t3,  7 * REGSZ(a1)
	LREG t4,  8 * REGSZ(a1)
	LREG t5,  9 * REGSZ(a1)
	SREG a4,          0(t6)
	SREG a5,      REGSZ(t6)
	SREG a6,  2 * REGSZ(t6)
	SREG a7,  3 * REGSZ(t6)
	SREG t0,  4 * REGSZ(t6)
	SREG t1,  5 * REGSZ(t6)
	SREG t2,  6 * REGSZ(t6)
	SREG t3,  7 * REGSZ(t6)
	SREG t4,  8 * REGSZ(t6)
	SREG t5,  9 * REGSZ(t6)
	LREG a4, 10 * REGSZ(a1)
	LREG a5, 11 * REGSZ(a1)
	LREG a6, 12 * REGSZ(a1)
	LREG a7, 13 * REGSZ(a1)
	LREG t0, 14 * REGSZ(a1)
	LREG t1, 15 * REGSZ(a1)
	addi a1, a1, 16 * REGSZ
	SREG a4, 10 * REGSZ(t6)
	SREG a5, 11 * REGSZ(t6)
	SREG a6, 12 * REGSZ(t6)
	SREG a7, 13 * REGSZ(t6)
	SREG t0, 14 * REGSZ(t6)
	SREG t1, 15 * REGSZ(t6)
	addi t6, t6, 16 * REGSZ
	bltu a1, a3, 3b
	andi a2, a2, (16 * REGSZ) - 1
4:	beqz a2, 6f
	add a3, a1, a2
	or a5, a1, t6
	or a5, a5, a3
	andi a5, a5, 3
	bnez a5, 5f
7:	lw a4, 0(a1)
	addi a1, a1, 4
	sw a4, 0(t6)
	addi t6, t6, 4
	bltu a1, a3, 7b
	ret
5:	lb a4, 0(a1)
	addi a1, a1, 1
	sb a4, 0(t6)
	addi t6, t6, 1
	bltu a1, a3, 5b
6:	ret
