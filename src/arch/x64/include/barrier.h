#ifndef __X64_BARRIER_H__
#define __X64_BARRIER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Read and write memory barrier */
#define mb()		__asm__ __volatile__ ("mfence\n");
/* Read memory barrier */
#define rmb()		__asm__ __volatile__ ("lfence\n");
/* Write memory barrier */
#define wmb()		__asm__ __volatile__ ("sfence\n");

/* SMP read and write memory barrier */
#define smp_mb()	__asm__ __volatile__ ("mfence" ::: "memory");
/* SMP read memory barrier */
#define smp_rmb()	__asm__ __volatile__ ("lfence" ::: "memory");
/* SMP write memory barrier */
#define smp_wmb()	__asm__ __volatile__ ("sfence" ::: "memory");

#ifdef __cplusplus
}
#endif

#endif /* __X64_BARRIER_H__ */
