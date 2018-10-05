#ifndef __RISCV64_H__
#define __RISCV64_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MSTATUS_UIE			(1 << 0)
#define MSTATUS_SIE			(1 << 1)
#define MSTATUS_MIE			(1 << 3)
#define MSTATUS_UPIE		(1 << 4)
#define MSTATUS_SPIE		(1 << 5)
#define MSTATUS_MPIE		(1 << 7)
#define MSTATUS_SPP			(1 << 8)
#define MSTATUS_MPP			(3 << 11)
#define MSTATUS_FS			(3 << 13)
#define MSTATUS_XS			(3 << 15)
#define MSTATUS_MPRV		(1 << 17)
#define MSTATUS_SUM			(1 << 18)
#define MSTATUS_MXR			(1 << 19)
#define MSTATUS_TVM			(1 << 20)
#define MSTATUS_TW			(1 << 21)
#define MSTATUS_TSR			(1 << 22)
#define MSTATUS32_SD		(1 << 31)
#define MSTATUS_UXL			(3ULL << 32)
#define MSTATUS_SXL			(3ULL << 34)
#define MSTATUS64_SD		(1ULL << 63)

#define MIP_USIP			(1 << 0)
#define MIP_SSIP			(1 << 1)
#define MIP_MSIP			(1 << 3)
#define MIP_UTIP			(1 << 4)
#define MIP_STIP			(1 << 5)
#define MIP_MTIP			(1 << 7)
#define MIP_UEIP			(1 << 8)
#define MIP_SEIP			(1 << 9)
#define MIP_MEIP			(1 << 11)

#define MIE_USIE			(1 << 0)
#define MIE_SSIE			(1 << 1)
#define MIE_MSIE			(1 << 3)
#define MIE_UTIE			(1 << 4)
#define MIE_STIE			(1 << 5)
#define MIE_MTIE			(1 << 7)
#define MIE_UEIE			(1 << 8)
#define MIE_SEIE			(1 << 9)
#define MIE_MEIE			(1 << 11)

#define csr_swap(csr, val)							\
({													\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrrw %0, " #csr ", %1"	\
			      : "=r" (__v) : "rK" (__v)			\
			      : "memory");						\
	__v;											\
})

#define csr_read(csr)								\
({													\
	register unsigned long __v;						\
	__asm__ __volatile__ ("csrr %0, " #csr			\
			      : "=r" (__v) :					\
			      : "memory");						\
	__v;											\
})

#define csr_write(csr, val)							\
({													\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrw " #csr ", %0"		\
			      : : "rK" (__v)					\
			      : "memory");						\
})

#define csr_read_set(csr, val)						\
({													\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrrs %0, " #csr ", %1"	\
			      : "=r" (__v) : "rK" (__v)			\
			      : "memory");						\
	__v;											\
})

#define csr_set(csr, val)							\
({													\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrs " #csr ", %0"		\
			      : : "rK" (__v)					\
			      : "memory");						\
})

#define csr_read_clear(csr, val)					\
({													\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrrc %0, " #csr ", %1"	\
			      : "=r" (__v) : "rK" (__v)			\
			      : "memory");						\
	__v;											\
})

#define csr_clear(csr, val)							\
({													\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrc " #csr ", %0"		\
			      : : "rK" (__v)					\
			      : "memory");						\
})

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_H__ */
