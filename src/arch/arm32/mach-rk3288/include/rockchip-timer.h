#ifndef __ROCKCHIP_TIMER_H__
#define __ROCKCHIP_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

void rockchip_timer_start(virtual_addr_t virt, int irqon, int oneshot);
void rockchip_timer_stop(virtual_addr_t virt);
void rockchip_timer_count(virtual_addr_t virt, u64_t cnt);
u32_t rockchip_timer_read32(virtual_addr_t virt);
u64_t rockchip_timer_read64(virtual_addr_t virt);
void rockchip_timer_irq_clear(virtual_addr_t virt);

#ifdef __cplusplus
}
#endif

#endif /* __ROCKCHIP_TIMER_H__ */
