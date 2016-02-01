#ifndef __ARM32_H__
#define __ARM32_H__

#ifdef __cplusplus
extern "C" {
#endif

static inline u32_t arm32_read_p15_c1(void)
{
	u32_t value;

	__asm__ __volatile__(
		"mrc p15, 0, %0, c1, c0, 0"
		: "=r" (value)
		:
		: "memory");

	return value;
}

static inline void arm32_write_p15_c1(u32_t value)
{
	__asm__ __volatile__(
		"mcr p15, 0, %0, c1, c0, 0"
		:
		: "r" (value)
		: "memory");
	arm32_read_p15_c1();
}

static inline void arm32_interrupt_enable(void)
{
	u32_t tmp;

	__asm__ __volatile__(
		"mrs %0, cpsr\n"
		"bic %0, %0, #(1<<7)\n"
		"msr cpsr_cxsf, %0"
		: "=r" (tmp)
		:
		: "memory");
}

static inline void arm32_interrupt_disable(void)
{
	u32_t tmp;

	__asm__ __volatile__(
		"mrs %0, cpsr\n"
		"orr %0, %0, #(1<<7)\n"
		"msr cpsr_cxsf, %0"
		: "=r" (tmp)
		:
		: "memory");
}

static inline u32_t arm32_smp_processor_id(void)
{
	u32_t tmp;

	__asm__ __volatile__(
		"mrc p15,0,%0,c0,c0,5\n"
		"and %0,%0,#0x3\n"
		: "=r" (tmp)
		:
		: "memory");
	return tmp;
}

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_H__ */
