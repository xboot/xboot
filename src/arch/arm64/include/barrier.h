#ifndef __ARM64_BARRIER_H__
#define __ARM64_BARRIER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Read and write memory barrier */
#define mb()		__asm__ __volatile__ ("dsb sy" : : : "memory")
/* Read memory barrier */
#define rmb()		__asm__ __volatile__ ("dsb ld" : : : "memory")
/* Write memory barrier */
#define wmb()		__asm__ __volatile__ ("dsb st" : : : "memory")

/* SMP read and write memory barrier */
#define smp_mb()	__asm__ __volatile__ ("dmb ish" : : : "memory")
/* SMP read memory barrier */
#define smp_rmb()	__asm__ __volatile__ ("dmb ishld" : : : "memory")
/* SMP write memory barrier */
#define smp_wmb()	__asm__ __volatile__ ("dmb ishst" : : : "memory")

#ifdef __cplusplus
}
#endif

#endif /* __ARM64_BARRIER_H__ */
