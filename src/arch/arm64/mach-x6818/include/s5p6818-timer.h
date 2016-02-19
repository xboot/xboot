#ifndef __S5P6818_TIMER_H__
#define __S5P6818_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <s5p6818-rstcon.h>
#include <s5p6818/reg-timer.h>

void s5p6818_timer_reset(void);
void s5p6818_timer_enable(virtual_addr_t virt, int ch, int irqon);
void s5p6818_timer_disable(virtual_addr_t virt, int ch);
void s5p6818_timer_start(virtual_addr_t virt, int ch, int oneshot);
void s5p6818_timer_stop(virtual_addr_t virt, int ch);
u64_t s5p6818_timer_calc_tin(virtual_addr_t virt, int ch, u32_t period);
void s5p6818_timer_count(virtual_addr_t virt, int ch, u32_t cnt);
u32_t s5p6818_timer_read(virtual_addr_t virt, int ch);
void s5p6818_timer_irq_clear(virtual_addr_t virt, int ch);

#ifdef __cplusplus
}
#endif

#endif /* __S5P6818_TIMER_H__ */
