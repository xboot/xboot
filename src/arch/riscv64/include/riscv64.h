#ifndef __RISCV64_H__
#define __RISCV64_H__

#ifdef __cplusplus
extern "C" {
#endif

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
