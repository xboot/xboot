#ifndef __R128_REG_CCU_AON_H__
#define __R128_REG_CCU_AON_H__

#define R128_CCU_AON_BASE				(0x4004c400)

#define CCU_HOSC_FREQ_DET				(0x0080)
#define CCU_HOSC_TYPE					(0x0084)
#define CCU_DCXO_CTRL					(0x0088)
#define CCU_DPLL1_CTRL					(0x008c)
#define CCU_DPLL2_CTRL					(0x0090)
#define CCU_DPLL3_CTRL					(0x0094)
#define CCU_AUDIO_PLL_CTRL				(0x0098)
#define CCU_AUDIO_PLL_BIAS				(0x009c)
#define CCU_AUDIO_PATTERN				(0x00a0)
#define CCU_DPLL1_OUT_CONFIG			(0x00a4)
#define CCU_DPLL3_OUT_CONFIG			(0x00a8)
#define CCU_CLK_LDO_CTRL				(0x00ac)
#define CCU_WLAN_BT_RFIP_CTRL			(0x00c4)
#define CCU_MODULE_RST_CTRL				(0x00c8)
#define CCU_MODULE_CLK_EN_CTRL			(0x00cc)
#define CCU_LPUART0_WAKEUP_CTRL			(0x00d0)
#define CCU_LPUART1_WAKEUP_CTRL			(0x00d4)
#define CCU_GPADC_CLK_CTRL				(0x00d8)
#define CCU_AUDIO_CLK_CTRL				(0x00dc)
#define CCU_SYS_CLK_CTRL				(0x00e0)
#define CCU_MAD_LPSD_CLK_CTRL			(0x00e4)
#define CCU_OWA_RX_CLK_CTRL				(0x00e8)
#define CCU_I2S_ASRC_CLK_CTRL			(0x00ec)

#endif /* __R128_REG_CCU_AON_H__ */
