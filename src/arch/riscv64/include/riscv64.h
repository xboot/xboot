#ifndef __RISCV64_H__
#define __RISCV64_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef STRINGIFY
#define _STRINGIFY(x)	#x
#define STRINGIFY(x)	_STRINGIFY(x)
#endif

#define DEFINE_MPRV_READ_FLAGS(name, type, insn, flags)		\
	static inline type name(type *p)						\
	{														\
		size_t mprv = flags;								\
		type value;											\
		__asm__ __volatile__ (								\
			"csrs mstatus, %1\n"							\
			STRINGIFY(insn) " %0, 0(%2)\n"					\
			"csrc mstatus, %1\n"							\
			: "=&r"(value) : "r"(mprv), "r"(p) : "memory"	\
		);													\
		return value;										\
	}

#define DEFINE_MPRV_READ(name, type, insn) 					\
	DEFINE_MPRV_READ_FLAGS(name, type, insn, 0x00020000)

#define DEFINE_MPRV_READ_MXR(name, type, insn) 				\
	DEFINE_MPRV_READ_FLAGS(name, type, insn, 0x00020000 | 0x00080000)

#define DEFINE_MPRV_WRITE(name, type, insn)					\
	static inline void name(type *p, type value)			\
	{														\
		size_t mprv = 0x00020000;							\
		__asm__ __volatile__ (								\
			"csrs mstatus, %0\n"							\
			STRINGIFY(insn) " %1, 0(%2)\n"					\
			"csrc mstatus, %0\n"							\
			:: "r"(mprv), "r"(value), "r"(p) : "memory"		\
		);													\
	}

DEFINE_MPRV_READ(mprv_read_u8, uint8_t, lbu)
DEFINE_MPRV_READ(mprv_read_u16, uint16_t, lhu)
DEFINE_MPRV_READ(mprv_read_u32, uint32_t, lwu)
DEFINE_MPRV_READ(mprv_read_u64, uint64_t, ld)
DEFINE_MPRV_READ(mprv_read_long, long, ld)
DEFINE_MPRV_READ(mprv_read_ulong, unsigned long, ld)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_u8, uint8_t, lbu)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_u16, uint16_t, lhu)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_u32, uint32_t, lwu)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_u64, uint64_t, ld)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_long, long, ld)
DEFINE_MPRV_READ_MXR(mprv_read_mxr_ulong, unsigned long, ld)
DEFINE_MPRV_WRITE(mprv_write_u8, uint8_t, sb)
DEFINE_MPRV_WRITE(mprv_write_u16, uint16_t, sh)
DEFINE_MPRV_WRITE(mprv_write_u32, uint32_t, sw)
DEFINE_MPRV_WRITE(mprv_write_u64, uint64_t, sd)
DEFINE_MPRV_WRITE(mprv_write_long, long, sd)
DEFINE_MPRV_WRITE(mprv_write_ulong, unsigned long, sd)

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

#define csr_read_mhartid()		csr_read(mhartid)

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_H__ */
