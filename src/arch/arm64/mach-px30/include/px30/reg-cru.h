#ifndef __PX30_REG_CRU_H__
#define __PX30_REG_CRU_H__

#define PX30_CRU_BASE			(0xff2b0000)

#define CRU_PLL_CON(id, i)		((id) * 0x20 + ((i) * 4))
#define CRU_CLKSELS_CON(i)		(0x100 + ((i) * 4))
#define CRU_CLKGATES_CON(i)		(0x300 + ((i) * 4))
#define CRU_SOFTRSTS_CON(i)		(0x400 + ((i) * 4))

#define CRU_GLB_CNT_TH			(0x0b0)
#define CRU_GLB_RST_ST			(0x0b4)
#define CRU_GLB_SRST_FST_VALUE	(0x0b8)
#define CRU_GLB_SRST_SND_VALUE	(0x0bc)
#define CRU_GLB_RST_CON			(0x0c0)

#endif /* __PX30_REG_CRU_H__ */
