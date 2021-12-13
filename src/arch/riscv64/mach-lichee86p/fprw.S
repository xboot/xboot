#if defined(__riscv_flen)
#if __riscv_flen >= 32
#define fread32(which)	.Lr32_##which: fsw which, 0(a1); ret
#define fwrite32(which)	.Lw32_##which: flw which, 0(a1); ret
	.text
	.align 3
	.globl f32_read
f32_read:
	la a2, .Lr32_t
	andi a0, a0, 31
	slli a0, a0, 1
	add	a0, a0, a2
	lbu	a0, 0(a0)
	add	a0, a0, a2
	jr a0
	.align 3
.Lr32_t:
	.half .Lr32_f0  - .Lr32_t
	.half .Lr32_f1  - .Lr32_t
	.half .Lr32_f2  - .Lr32_t
	.half .Lr32_f3  - .Lr32_t
	.half .Lr32_f4  - .Lr32_t
	.half .Lr32_f5  - .Lr32_t
	.half .Lr32_f6  - .Lr32_t
	.half .Lr32_f7  - .Lr32_t
	.half .Lr32_f8  - .Lr32_t
	.half .Lr32_f9  - .Lr32_t
	.half .Lr32_f10 - .Lr32_t
	.half .Lr32_f11 - .Lr32_t
	.half .Lr32_f12 - .Lr32_t
	.half .Lr32_f13 - .Lr32_t
	.half .Lr32_f14 - .Lr32_t
	.half .Lr32_f15 - .Lr32_t
	.half .Lr32_f16 - .Lr32_t
	.half .Lr32_f17 - .Lr32_t
	.half .Lr32_f18 - .Lr32_t
	.half .Lr32_f19 - .Lr32_t
	.half .Lr32_f20 - .Lr32_t
	.half .Lr32_f21 - .Lr32_t
	.half .Lr32_f22 - .Lr32_t
	.half .Lr32_f23 - .Lr32_t
	.half .Lr32_f24 - .Lr32_t
	.half .Lr32_f25 - .Lr32_t
	.half .Lr32_f26 - .Lr32_t
	.half .Lr32_f27 - .Lr32_t
	.half .Lr32_f28 - .Lr32_t
	.half .Lr32_f29 - .Lr32_t
	.half .Lr32_f30 - .Lr32_t
	.half .Lr32_f31 - .Lr32_t
	fread32(f0)
	fread32(f1)
	fread32(f2)
	fread32(f3)
	fread32(f4)
	fread32(f5)
	fread32(f6)
	fread32(f7)
	fread32(f8)
	fread32(f9)
	fread32(f10)
	fread32(f11)
	fread32(f12)
	fread32(f13)
	fread32(f14)
	fread32(f15)
	fread32(f16)
	fread32(f17)
	fread32(f18)
	fread32(f19)
	fread32(f20)
	fread32(f21)
	fread32(f22)
	fread32(f23)
	fread32(f24)
	fread32(f25)
	fread32(f26)
	fread32(f27)
	fread32(f28)
	fread32(f29)
	fread32(f30)
	fread32(f31)

	.align 3
	.globl f32_write
f32_write:
	la a2, .Lw32_t
	andi a0, a0, 31
	slli a0, a0, 1
	add	a0, a0, a2
	lbu	a0, 0(a0)
	add	a0, a0, a2
	jr a0
	.align 3
.Lw32_t:
	.half .Lw32_f0  - .Lw32_t
	.half .Lw32_f1  - .Lw32_t
	.half .Lw32_f2  - .Lw32_t
	.half .Lw32_f3  - .Lw32_t
	.half .Lw32_f4  - .Lw32_t
	.half .Lw32_f5  - .Lw32_t
	.half .Lw32_f6  - .Lw32_t
	.half .Lw32_f7  - .Lw32_t
	.half .Lw32_f8  - .Lw32_t
	.half .Lw32_f9  - .Lw32_t
	.half .Lw32_f10 - .Lw32_t
	.half .Lw32_f11 - .Lw32_t
	.half .Lw32_f12 - .Lw32_t
	.half .Lw32_f13 - .Lw32_t
	.half .Lw32_f14 - .Lw32_t
	.half .Lw32_f15 - .Lw32_t
	.half .Lw32_f16 - .Lw32_t
	.half .Lw32_f17 - .Lw32_t
	.half .Lw32_f18 - .Lw32_t
	.half .Lw32_f19 - .Lw32_t
	.half .Lw32_f20 - .Lw32_t
	.half .Lw32_f21 - .Lw32_t
	.half .Lw32_f22 - .Lw32_t
	.half .Lw32_f23 - .Lw32_t
	.half .Lw32_f24 - .Lw32_t
	.half .Lw32_f25 - .Lw32_t
	.half .Lw32_f26 - .Lw32_t
	.half .Lw32_f27 - .Lw32_t
	.half .Lw32_f28 - .Lw32_t
	.half .Lw32_f29 - .Lw32_t
	.half .Lw32_f30 - .Lw32_t
	.half .Lw32_f31 - .Lw32_t
	fwrite32(f0)
	fwrite32(f1)
	fwrite32(f2)
	fwrite32(f3)
	fwrite32(f4)
	fwrite32(f5)
	fwrite32(f6)
	fwrite32(f7)
	fwrite32(f8)
	fwrite32(f9)
	fwrite32(f10)
	fwrite32(f11)
	fwrite32(f12)
	fwrite32(f13)
	fwrite32(f14)
	fwrite32(f15)
	fwrite32(f16)
	fwrite32(f17)
	fwrite32(f18)
	fwrite32(f19)
	fwrite32(f20)
	fwrite32(f21)
	fwrite32(f22)
	fwrite32(f23)
	fwrite32(f24)
	fwrite32(f25)
	fwrite32(f26)
	fwrite32(f27)
	fwrite32(f28)
	fwrite32(f29)
	fwrite32(f30)
	fwrite32(f31)
#endif

#if __riscv_flen >= 64
#define fread64(which)	.Lr64_##which: fsd which, 0(a1); ret
#define fwrite64(which)	.Lw64_##which: fld which, 0(a1); ret
	.text
	.align 3
	.globl f64_read
f64_read:
	la a2, .Lr64_t
	andi a0, a0, 31
	slli a0, a0, 1
	add	a0, a0, a2
	lbu	a0, 0(a0)
	add	a0, a0, a2
	jr a0
	.align 3
.Lr64_t:
	.half .Lr64_f0  - .Lr64_t
	.half .Lr64_f1  - .Lr64_t
	.half .Lr64_f2  - .Lr64_t
	.half .Lr64_f3  - .Lr64_t
	.half .Lr64_f4  - .Lr64_t
	.half .Lr64_f5  - .Lr64_t
	.half .Lr64_f6  - .Lr64_t
	.half .Lr64_f7  - .Lr64_t
	.half .Lr64_f8  - .Lr64_t
	.half .Lr64_f9  - .Lr64_t
	.half .Lr64_f10 - .Lr64_t
	.half .Lr64_f11 - .Lr64_t
	.half .Lr64_f12 - .Lr64_t
	.half .Lr64_f13 - .Lr64_t
	.half .Lr64_f14 - .Lr64_t
	.half .Lr64_f15 - .Lr64_t
	.half .Lr64_f16 - .Lr64_t
	.half .Lr64_f17 - .Lr64_t
	.half .Lr64_f18 - .Lr64_t
	.half .Lr64_f19 - .Lr64_t
	.half .Lr64_f20 - .Lr64_t
	.half .Lr64_f21 - .Lr64_t
	.half .Lr64_f22 - .Lr64_t
	.half .Lr64_f23 - .Lr64_t
	.half .Lr64_f24 - .Lr64_t
	.half .Lr64_f25 - .Lr64_t
	.half .Lr64_f26 - .Lr64_t
	.half .Lr64_f27 - .Lr64_t
	.half .Lr64_f28 - .Lr64_t
	.half .Lr64_f29 - .Lr64_t
	.half .Lr64_f30 - .Lr64_t
	.half .Lr64_f31 - .Lr64_t
	fread64(f0)
	fread64(f1)
	fread64(f2)
	fread64(f3)
	fread64(f4)
	fread64(f5)
	fread64(f6)
	fread64(f7)
	fread64(f8)
	fread64(f9)
	fread64(f10)
	fread64(f11)
	fread64(f12)
	fread64(f13)
	fread64(f14)
	fread64(f15)
	fread64(f16)
	fread64(f17)
	fread64(f18)
	fread64(f19)
	fread64(f20)
	fread64(f21)
	fread64(f22)
	fread64(f23)
	fread64(f24)
	fread64(f25)
	fread64(f26)
	fread64(f27)
	fread64(f28)
	fread64(f29)
	fread64(f30)
	fread64(f31)

	.align 3
	.globl f64_write
f64_write:
	la a2, .Lw64_t
	andi a0, a0, 31
	slli a0, a0, 1
	add	a0, a0, a2
	lbu	a0, 0(a0)
	add	a0, a0, a2
	jr a0
	.align 3
.Lw64_t:
	.half .Lw64_f0  - .Lw64_t
	.half .Lw64_f1  - .Lw64_t
	.half .Lw64_f2  - .Lw64_t
	.half .Lw64_f3  - .Lw64_t
	.half .Lw64_f4  - .Lw64_t
	.half .Lw64_f5  - .Lw64_t
	.half .Lw64_f6  - .Lw64_t
	.half .Lw64_f7  - .Lw64_t
	.half .Lw64_f8  - .Lw64_t
	.half .Lw64_f9  - .Lw64_t
	.half .Lw64_f10 - .Lw64_t
	.half .Lw64_f11 - .Lw64_t
	.half .Lw64_f12 - .Lw64_t
	.half .Lw64_f13 - .Lw64_t
	.half .Lw64_f14 - .Lw64_t
	.half .Lw64_f15 - .Lw64_t
	.half .Lw64_f16 - .Lw64_t
	.half .Lw64_f17 - .Lw64_t
	.half .Lw64_f18 - .Lw64_t
	.half .Lw64_f19 - .Lw64_t
	.half .Lw64_f20 - .Lw64_t
	.half .Lw64_f21 - .Lw64_t
	.half .Lw64_f22 - .Lw64_t
	.half .Lw64_f23 - .Lw64_t
	.half .Lw64_f24 - .Lw64_t
	.half .Lw64_f25 - .Lw64_t
	.half .Lw64_f26 - .Lw64_t
	.half .Lw64_f27 - .Lw64_t
	.half .Lw64_f28 - .Lw64_t
	.half .Lw64_f29 - .Lw64_t
	.half .Lw64_f30 - .Lw64_t
	.half .Lw64_f31 - .Lw64_t
	fwrite64(f0)
	fwrite64(f1)
	fwrite64(f2)
	fwrite64(f3)
	fwrite64(f4)
	fwrite64(f5)
	fwrite64(f6)
	fwrite64(f7)
	fwrite64(f8)
	fwrite64(f9)
	fwrite64(f10)
	fwrite64(f11)
	fwrite64(f12)
	fwrite64(f13)
	fwrite64(f14)
	fwrite64(f15)
	fwrite64(f16)
	fwrite64(f17)
	fwrite64(f18)
	fwrite64(f19)
	fwrite64(f20)
	fwrite64(f21)
	fwrite64(f22)
	fwrite64(f23)
	fwrite64(f24)
	fwrite64(f25)
	fwrite64(f26)
	fwrite64(f27)
	fwrite64(f28)
	fwrite64(f29)
	fwrite64(f30)
	fwrite64(f31)
#endif
#endif
