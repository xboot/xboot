#ifndef __ARM64_BARRIER_H__
#define __ARM64_BARRIER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Read & Write Memory barrier */
#define mb()		__asm__ __volatile__ ("dmb sy" : : : "memory")

/* Read Memory barrier */
#define rmb()		__asm__ __volatile__ ("dmb ld" : : : "memory")

/* Write Memory barrier */
#define wmb()		__asm__ __volatile__ ("dmb st" : : : "memory")

/* SMP Read & Write Memory barrier */
#define smp_mb()	__asm__ __volatile__ ("dmb ish" : : : "memory")

/* SMP Read Memory barrier */
#define smp_rmb()	__asm__ __volatile__ ("dmb ishld" : : : "memory")

/* SMP Write Memory barrier */
#define smp_wmb()	__asm__ __volatile__ ("dmb ishst" : : : "memory")

#ifdef __cplusplus
}
#endif

#endif /* __ARM64_BARRIER_H__ */
