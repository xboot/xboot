/*
 * cp15.c
 */

#include <cp15.h>

/*
 * CR1 bits (CP#15 CR1)
 */
#define CR_M    (1 << 0)        /* MMU enable                           */
#define CR_A    (1 << 1)        /* Alignment abort enable               */
#define CR_C    (1 << 2)        /* Dcache enable                        */
#define CR_W    (1 << 3)        /* Write buffer enable                  */
#define CR_P    (1 << 4)        /* 32-bit exception handler             */
#define CR_D    (1 << 5)        /* 32-bit data address range            */
#define CR_L    (1 << 6)        /* Implementation defined               */
#define CR_B    (1 << 7)        /* Big endian                           */
#define CR_S    (1 << 8)        /* System MMU protection                */
#define CR_R    (1 << 9)        /* ROM MMU protection                   */
#define CR_F    (1 << 10)       /* Implementation defined               */
#define CR_Z    (1 << 11)       /* Branch prediction enable				*/
#define CR_I    (1 << 12)       /* Icache enable                        */
#define CR_V    (1 << 13)       /* Vectors relocated to 0xffff0000      */
#define CR_RR   (1 << 14)       /* Round Robin cache replacement        */
#define CR_L4   (1 << 15)       /* LDR pc can set T bit                 */
#define CR_DT   (1 << 16)
#define CR_IT   (1 << 18)
#define CR_ST   (1 << 19)
#define CR_FI   (1 << 21)       /* Fast interrupt (lower latency mode)  */
#define CR_U    (1 << 22)       /* Unaligned access operation           */
#define CR_XP   (1 << 23)       /* Extended page tables                 */
#define CR_VE   (1 << 24)       /* Vectored interrupts                  */
#define CR_EE   (1 << 25)       /* Exception (Big) Endian               */
#define CR_TRE  (1 << 28)       /* TEX remap enable                     */
#define CR_AFE  (1 << 29)       /* Access flag enable                   */
#define CR_TE   (1 << 30)       /* Thumb exception enable               */

static u32_t read_p15_c1(void)
{
	u32_t value;

	__asm__ __volatile__(
		"mrc p15, 0, %0, c1, c0, 0"
		: "=r" (value)
		:
		: "memory");

	return value;
}

static void write_p15_c1(u32_t value)
{
	__asm__ __volatile__(
		"mcr p15, 0, %0, c1, c0, 0"
		:
		: "r" (value)
		: "memory");

	read_p15_c1();
}

void irq_enable(void)
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

void irq_disable(void)
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

void fiq_enable(void)
{
	u32_t tmp;

	__asm__ __volatile__(
		"mrs %0, cpsr\n"
		"bic %0, %0, #(1<<6)\n"
		"msr cpsr_cxsf, %0"
		: "=r" (tmp)
		:
		: "memory");
}

void fiq_disable(void)
{
	u32_t tmp;

	__asm__ __volatile__(
		"mrs %0, cpsr\n"
		"orr %0, %0, #(1<<6)\n"
		"msr cpsr_cxsf, %0"
		: "=r" (tmp)
		:
		: "memory");
}

void icache_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | CR_I);
}

void icache_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~CR_I);
}

int icache_status(void)
{
	return ((read_p15_c1() & CR_I) != 0);
}

void dcache_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | CR_C);
}

void dcache_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~CR_C);
}

int dcache_status(void)
{
	return ((read_p15_c1() & CR_C) != 0);
}

void wbuffer_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | CR_W);
}

void wbuffer_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~CR_W);
}

int wbuffer_status(void)
{
	return ((read_p15_c1() & CR_W) != 0);
}

void mmu_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | CR_M);
}

void mmu_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~CR_M);
}

int mmu_status(void)
{
	return ((read_p15_c1() & CR_M) != 0);
}

void vic_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | CR_VE);
}

void vic_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~CR_VE);
}

int vic_status(void)
{
	return ((read_p15_c1() & CR_VE) != 0);
}

void branch_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | CR_Z);
}

void branch_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~CR_Z);
}

int branch_status(void)
{
	return ((read_p15_c1() & CR_Z) != 0);
}

void ttb_set(u32_t base)
{
	__asm__ __volatile__(
		"mcr p15, 0, %0, c2, c0, 0"
		:
		: "r" (base)
		: "memory");
}

u32_t ttb_get(void)
{
	u32_t base;

	__asm__ __volatile__(
		"mrc p15, 0, %0, c2, c0, 0"
		: "=r" (base)
		:
		: "memory");

	return base;
}

void domain_set(u32_t domain)
{
	__asm__ __volatile__(
		"mcr p15, 0, %0, c3, c0, 0"
		:
		: "r" (domain)
		: "memory");
}

u32_t domain_get(void)
{
	u32_t domain;

	__asm__ __volatile__(
		"mrc p15, 0, %0, c3, c0, 0"
		: "=r" (domain)
		:
		: "memory");

	return domain;
}

void tlb_invalidate(void)
{
	__asm__ __volatile__(
		"mov r0, #0\n"
		"mcr p15, 0, r0, c7, c10, 4\n"
		"mcr p15, 0, r0, c8, c6, 0\n"
		"mcr p15, 0, r0, c8, c5, 0\n"
		:
		:
		: "r0");
}

u32_t get_cpuid(void)
{
	u32_t tmp;

	__asm__ __volatile__(
		"mrc     p15,0,%0,c0,c0,5\n"
		"and     %0,%0,#0x3\n"
		: "=r" (tmp)
		:
		: "memory");
	return tmp;
}
