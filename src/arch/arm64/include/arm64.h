#ifndef __ARM64_H__
#define __ARM64_H__

#ifdef __cplusplus
extern "C" {
#endif

static inline void arm64_irq_enable(void)
{
    __asm__ __volatile__("msr daifclr, #3" ::: "memory");
}

static inline void arm64_irq_disable(void)
{
    __asm__ __volatile__("msr daifset, #3" ::: "memory");
}

#define arm64_read_sysreg(reg)			({ u64_t val; __asm__ __volatile__("mrs %0," #reg :"=r"(val)); val; })
#define arm64_write_sysreg(reg, val)	__asm__ __volatile__("msr " #reg ", %0\n\tdsb sy\n\tisb" ::"r"(val));

#ifdef __cplusplus
}
#endif

#endif /* __ARM64_H__ */
