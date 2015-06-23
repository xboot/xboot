#ifndef __X64_BARRIER_H__
#define __X64_BARRIER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Read & Write Memory barrier */
#define mb()		__asm__ __volatile__ ("mfence\n");

/* Read Memory barrier */
#define rmb()		__asm__ __volatile__ ("lfence\n");

/* Write Memory barrier */
#define wmb()		__asm__ __volatile__ ("sfence\n");

/* SMP Read & Write Memory barrier */
#define smp_mb()	__asm__ __volatile__ ("mfence":::"memory");

/* SMP Read Memory barrier */
#define smp_rmb()	__asm__ __volatile__ ("lfence":::"memory");

/* SMP Write Memory barrier */
#define smp_wmb()	__asm__ __volatile__ ("sfence":::"memory");

#ifdef __cplusplus
}
#endif

#endif /* __X64_BARRIER_H__ */
