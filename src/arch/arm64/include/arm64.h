#ifndef __ARM64_H__
#define __ARM64_H__

#ifdef __cplusplus
extern "C" {
#endif

#define arm64_read_sysreg(reg)			({ u64_t val; __asm__ __volatile__("mrs %0," #reg :"=r"(val)); val; })
#define arm64_write_sysreg(reg, val)	__asm__ __volatile__("msr " #reg ", %0\n\tdsb sy\n\tisb" ::"r"(val));

static inline void arm64_interrupt_enable(void)
{
	__asm__ __volatile__("msr daifclr, #2" ::: "memory");
}

static inline void arm64_interrupt_disable(void)
{
	__asm__ __volatile__("msr daifset, #2" ::: "memory");
}

static inline int arm64_smp_processor_id(void)
{
	return arm64_read_sysreg(tpidrro_el0);
}

static inline void arm64_timer_start(void)
{
	u64_t ctrl = arm64_read_sysreg(cntp_ctl_el0);
	if(!(ctrl & (1 << 0)))
	{
		ctrl |= (1 << 0);
		arm64_write_sysreg(cntp_ctl_el0, ctrl);
	}
}

static inline void arm64_timer_stop(void)
{
	u64_t ctrl = arm64_read_sysreg(cntp_ctl_el0);
	if((ctrl & (1 << 0)))
	{
		ctrl &= ~(1 << 0);
		arm64_write_sysreg(cntp_ctl_el0, ctrl);
	}
}

static inline void arm64_timer_interrupt_enable(void)
{
	u64_t ctrl = arm64_read_sysreg(cntp_ctl_el0);
	if(ctrl & (1 << 1))
	{
		ctrl &= ~(1 << 1);
		arm64_write_sysreg(cntp_ctl_el0, ctrl);
	}
}

static inline void arm64_timer_interrupt_disable(void)
{
	u64_t ctrl = arm64_read_sysreg(cntp_ctl_el0);
	if(!(ctrl & (1 << 1)))
	{
		ctrl |= (1 << 1);
		arm64_write_sysreg(cntp_ctl_el0, ctrl);
	}
}

static inline u64_t arm64_timer_frequecy(void)
{
	u64_t rate = arm64_read_sysreg(cntfrq_el0);
	return (rate != 0) ? rate : 1000000;
}

static inline u64_t arm64_timer_read(void)
{
	return arm64_read_sysreg(cntpct_el0);
}

static inline void arm64_timer_compare(u64_t interval)
{
	u64_t last = arm64_read_sysreg(cntpct_el0) + interval;
	arm64_write_sysreg(cntp_cval_el0, last);
}

#ifdef __cplusplus
}
#endif

#endif /* __ARM64_H__ */
