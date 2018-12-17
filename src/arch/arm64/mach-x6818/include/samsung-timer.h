#ifndef __SAMSUNG_TIMER_H__
#define __SAMSUNG_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

void samsung_timer_enable(virtual_addr_t virt, int ch, int irqon);
void samsung_timer_disable(virtual_addr_t virt, int ch);
void samsung_timer_start(virtual_addr_t virt, int ch, int oneshot);
void samsung_timer_stop(virtual_addr_t virt, int ch);
u64_t samsung_timer_calc_tin(virtual_addr_t virt, const char * clk, int ch, u32_t period);
void samsung_timer_count(virtual_addr_t virt, int ch, u32_t cnt);
u32_t samsung_timer_read(virtual_addr_t virt, int ch);
void samsung_timer_irq_clear(virtual_addr_t virt, int ch);

#ifdef __cplusplus
}
#endif

#endif /* __SAMSUNG_TIMER_H__ */
