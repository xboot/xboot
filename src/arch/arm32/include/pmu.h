#ifndef __ARM32_PMU_H__
#define __ARM32_PMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>

enum {
	SOFTWARE_INCREMENT			= 0x00,
	L1ICACHE_MISS				= 0x01,
	L1ITLB_MISS					= 0x02,
	L1DCACHE_MISS				= 0x03,
	L1DCACHE_ACCESS 			= 0x04,
	L1DTLB_MISS 				= 0x05,
	RINSTRUCTION				= 0x06,
	WINSTRUCTION				= 0x07,
	INSTRUCTION					= 0x08,
	EXCEPTION					= 0x09,
	EXCEPTION_RETURN			= 0x0A,
	CONTEXTID					= 0x0B,
	SOFT_PC						= 0x0C,
	IMMEDIATE_BRANCH			= 0x0D,
	PROCEDURE_RETURN			= 0x0E,
	UNALIGNED_LS				= 0x0F,
	MISPREDICTED_BRANCH			= 0x10,
	CYCLE						= 0x11,
	PREDICTABLE_BRANCH			= 0x12,
	DATA_MEM_ACCESS				= 0x13,
	L1ICACHE_ACCESS				= 0x14,
	L1DCACHE_EVICTION			= 0x15,
	L2DCACHE_ACCESS				= 0x16,
	L2DCACHE_MISS				= 0x17,
	L2DCACHE_EVICTION			= 0x18,
	BUS_ACCESS					= 0x19,
	LOCAL_MEM_ERR				= 0x1A,
	INST_SPECULATIVE			= 0x1B,
	INST_TTB					= 0x1C,
	BUS_CYCLE					= 0x1D,
	L1DCACHE_ACCESS_LD			= 0x40,
	L1DCACHE_ACCESS_ST			= 0x41,
	L1DCACHE_MISS_LD			= 0x42,
	L1DCACHE_MISS_ST			= 0x43,
	L1DCACHE_EVICTION_LD		= 0x46,
	L1DCACHE_EVICTION_ST		= 0x47,
	L1DCACHE_INVALID			= 0x48,
	L1DTLB_MISS_LD				= 0x4C,
	L1DTLB_MISS_ST				= 0x4D,
	D2DCACHE_ACCESS_LD			= 0x50,
	D2DCACHE_ACCESS_ST			= 0x51,
	L2DCACHE_MISS_LD			= 0x52,
	L2DCACHE_MISS_ST			= 0x53,
	L2DCACHE_EVICTION_VICTIM	= 0x56,
	L2DCACHE_EVICTION_CLEAN		= 0x57,
	L2DCACHE_INVALID			= 0x58,
	BUS_ACCESS_LD				= 0x60,
	BUS_ACCESS_ST				= 0x61,
	BUS_ACCESS_NORM				= 0x62,
	BUS_ACCESS_ANORM			= 0x63,
	BUS_ACCESS_NORM2			= 0x64,
	BUS_ACCESS_PERI				= 0x65,
	DATA_MEM_ACCESS_LD			= 0x66,
	DATA_MEM_ACCESS_ST			= 0x67,
	UNALIGNED_ACCESS_LD			= 0x68,
	UNALIGNED_ACCESS_ST			= 0x69,
	UNALIGNED_ACCESS			= 0x6A,
	LDREX						= 0x6C,
	STREX_PASS					= 0x6D,
	STREX_FAIL					= 0x6E,
	INSN_SPEC_LD				= 0x70,
	INSN_SPEC_ST				= 0x71,
	INSN_SPEC_LD_ST				= 0x72,
	INSN_SPEC_INT				= 0x73,
	INSN_SPEC_SIMD				= 0x74,
	INSN_SPEC_VFP				= 0x75,
	INSN_SPEC_SOFTPC			= 0x76,
	BRANCH_SPEC_IMMEDIATE		= 0x78,
	BRANCH_SPEC_PROCEDURE		= 0x79,
	BRANCH_SPEC_INDIRECT		= 0x7A,
	BRANCH_SPEC_ISB				= 0x7C,
	BRANCH_SPEC_DSB				= 0x7D,
	BRANCH_SPEC_DMB				= 0x7E,
	IRQ_EXCEPTION				= 0x86,
	FIQ_EXCEPTION				= 0x87,
	EXTERN_MEM_REQ				= 0xC0,
	NONCACHABLE_EXTERN_MEM_REQ	= 0xC1,
	LINEFILL_PREFETCH			= 0xC2,
	LINEFILL_PREFETCH_DROPPED	= 0xC3,
	READ_ALLOCATE_MD			= 0xC4,
	ETM_EXT_OUT0				= 0xC7,
	ETM_EXT_OUT1				= 0xC8,
	WRITE_STL_BUF				= 0xC9,
	DATA_SNOOPED				= 0xCA,
};

static inline void pmu_enable(void)
{
	uint32_t value;

	__asm__ __volatile__("mrc p15, 0, %0, c9, c12, 0" : "=r"(value));
	value |= (1 << 0);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 0" :: "r"(value));
}

static inline void pmu_disable(void)
{
	uint32_t value;

	__asm__ __volatile__("mrc p15, 0, %0, c9, c12, 0" : "=r"(value));
	value &= ~(1 << 0);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 0" :: "r"(value));
}

static inline void pmu_user_enable(void)
{
	uint32_t value;

	__asm__ __volatile__("mrc p15, 0, %0, c9, c14, 0" : "=r"(value));
	value |= (1 << 0);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c14, 0" :: "r"(value));
}

static inline void pmu_user_disable(void)
{
	uint32_t value;

	__asm__ __volatile__("mrc p15, 0, %0, c9, c14, 0" : "=r"(value));
	value &= ~(1 << 0);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c14, 0" :: "r"(value));
}

static inline void pmn_reset(void)
{
	uint32_t value;

	__asm__ __volatile__("mrc p15, 0, %0, c9, c12, 0" : "=r"(value));
	value |= (1 << 1);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 0" :: "r"(value));
}

static inline void pmn_enable(int counter)
{
	uint32_t value = 0x1 << counter;
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 1" :: "r"(value));
}

static inline void pmn_disable(int counter)
{
	uint32_t value = 0x1 << counter;
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 2" :: "r"(value));
}

static inline uint32_t pmn_number(void)
{
	uint32_t value;
	__asm__ __volatile__("mrc p15, 0, %0, c9, c12, 0" : "=r"(value));
	return (value >> 11) & 0x1f;
}

static inline uint32_t pmn_soft_increment(uint32_t counter)
{
	uint32_t value = 0x1 << counter;
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 4" :: "r"(value));
	return value;
}

static inline void pmn_config(uint32_t counter, uint32_t event)
{
	counter &= 0x1f;
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 5" :: "r"(counter));
	__asm__ __volatile__("mcr p15, 0, %0, c9, c13, 1" :: "r"(event));
}

static inline uint32_t pmn_read(uint32_t counter)
{
	uint32_t value;

	counter &= 0x1f;
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 5" :: "r"(counter));
	__asm__ __volatile__("mrc p15, 0, %0, c9, c13, 2" : "=r"(value));
	return value;
}

static inline void pmn_irq_enable(int counter)
{
	uint32_t value = 0x1 << counter;
	__asm__ __volatile__("mcr p15, 0, %0, c9, c14, 1" :: "r"(value));
}

static inline void pmn_irq_disable(int counter)
{
	uint32_t value = 0x1 << counter;
	__asm__ __volatile__("mcr p15, 0, %0, c9, c14, 2" :: "r"(value));
}

static inline void ccnt_reset(void)
{
	uint32_t value;

	__asm__ __volatile__("mrc p15, 0, %0, c9, c12, 0" : "=r"(value));
	value |= (1 << 2);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 0" :: "r"(value));
}

static inline void ccnt_enable(void)
{
	uint32_t value = (1 << 31);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 1" :: "r"(value));
}

static inline void ccnt_disable(void)
{
	uint32_t value = (1 << 31);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 2" :: "r"(value));
}

static inline void ccnt_divider(int divider)
{
	uint32_t value;

	__asm__ __volatile__("mrc p15, 0, %0, c9, c12, 0" : "=r"(value));
	if(divider)
		value |= (1 << 3);
	else
		value &= ~(1 << 3);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 0" :: "r"(value));
}

static inline uint32_t ccnt_read(void)
{
	uint32_t value;
	__asm__ __volatile__("mrc p15, 0, %0, c9, c13, 0" : "=r"(value));
	return value;
}

static inline void ccnt_irq_enable(void)
{
	uint32_t value = (1 << 31);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c14, 1" :: "r"(value));
}

static inline void ccnt_irq_disable(void)
{
	uint32_t value = (1 << 31);
	__asm__ __volatile__("mcr p15, 0, %0, c9, c14, 2" :: "r"(value));
}

static inline uint32_t overflow_read(void)
{
	uint32_t value;
	__asm__ __volatile__("mrc p15, 0, %0, c9, c12, 3" : "=r"(value));
	return value;
}

static inline void overflow_write(uint32_t value)
{
	__asm__ __volatile__("mcr p15, 0, %0, c9, c12, 3" :: "r"(value));
}

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_PMU_H__ */
