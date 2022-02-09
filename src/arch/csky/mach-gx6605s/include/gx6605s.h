/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_H__
#define __GX6605S_H__

#include <xboot.h>

#define MHZ         1000000UL
#define osc_freq    (27 * MHZ)
#define dto_freq    (CONFIG_PRELOAD_DTO_CLK * MHZ)
#define dvb_freq    (CONFIG_PRELOAD_DVB_CLK * MHZ)
#define cpu_freq    (CONFIG_PRELOAD_CPU_CLK * MHZ)
#define axi_freq    (CONFIG_PRELOAD_AXI_CLK * MHZ)
#define ahb_freq    (CONFIG_PRELOAD_AHB_CLK * MHZ)
#define apb_freq    (CONFIG_PRELOAD_APB_CLK * MHZ)
#define dram_freq   (CONFIG_PRELOAD_DRAM_CLK * MHZ)
#define uart_freq   (CONFIG_PRELOAD_UART_CLK)

#define SSEG0_BASE  ((virtual_addr_t)0x80000000)
#define SSEG1_BASE  ((virtual_addr_t)0xa0000000)
#define KVMR_BASE   ((virtual_addr_t)0xc0000000)

#define TIM_BASE    (SSEG1_BASE + 0x0020a000)
#define WDT_BASE    (SSEG1_BASE + 0x0020b000)
#define SPI_BASE    (SSEG1_BASE + 0x00302000)
#define GPIO_BASE   (SSEG1_BASE + 0x00305000)
#define GCTL_BASE   (SSEG1_BASE + 0x0030a000)
#define SER_BASE    (SSEG1_BASE + 0x00400000)
#define DRAMC_BASE  (SSEG1_BASE + 0x00c00000)
#define DRAM_BASE   (SSEG0_BASE + 0x10000000)

#define __startup       __attribute__((__section__(".startup.text")))
#define __startup_data  __attribute__((__section__(".startup.data")))

extern uint8_t _ld_image_start;
extern uint8_t _ld_image_end;
#define IMAGE_SIZE ((uint32_t)&_ld_image_end - (uint32_t)&_ld_image_start)
#define IMAGE_MAGIC 0x55aa55aaU
#define IMAGE_POS sizeof(IMAGE_MAGIC)

static inline virtual_addr_t cache_to_dma(virtual_addr_t cache)
{
    if (cache < SSEG0_BASE || cache >= SSEG1_BASE)
        return 0;
    return (cache - SSEG0_BASE) + SSEG1_BASE;
}

static inline virtual_addr_t dma_to_cache(virtual_addr_t dma)
{
    if (dma < SSEG1_BASE || dma >= KVMR_BASE)
        return 0;
    return (dma - SSEG1_BASE) + SSEG0_BASE;
}

extern __startup void halt(void);
extern __startup void sys_ccu_cpu(uint32_t dto, uint32_t freq);
extern __startup void sys_ccu_axi(uint32_t cpu, uint32_t freq);
extern __startup void sys_ccu_ahb(uint32_t cpu, uint32_t freq);
extern __startup void sys_ccu_apb(uint32_t dto, uint32_t freq);
extern __startup void sys_ccu_dram(uint32_t freq);
extern __startup void sys_ccu_init(void);
extern __startup void sys_dramc_init(void);
extern __startup void sys_uart_putc(char ch);
extern __startup void sys_uart_print(const char *str);
extern __startup void sys_uart_init(uint32_t apb, uint32_t freq);
extern __startup void sys_spinor_init(void);
extern __startup void sys_spinor_read(uint8_t *buff, uint32_t addr, uint32_t len);
extern __startup void tim_mdelay(uint32_t ms);
extern __startup void tim_init(uint32_t freq);

#endif  /* __GX6605S_H__ */
