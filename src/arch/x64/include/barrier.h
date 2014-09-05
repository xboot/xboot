#ifndef __X64_BARRIER_H__
#define __X64_BARRIER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define isb()
#define dsb()
#define dmb()

#define mb()		dsb()
#define rmb()		dsb()
#define wmb()		dsb()
#define smp_mb()	dmb()
#define smp_rmb()	dmb()
#define smp_wmb()	dmb()

#ifdef __cplusplus
}
#endif

#endif /* __X64_BARRIER_H__ */
