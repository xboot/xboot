#ifndef __BCM2837_REG_PM_H__
#define __BCM2837_REG_PM_H__

#define BCM2837_PM_BASE				(0x3f100000)

#define PM_RSTC						(0x1c)
#define PM_RSTS						(0x20)
#define PM_WDOG						(0x24)

#define PM_PASSWORD					0x5a000000
#define PM_WDOG_TIME_SET			0x000fffff
#define PM_RSTC_WRCFG_CLR			0xffffffcf
#define PM_RSTC_WRCFG_SET			0x00000030
#define PM_RSTC_WRCFG_FULL_RESET	0x00000020
#define PM_RSTC_RESET				0x00000102
#define PM_RSTS_HADWRH_SET			0x00000040
#define PM_RSTS_RASPBERRYPI_HALT	0x00000555

#endif /* __BCM2837_REG_PM_H__ */
