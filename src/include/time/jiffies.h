#ifndef __JIFFIES_H__
#define __JIFFIES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

#define time_after(a, b)		(((s32_t)(b) - (s32_t)(a) < 0))
#define time_before(a, b)		time_after(b, a)
#define time_after_eq(a, b)		(((s32_t)(a) - (s32_t)(b) >= 0))
#define time_before_eq(a, b)	time_after_eq(b, a)

extern volatile int HZ;
extern volatile u32_t jiffies;

u64_t jiffies_to_msecs(const u64_t j);
u64_t jiffies_to_usecs(const u64_t j);
u64_t msecs_to_jiffies(const u64_t m);
u64_t usecs_to_jiffies(const u64_t u);

void subsys_init_jiffies(void);

#ifdef __cplusplus
}
#endif

#endif /* __JIFFIES_H__ */
