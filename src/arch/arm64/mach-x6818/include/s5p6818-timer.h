#ifndef __S5P6818_TIMER_H__
#define __S5P6818_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

void s5p6818_timer_reset(void);
void s5p6818_timer_enable(int ch, int irqon);
void s5p6818_timer_disable(int ch);
void s5p6818_timer_start(int ch, int oneshot);
void s5p6818_timer_stop(int ch);
u64_t s5p6818_timer_calc_tin(int ch, u32_t period);
void s5p6818_timer_count(int ch, u32_t cnt);
u32_t s5p6818_timer_read(int ch);
void s5p6818_timer_irq_clear(int ch);

#ifdef __cplusplus
}
#endif

#endif /* __S5P6818_TIMER_H__ */
