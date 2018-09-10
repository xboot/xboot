#ifndef __RK3128_REG_PMU_H__
#define __RK3128_REG_PMU_H__

#define RK3128_PMU_BASE			(0x100a0000)

#define PMU_WAKEUP_CFG			(0x00)
#define PMU_PWRDN_CON			(0x04)
#define PMU_PWRDN_ST			(0x08)
#define PMU_IDLE_REQ			(0x0c)
#define PMU_IDLE_ST				(0x10)
#define PMU_PWRMODE_CON			(0x14)
#define PMU_PWR_STATE			(0x18)
#define PMU_OSC_CNT				(0x1c)
#define PMU_CORE_PWRDWN_CNT		(0x20)
#define PMU_CORE_PWRUP_CNT		(0x24)
#define PMU_SFT_CON				(0x28)
#define PMU_DDR_SREF_ST			(0x2c)
#define PMU_INT_CON				(0x30)
#define PMU_INT_ST				(0x34)
#define PMU_SYS_REG0			(0x38)
#define PMU_SYS_REG1			(0x3c)
#define PMU_SYS_REG2			(0x40)
#define PMU_SYS_REG3			(0x44)

#endif /* __RK3128_REG_PMU_H__ */
