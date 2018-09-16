#ifndef __RISCV64_BARRIER_H__
#define __RISCV64_BARRIER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Read and write memory barrier */
#define mb()
/* Read memory barrier */
#define rmb()
/* Write memory barrier */
#define wmb()

/* SMP read and write memory barrier */
#define smp_mb()
/* SMP read memory barrier */
#define smp_rmb()
/* SMP write memory barrier */
#define smp_wmb()

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_BARRIER_H__ */
