#ifndef __ARM32_BARRIER_H__
#define __ARM32_BARRIER_H__

#ifdef __cplusplus
extern "C" {
#endif

#if __ARM32_ARCH__ == 5
#define isb()		__asm__ __volatile__ ("" : : : "memory")
#define dsb()		__asm__ __volatile__ ("mcr p15, 0, %0, c7, c10,  4" : : "r" (0) : "memory")
#define dmb()		__asm__ __volatile__ ("" : : : "memory")
#elif __ARM32_ARCH__ == 6
#define isb()		__asm__ __volatile__ ("mcr p15, 0, %0, c7, c5,  4" : : "r" (0) : "memory")
#define dsb()		__asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 4" : : "r" (0) : "memory")
#define dmb()		__asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 5" : : "r" (0) : "memory")
#else
#define isb()		__asm__ __volatile__ ("isb" : : : "memory")
#define dsb()		__asm__ __volatile__ ("dsb" : : : "memory")
#define dmb()		__asm__ __volatile__ ("dmb" : : : "memory")
#endif

/* Read and write memory barrier */
#define mb()		dsb()
/* Read memory barrier */
#define rmb()		dsb()
/* Write memory barrier */
#define wmb()		dsb()

/* SMP read and write memory barrier */
#define smp_mb()	dmb()
/* SMP read memory barrier */
#define smp_rmb()	dmb()
/* SMP write memory barrier */
#define smp_wmb()	dmb()

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_BARRIER_H__ */
