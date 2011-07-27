/*
 * include/syscfg.inc
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 */

.equ CLK_800_330_165,			1
.equ CLK_1000_400_200,			0

.if (CLK_800_330_165 == 1)
	.equ APLL_MDIV,			(0xc8)
	.equ APLL_PDIV,			(0x6)
	.equ APLL_SDIV,			(0x1)

	.equ MPLL_MDIV,			(0x6e)
	.equ MPLL_PDIV,			(0x4)
	.equ MPLL_SDIV,			(0x1)

	.equ MPLL_AFC_ENB,		0x0
	.equ MPLL_AFC,			0xd
	
.elseif (CLK_1000_400_200 == 1)
	.equ APLL_MDIV,			(0xfa)
	.equ APLL_PDIV,			(0x6)
	.equ APLL_SDIV,			(0x1)

	.equ MPLL_MDIV,			(0xc8)
	.equ MPLL_PDIV,			(0x6)
	.equ MPLL_SDIV,			(0x1)

	.equ MPLL_AFC_ENB,		0x0
	.equ MPLL_AFC,			0x1c
.endif

.equ APLL_CON0_VAL,			((1<<31) | (APLL_MDIV<<16) | (APLL_PDIV<<8) | (APLL_SDIV))
.equ MPLL_CON0_VAL,			((1<<31) | (MPLL_MDIV<<16) | (MPLL_PDIV<<8) | (MPLL_SDIV))
.equ EPLL_CON0_VAL,			((1<<31) | (EPLL_MDIV<<16) | (EPLL_PDIV<<8) | (EPLL_SDIV))
.equ VPLL_CON0_VAL,			((1<<31) | (VPLL_MDIV<<16) | (VPLL_PDIV<<8) | (VPLL_SDIV))

/* APLL_CON1 */
.equ APLL_AFC_ENB,			0x1
.equ APLL_AFC,				0xc
.equ APLL_CON1_VAL,			((APLL_AFC_ENB << 31) | (APLL_AFC))

/* MPLL_CON1 */
.equ MPLL_CON1_VAL,			((MPLL_AFC_ENB << 31) | (MPLL_AFC))

.equ EPLL_MDIV,	0x30
.equ EPLL_PDIV,	0x3
.equ EPLL_SDIV,	0x2
.equ EPLL_K,		0x0
.equ EPLL_CON1_VAL,	(EPLL_K)

.equ VPLL_MDIV,	0x35
.equ VPLL_PDIV,	0x3
.equ VPLL_SDIV,	0x2

.equ VPLL_SSCG_EN,	0x0
.equ VPLL_SEL_PF,	0x0
.equ VPLL_MRR,	0x11
.equ VPLL_MFR,	0x0
.equ VPLL_K,		0x400
.equ VPLL_CON1_VAL,	((VPLL_SSCG_EN <<31) | (VPLL_SEL_PF <<29) | (VPLL_MRR <<24)	| (VPLL_MFR << 16) | (VPLL_K))


/* CLK_SRC_CPU	*/
/* 0 = MOUTAPLL,  1 = SCLKMPLL	*/
.equ MUX_HPM_SEL_MOUTAPLL,	0
.equ MUX_HPM_SEL_SCLKMPLL,	1
.equ MUX_CORE_SEL_MOUTAPLL,	0
.equ MUX_CORE_SEL_SCLKMPLL,	1

/* 0 = FILPLL, 1 = MOUT */
.equ  MUX_MPLL_SEL_FILPLL,	        0
.equ  MUX_MPLL_SEL_MOUTMPLLFOUT,	1

.equ MUX_APLL_SEL_FILPLL,	        0
.equ MUX_APLL_SEL_MOUTMPLLFOUT,	1

.equ CLK_SRC_CPU_VAL_FINPLL,	        ((MUX_HPM_SEL_MOUTAPLL << 20) | (MUX_CORE_SEL_MOUTAPLL <<16) | (MUX_MPLL_SEL_FILPLL << 8) | (MUX_APLL_SEL_FILPLL))
                                
                                
.equ CLK_SRC_CPU_VAL_MOUTMPLLFOUT,    ((MUX_HPM_SEL_MOUTAPLL << 20) | (MUX_CORE_SEL_MOUTAPLL <<16) | (MUX_MPLL_SEL_MOUTMPLLFOUT << 8) | (MUX_APLL_SEL_MOUTMPLLFOUT <<0))
                                        
                                        
/* CLK_DIV_CPU0	*/
.equ APLL_RATIO,			0x1
.equ PCLK_DBG_RATIO,		0x1
.equ ATB_RATIO,				0x3
.equ PERIPH_RATIO,			0x3
.equ COREM1_RATIO,			0x7
.equ COREM0_RATIO,			0x3
.equ CORE_RATIO,			0x0
.equ CLK_DIV_CPU0_VAL,		((APLL_RATIO << 24) | (PCLK_DBG_RATIO << 20) | (ATB_RATIO << 16) | (PERIPH_RATIO <<12) | (COREM1_RATIO << 8) | (COREM0_RATIO << 4) | (CORE_RATIO))
                                
/* CLK_DIV_CPU1	*/
.equ HPM_RATIO,				0x0
.equ COPY_RATIO,			0x3
.equ CLK_DIV_CPU1_VAL,		((HPM_RATIO << 4) | (COPY_RATIO))

/* CLK_SRC_DMC	*/
.equ MUX_PWI_SEL,			0x1
.equ MUX_CORE_TIMERS_SEL,	0x0
.equ MUX_DPHY_SEL,			0x0
.equ MUX_DMC_BUS_SEL,		0x0
.equ CLK_SRC_DMC_VAL,		((MUX_PWI_SEL << 16) | (MUX_CORE_TIMERS_SEL << 12) | (MUX_DPHY_SEL << 8) | (MUX_DMC_BUS_SEL << 4))
                                
/* CLK_DIV_DMC0	*/
.equ CORE_TIMERS_RATIO,		0x1
.equ COPY2_RATIO,			0x3
.equ DMCP_RATIO,			0x1
.equ DMCD_RATIO,			0x1
.equ DMC_RATIO,				0x1
.equ DPHY_RATIO,			0x1
.equ ACP_PCLK_RATIO,		0x1
.equ ACP_RATIO,				0x3
.equ CLK_DIV_DMC0_VAL,		((CORE_TIMERS_RATIO << 28) | (COPY2_RATIO << 24) | (DMCP_RATIO << 20) | (DMCD_RATIO << 16) | (DMC_RATIO << 12) | (DPHY_RATIO << 8) | (ACP_PCLK_RATIO << 4) | (ACP_RATIO))

/* CLK_DIV_DMC1	*/
.equ DPM_RATIO,				0x1
.equ DVSEM_RATIO,			0x1
.equ PWI_RATIO,				0x1
.equ CLK_DIV_DMC1_VAL,		((DPM_RATIO << 24) | (DVSEM_RATIO << 16) | (PWI_RATIO << 8))

/* CLK_SRC_TOP0	*/
.equ MUX_ONENAND_SEL,		0x1
.equ MUX_ACLK_133_SEL,		0x0
.equ MUX_ACLK_160_SEL,		0x0
.equ MUX_ACLK_100_SEL,		0x0
.equ MUX_ACLK_200_SEL,		0x0
.equ MUX_VPLL_SEL,	        0x0
.equ MUX_EPLL_SEL,	        0x0
.equ CLK_SRC_TOP0_VAL,		((MUX_ONENAND_SEL << 28) | (MUX_ACLK_133_SEL << 24) | (MUX_ACLK_160_SEL << 20) | (MUX_ACLK_100_SEL << 16) | (MUX_ACLK_200_SEL << 12) | (MUX_VPLL_SEL << 8) | (MUX_EPLL_SEL << 4))

/* CLK_SRC_TOP1	*/
.equ VPLLSRC_SEL,			0x0
.equ CLK_SRC_TOP1_VAL,		(VPLLSRC_SEL)

/* CLK_DIV_TOP	*/
.equ ONENAND_RATIO,			0x0
.equ ACLK_133_RATIO,		0x5
.equ ACLK_160_RATIO,		0x4
.equ ACLK_100_RATIO,		0x7
.equ ACLK_200_RATIO,		0x3
.equ CLK_DIV_TOP_VAL,		((ONENAND_RATIO << 16) | (ACLK_133_RATIO << 12)	| (ACLK_160_RATIO << 8)	| (ACLK_100_RATIO << 4) | (ACLK_200_RATIO))

/* CLK_SRC_LEFTBUS	*/
.equ MUX_GDL_SEL,			0x0
.equ CLK_SRC_LEFTBUS_VAL,	(MUX_GDL_SEL)

/* CLK_DIV_LEFRBUS	*/
.equ GPL_RATIO,				0x1
.equ GDL_RATIO,				0x3
.equ CLK_DIV_LEFRBUS_VAL,	((GPL_RATIO << 4) | (GDL_RATIO))

/* CLK_SRC_RIGHTBUS	*/
.equ MUX_GDR_SEL,			0x0
.equ CLK_SRC_RIGHTBUS_VAL,	(MUX_GDR_SEL)

/* CLK_DIV_RIGHTBUS	*/
.equ GPR_RATIO,				0x1
.equ GDR_RATIO,				0x3
.equ CLK_DIV_RIGHTBUS_VAL,	((GPR_RATIO << 4) | (GDR_RATIO))

/* PLL LOCK TIME */
.equ PLL_LOCKTIME,			0x1C20
.equ APLL_LOCK_VAL,			(PLL_LOCKTIME)
.equ MPLL_LOCK_VAL,			(PLL_LOCKTIME)
.equ EPLL_LOCK_VAL,			(PLL_LOCKTIME)
.equ VPLL_LOCK_VAL,			(PLL_LOCKTIME)





				
.equ CLK_400_100_166_133,		0
.equ CLK_400_200_166_133,		0
.equ CLK_533_133_100_100,		0
.equ CLK_667_166_166_133,		0
.equ CLK_800_100_166_133,		0
.equ CLK_800_200_166_133,		0
.equ CLK_1000_200_166_133,		1


.if (CLK_400_100_166_133 == 1)
	.equ APLL_MDIV,				(0x64)
	.equ APLL_PDIV,				(0x3)
	.equ APLL_SDIV,				(0x1)
	.equ MPLL_MDIV,				(0x29b)
	.equ MPLL_PDIV,				(0xc)
	.equ MPLL_SDIV,				(0x1)
	.equ EPLL_MDIV,				(0x60)
	.equ EPLL_PDIV,				(0x6)
	.equ EPLL_SDIV,				(0x2)
	.equ VPLL_MDIV,				(0x6c)
	.equ VPLL_PDIV,				(0x6)
	.equ VPLL_SDIV,				(0x3)

	.equ CLK_DIV0_VAL,			((1<<0) | (7<<4) | (3<<8) | (1<<12) | (3<<16) | (1<<20) | (4<<24) | (1<<28))
	.equ CLK_DIV1_VAL,			((1<<16) | (1<<12) | (1<<8) | (1<<4))
	.equ CLK_DIV2_VAL,			(1<<0)

.elseif (CLK_400_200_166_133 == 1)
	.equ APLL_MDIV,				(0x64)
	.equ APLL_PDIV,				(0x3)
	.equ APLL_SDIV,				(0x1)
	.equ MPLL_MDIV,				(0x29b)
	.equ MPLL_PDIV,				(0xc)
	.equ MPLL_SDIV,				(0x1)
	.equ EPLL_MDIV,				(0x60)
	.equ EPLL_PDIV,				(0x6)
	.equ EPLL_SDIV,				(0x2)
	.equ VPLL_MDIV,				(0x6c)
	.equ VPLL_PDIV,				(0x6)
	.equ VPLL_SDIV,				(0x3)

	.equ CLK_DIV0_VAL,			((0<<0) | (3<<4) | (1<<8) | (1<<12) | (3<<16) | (1<<20) | (4<<24) | (1<<28))
	.equ CLK_DIV1_VAL,			((1<<16) | (1<<12) | (1<<8) | (1<<4))
	.equ CLK_DIV2_VAL,			(1<<0)

.elseif (CLK_533_133_100_100 == 1)
	.equ APLL_MDIV,				(0x215)
	.equ APLL_PDIV,				(0x18)
	.equ APLL_SDIV,				(0x1)
	.equ MPLL_MDIV,				(0x190)
	.equ MPLL_PDIV,				(0x6)
	.equ MPLL_SDIV,				(0x2)
	.equ EPLL_MDIV,				(0x60)
	.equ EPLL_PDIV,				(0x6)
	.equ EPLL_SDIV,				(0x2)
	.equ VPLL_MDIV,				(0x6c)
	.equ VPLL_PDIV,				(0x6)
	.equ VPLL_SDIV,				(0x3)

	.equ CLK_DIV0_VAL,			((0<<0) | (3<<4) | (3<<8) | (1<<12) | (3<<16) | (1<<20) | (3<<24) | (1<<28))
	.equ CLK_DIV1_VAL,			((1<<16) | (1<<12) | (1<<8) | (1<<4))
	.equ CLK_DIV2_VAL,			(1<<0)

.elseif (CLK_667_166_166_133 == 1)
	.equ APLL_MDIV,				(0xfa)
	.equ APLL_PDIV,				(0x6)
	.equ APLL_SDIV,				(0x1)
	.equ MPLL_MDIV,				(0x29b)
	.equ MPLL_PDIV,				(0xc)
	.equ MPLL_SDIV,				(0x1)
	.equ EPLL_MDIV,				(0x60)
	.equ EPLL_PDIV,				(0x6)
	.equ EPLL_SDIV,				(0x2)
	.equ VPLL_MDIV,				(0x6c)
	.equ VPLL_PDIV,				(0x6)
	.equ VPLL_SDIV,				(0x3)

	.equ CLK_DIV0_VAL,			((0<<0) | (3<<4) | (3<<8) | (1<<12) | (3<<16) | (1<<20) | (4<<24) | (1<<28))
	.equ CLK_DIV1_VAL,			((1<<16) | (1<<12) | (1<<8) | (1<<4))
	.equ CLK_DIV2_VAL,			(1<<0)

.elseif (CLK_800_100_166_133 == 1)
	.equ APLL_MDIV,				(0x64)
	.equ APLL_PDIV,				(0x3)
	.equ APLL_SDIV,				(0x1)
	.equ MPLL_MDIV,				(0x29b)
	.equ MPLL_PDIV,				(0xc)
	.equ MPLL_SDIV,				(0x1)
	.equ EPLL_MDIV,				(0x60)
	.equ EPLL_PDIV,				(0x6)
	.equ EPLL_SDIV,				(0x2)
	.equ VPLL_MDIV,				(0x6c)
	.equ VPLL_PDIV,				(0x6)
	.equ VPLL_SDIV,				(0x3)

	.equ CLK_DIV0_VAL,			((0<<0) | (7<<4) | (7<<8) | (1<<12) | (3<<16) | (1<<20) | (4<<24) | (1<<28))
	.equ CLK_DIV1_VAL,			((1<<16) | (1<<12) | (1<<8) | (1<<4))
	.equ CLK_DIV2_VAL,			(1<<0)

.elseif (CLK_800_200_166_133 == 1)
	.equ APLL_MDIV,				(0x64)
	.equ APLL_PDIV,				(0x3)
	.equ APLL_SDIV,				(0x1)
	.equ MPLL_MDIV,				(0x29b)
	.equ MPLL_PDIV,				(0xc)
	.equ MPLL_SDIV,				(0x1)
	.equ EPLL_MDIV,				(0x60)
	.equ EPLL_PDIV,				(0x6)
	.equ EPLL_SDIV,				(0x2)
	.equ VPLL_MDIV,				(0x6c)
	.equ VPLL_PDIV,				(0x6)
	.equ VPLL_SDIV,				(0x3)

	.equ CLK_DIV0_VAL,			((0<<0) | (3<<4) | (3<<8) | (1<<12) | (3<<16) | (1<<20) | (4<<24) | (1<<28))
	.equ CLK_DIV1_VAL,			((1<<16) | (1<<12) | (1<<8) | (1<<4))
	.equ CLK_DIV2_VAL,			(1<<0)

.elseif (CLK_1000_200_166_133 == 1)
	.equ APLL_MDIV,				(0x7d)
	.equ APLL_PDIV,				(0x3)
	.equ APLL_SDIV,				(0x1)
	.equ MPLL_MDIV,				(0x29b)
	.equ MPLL_PDIV,				(0xc)
	.equ MPLL_SDIV,				(0x1)
	.equ EPLL_MDIV,				(0x60)
	.equ EPLL_PDIV,				(0x6)
	.equ EPLL_SDIV,				(0x2)
	.equ VPLL_MDIV,				(0x6c)
	.equ VPLL_PDIV,				(0x6)
	.equ VPLL_SDIV,				(0x3)

	.equ CLK_DIV0_VAL,			((0<<0) | (4<<4) | (4<<8) | (1<<12) | (3<<16) | (1<<20) | (4<<24) | (1<<28))
	.equ CLK_DIV1_VAL,			((1<<16) | (1<<12) | (1<<8) | (1<<4))
	.equ CLK_DIV2_VAL,			(1<<0)

.endif

.equ APLL_VAL,					((1<<31) | (APLL_MDIV<<16) | (APLL_PDIV<<8) | (APLL_SDIV))
.equ MPLL_VAL,					((1<<31) | (MPLL_MDIV<<16) | (MPLL_PDIV<<8) | (MPLL_SDIV))
.equ EPLL_VAL,					((1<<31) | (EPLL_MDIV<<16) | (EPLL_PDIV<<8) | (EPLL_SDIV))
.equ VPLL_VAL,					((1<<31) | (VPLL_MDIV<<16) | (VPLL_PDIV<<8) | (VPLL_SDIV))


.if (CLK_400_200_166_133 == 1) || (CLK_800_200_166_133 == 1) || (CLK_1000_200_166_133 == 1)
	.equ DMC0_MEMCONFIG_0,		0x20E01323
	.equ DMC0_MEMCONFIG_1,		0x40F01323
	.equ DMC0_TIMINGA_REF,		0x00000618
	.equ DMC0_TIMING_ROW,		0x28233287
	.equ DMC0_TIMING_DATA,		0x23240304
	.equ DMC0_TIMING_PWR,		0x09C80232

	.equ DMC1_MEMCONFIG_0,		0x40C01323
	.equ DMC1_MEMCONFIG_1,		0x00E01323
	.equ DMC1_TIMINGA_REF,		0x00000618
	.equ DMC1_TIMING_ROW,		0x28233289
	.equ DMC1_TIMING_DATA,		0x23240304
	.equ DMC1_TIMING_PWR,		0x08280232

.elseif (CLK_400_100_166_133 == 1) || (CLK_800_100_166_133 == 1)
	.equ DMC0_MEMCONFIG_0,		0x20E01323
	.equ DMC0_MEMCONFIG_1,		0x40F01323
	.equ DMC0_TIMINGA_REF,		0x0000030C
	.equ DMC0_TIMING_ROW,		0x28233287
	.equ DMC0_TIMING_DATA,		0x23240304
	.equ DMC0_TIMING_PWR,		0x09C80232

	.equ DMC1_MEMCONFIG_0,		0x40C01323
	.equ DMC1_MEMCONFIG_1,		0x00E01323
	.equ DMC1_TIMINGA_REF,		0x0000030C
	.equ DMC1_TIMING_ROW,		0x28233289
	.equ DMC1_TIMING_DATA,		0x23240304
	.equ DMC1_TIMING_PWR,		0x08280232

.elseif (CLK_533_133_100_100 == 1)
	.equ DMC0_MEMCONFIG_0,		0x20E01323
	.equ DMC0_MEMCONFIG_1,		0x40F01323
	.equ DMC0_TIMINGA_REF,		0x0000040e
	.equ DMC0_TIMING_ROW,		0x10233206
	.equ DMC0_TIMING_DATA,		0x12130005
	.equDMC0_TIMING_PWR,		0x0E100222

	.equ DMC1_MEMCONFIG_0,		0x40C01323
	.equ DMC1_MEMCONFIG_1,		0x00E01323
	.equ DMC1_TIMINGA_REF,		0x0000040e
	.equ DMC1_TIMING_ROW,		0x10233206
	.equ DMC1_TIMING_DATA,		0x12130005
	.equ DMC1_TIMING_PWR,		0x0E100222

.elseif (CLK_667_166_166_133 == 1)
	.equ DMC0_MEMCONFIG_0,		0x20E01323
	.equ DMC0_MEMCONFIG_1,		0x40F01323
	.equ DMC0_TIMINGA_REF,		0x0000050e
	.equ DMC0_TIMING_ROW,		0x14233287
	.equ DMC0_TIMING_DATA,		0x12130005
	.equ DMC0_TIMING_PWR,		0x0E140222

	.equ DMC1_MEMCONFIG_0,		0x40C01323
	.equ DMC1_MEMCONFIG_1,		0x00E01323
	.equ DMC1_TIMINGA_REF,		0x00000618
	.equ DMC1_TIMING_ROW,		0x11344309
	.equ DMC1_TIMING_DATA,		0x12130005
	.equ DMC1_TIMING_PWR,		0x0E190222

.endif
