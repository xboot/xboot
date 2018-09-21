#include <linkage.h>

	.global setjmp
	.type setjmp, %function
setjmp:
	SREG s0,   0 * REGSZ(a0)
	SREG s1,   1 * REGSZ(a0)
	SREG s2,   2 * REGSZ(a0)
	SREG s3,   3 * REGSZ(a0)
	SREG s4,   4 * REGSZ(a0)
	SREG s5,   5 * REGSZ(a0)
	SREG s6,   6 * REGSZ(a0)
	SREG s7,   7 * REGSZ(a0)
	SREG s8,   8 * REGSZ(a0)
	SREG s9,   9 * REGSZ(a0)
	SREG s10, 10 * REGSZ(a0)
	SREG s11, 11 * REGSZ(a0)
	SREG sp,  12 * REGSZ(a0)
	SREG ra,  13 * REGSZ(a0)
#if __riscv_flen == 32
	fsw fs0,  14 * 4(a0)
	fsw fs1,  15 * 4(a0)
	fsw fs2,  16 * 4(a0)
	fsw fs3,  17 * 4(a0)
	fsw fs4,  18 * 4(a0)
	fsw fs5,  19 * 4(a0)
	fsw fs6,  20 * 4(a0)
	fsw fs7,  21 * 4(a0)
	fsw fs8,  22 * 4(a0)
	fsw fs9,  23 * 4(a0)
	fsw fs10, 24 * 4(a0)
	fsw fs11, 25 * 4(a0)
#elif __riscv_flen == 64
	fsd fs0,  14 * 8(a0)
	fsd fs1,  15 * 8(a0)
	fsd fs2,  16 * 8(a0)
	fsd fs3,  17 * 8(a0)
	fsd fs4,  18 * 8(a0)
	fsd fs5,  19 * 8(a0)
	fsd fs6,  20 * 8(a0)
	fsd fs7,  21 * 8(a0)
	fsd fs8,  22 * 8(a0)
	fsd fs9,  23 * 8(a0)
	fsd fs10, 24 * 8(a0)
	fsd fs11, 25 * 8(a0)
#endif
	li a0, 0
	ret

.global longjmp
.type longjmp, %function
longjmp:
	SREG s0,   0 * REGSZ(a0)
	SREG s1,   1 * REGSZ(a0)
	SREG s2,   2 * REGSZ(a0)
	SREG s3,   3 * REGSZ(a0)
	SREG s4,   4 * REGSZ(a0)
	SREG s5,   5 * REGSZ(a0)
	SREG s6,   6 * REGSZ(a0)
	SREG s7,   7 * REGSZ(a0)
	SREG s8,   8 * REGSZ(a0)
	SREG s9,   9 * REGSZ(a0)
	SREG s10, 10 * REGSZ(a0)
	SREG s11, 11 * REGSZ(a0)
	SREG sp,  12 * REGSZ(a0)
	SREG ra,  13 * REGSZ(a0)
#if __riscv_flen == 32
	flw fs0,  14 * 4(a0)
	flw fs1,  15 * 4(a0)
	flw fs2,  16 * 4(a0)
	flw fs3,  17 * 4(a0)
	flw fs4,  18 * 4(a0)
	flw fs5,  19 * 4(a0)
	flw fs6,  20 * 4(a0)
	flw fs7,  21 * 4(a0)
	flw fs8,  22 * 4(a0)
	flw fs9,  23 * 4(a0)
	flw fs10, 24 * 4(a0)
	flw fs11, 25 * 4(a0)
#elif __riscv_flen == 64
	fld fs0,  14 * 8(a0)
	fld fs1,  15 * 8(a0)
	fld fs2,  16 * 8(a0)
	fld fs3,  17 * 8(a0)
	fld fs4,  18 * 8(a0)
	fld fs5,  19 * 8(a0)
	fld fs6,  20 * 8(a0)
	fld fs7,  21 * 8(a0)
	fld fs8,  22 * 8(a0)
	fld fs9,  23 * 8(a0)
	fld fs10, 24 * 8(a0)
	fld fs11, 25 * 8(a0)
#endif
	mv a0, a1
	bnez a1, 1f
	li a0, 1
1:	ret
