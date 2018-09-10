#ifndef __RK3128_REG_CRU_H__
#define __RK3128_REG_CRU_H__

#define RK3128_CRU_BASE			(0x20000000)

#define CRU_PLL_CON(id, i)		((id) * 0x10 + ((i) * 4))
#define CRU_CLKSELS_CON(i)		(0x044 + ((i) * 4))
#define CRU_CLKGATES_CON(i)		(0x0d0 + ((i) * 4))
#define CRU_SOFTRSTS_CON(i)		(0x110 + ((i) * 4))

#define CRU_MODE_CON			(0x040)
#define CRU_GLB_SRST_FST_VALUE	(0x100)
#define CRU_GLB_SRST_SND_VALUE	(0x104)
#define CRU_MISC_CON			(0x134)
#define CRU_GLB_CNT_TH			(0x140)
#define CRU_GLB_RST_ST			(0x150)
#define CRU_SDMMC_CON0			(0x1c0)
#define CRU_SDMMC_CON1			(0x1c4)
#define CRU_SDIO0_CON0			(0x1c8)
#define CRU_SDIO0_CON1			(0x1cc)
#define CRU_EMMC_CON0			(0x1d8)
#define CRU_EMMC_CON1			(0x1dc)
#define CRU_PLL_MASK_CON		(0x1f0)

#endif /* __RK3128_REG_CRU_H__ */
