#ifndef __RISCV64_LINKAGE_H__
#define __RISCV64_LINKAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#if __riscv_xlen == 64
#define LREG	ld
#define SREG	sd
#define LFREG	fld
#define SFREG	fsd
#define REGSZ	8
#define RVPTR	.dword
#elif __riscv_xlen == 32
#define LREG	lw
#define SREG	sw
#define LFREG	flw
#define SFREG	fsw
#define REGSZ	4
#define RVPTR	.word
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_LINKAGE_H__ */
