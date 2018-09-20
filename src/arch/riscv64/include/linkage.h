#ifndef __RISCV64_LINKAGE_H__
#define __RISCV64_LINKAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#if __riscv_xlen == 64
#define REG_L	ld
#define REG_S	sd
#define FREG_L	fld
#define FREG_S	fsd
#define SZREG	8
#define LGREG	3
#define RVPTR	.dword
#elif __riscv_xlen == 32
#define REG_L	lw
#define REG_S	sw
#define FREG_L	flw
#define FREG_S	fsw
#define SZREG	4
#define LGREG	2
#define RVPTR	.word
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_LINKAGE_H__ */
