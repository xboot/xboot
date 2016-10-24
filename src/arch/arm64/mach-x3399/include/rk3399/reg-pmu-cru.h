#ifndef __RK3399_REG_PMU_CRU_H__
#define __RK3399_REG_PMU_CRU_H__

#define RK3399_PMU_CRU_BASE			(0xff750000)

#define PMU_CRU_PLL_CON(id, i)		((id) * 0x20 + ((i) * 4))
#define PMU_CRU_CLKSELS_CON(i)		(0x080 + ((i) * 4))
#define PMU_CRU_CLKGATES_CON(i)		(0x100 + ((i) * 4))
#define PMU_CRU_SOFTRSTS_CON(i)		(0x110 + ((i) * 4))

#endif /* __RK3399_REG_PMU_CRU_H__ */
