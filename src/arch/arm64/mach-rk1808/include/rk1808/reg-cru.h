#ifndef __RK1808_REG_CRU_H__
#define __RK1808_REG_CRU_H__

#define RK1808_CRU_BASE			(0xff350000)

#define CRU_PLL_CON(id, i)		((id) * 0x20 + ((i) * 4))
#define CRU_CLKSELS_CON(i)		(0x100 + ((i) * 4))
#define CRU_CLKGATES_CON(i)		(0x200 + ((i) * 4))
#define CRU_SOFTRSTS_CON(i)		(0x300 + ((i) * 4))

#define CRU_MODE				(0x0a0)
#define CRU_MISC				(0x0a4)
#define CRU_MISC1				(0x0a8)
#define CRU_GLB_CNT_TH			(0x0b0)
#define CRU_GLB_RST_ST			(0x0b4)
#define CRU_GLB_SRST_FST		(0x0b8)
#define CRU_GLB_SRST_SND		(0x0bc)
#define CRU_GLB_RST_CON			(0x0c0)

#endif /* __RK1808_REG_CRU_H__ */
