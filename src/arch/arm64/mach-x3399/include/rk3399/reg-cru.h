#ifndef __RK3399_REG_CRU_H__
#define __RK3399_REG_CRU_H__

#define RK3399_CRU_BASE			(0xff760000)

#define CRU_PLL_CON(id, i)		((id) * 0x20 + ((i) * 4))
#define CRU_CLKSELS_CON(i)		(0x100 + ((i) * 4))
#define CRU_CLKGATES_CON(i)		(0x300 + ((i) * 4))
#define CRU_SOFTRSTS_CON(i)		(0x400 + ((i) * 4))

#define CRU_MISC_CON			(0x50C)
#define CRU_GLB_CNT_TH			(0x508)
#define CRU_GLB_RST_CON			(0x510)
#define CRU_GLB_RST_ST			(0x514)

#define CRU_SDMMC_CON0			(0x580)
#define CRU_SDMMC_CON1			(0x584)
#define CRU_SDIO0_CON0			(0x588)
#define CRU_SDIO0_CON1			(0x58C)
#define CRU_SDIO1_CON0			(0x590)
#define CRU_SDIO1_CON1			(0x594)

#endif /* __RK3399_REG_CRU_H__ */
