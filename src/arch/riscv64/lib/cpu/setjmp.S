	.text
	.global setjmp
	.type setjmp, %function
setjmp:
	sd s0, 0(a0)
	sd s1, 8(a0)
	sd s2, 16(a0)
	sd s3, 24(a0)
	sd s4, 32(a0)
	sd s5, 40(a0)
	sd s6, 48(a0)
	sd s7, 56(a0)
	sd s8, 64(a0)
	sd s9, 72(a0)
	sd s10, 80(a0)
	sd s11, 88(a0)
	sd sp, 96(a0)
	sd ra, 104(a0)
#ifndef __riscv_soft_float
	fsd fs0, 112(a0)
	fsd fs1, 120(a0)
	fsd fs2, 128(a0)
	fsd fs3, 136(a0)
	fsd fs3, 144(a0)
	fsd fs4, 152(a0)
	fsd fs5, 160(a0)
	fsd fs6, 168(a0)
	fsd fs7, 176(a0)
	fsd fs8, 184(a0)
	fsd fs9, 192(a0)
	fsd fs10, 200(a0)
	fsd fs11, 208(a0)
#endif
	li a0, 0
	ret

.text
.global longjmp
.type longjmp, %function
longjmp:
	ld s0, 0(a0)
	ld s1, 8(a0)
	ld s2, 16(a0)
	ld s3, 24(a0)
	ld s4, 32(a0)
	ld s5, 40(a0)
	ld s6, 48(a0)
	ld s7, 56(a0)
	ld s8, 64(a0)
	ld s9, 72(a0)
	ld s10, 80(a0)
	ld s11, 88(a0)
	ld sp, 96(a0)
	ld ra, 104(a0)
#ifndef __riscv_soft_float
	fld fs0, 112(a0)
	fld fs1, 120(a0)
	fld fs2, 128(a0)
	fld fs3, 136(a0)
	fld fs3, 144(a0)
	fld fs4, 152(a0)
	fld fs5, 160(a0)
	fld fs6, 168(a0)
	fld fs7, 176(a0)
	fld fs8, 184(a0)
	fld fs9, 192(a0)
	fld fs10, 200(a0)
	fld fs11, 208(a0)
#endif
	mv a0, a1
	li a0, 1
1:	ret
