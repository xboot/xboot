#ifndef __ARM32_BARRIER_H__
#define __ARM32_BARRIER_H__

#ifdef __cplusplus
extern "C" {
#endif

#if __ARM_ARCH__ >= 7
#define isb()		__asm__ __volatile__ ("isb" : : : "memory")
#define dsb()		__asm__ __volatile__ ("dsb" : : : "memory")
#define dmb()		__asm__ __volatile__ ("dmb" : : : "memory")
#else
#define isb()		__asm__ __volatile__ ("" : : : "memory")
#define dsb()		__asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 4" : : "r" (0) : "memory")
#define dmb()		__asm__ __volatile__ ("" : : : "memory")
#endif

#define mb()		dsb()
#define rmb()		dsb()
#define wmb()		dsb()
#define smp_mb()	dmb()
#define smp_rmb()	dmb()
#define smp_wmb()	dmb()

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_BARRIER_H__ */
