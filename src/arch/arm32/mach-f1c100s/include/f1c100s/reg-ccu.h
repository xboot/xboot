#ifndef __F1C100S_REG_CCU_H__
#define __F1C100S_REG_CCU_H__

#define F1C100S_CCU_BASE		(0x01c20000)

#define CCU_PLL_CPU_CTRL		(0x000)
#define CCU_PLL_AUDIO_CTRL		(0x008)
#define CCU_PLL_VIDEO_CTRL		(0x010)
#define CCU_PLL_VE_CTRL			(0x018)
#define CCU_PLL_DDR_CTRL		(0x020)
#define CCU_PLL_PERIPH_CTRL		(0x028)

#define CCU_CPU_AXI_CFG			(0x050)
#define CCU_AHB1_APB1_CFG		(0x054)
#define CCU_APB2_CFG			(0x058)

#endif /* __F1C100S_REG_CCU_H__ */
