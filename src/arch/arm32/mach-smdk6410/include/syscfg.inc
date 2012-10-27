/*
 * mach/mach-smdk6410/syscfg.inc
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 */

.equ Startup_APLLdiv,		0
.equ Startup_HCLKx2div,		1
.equ Startup_PCLKdiv,		3
.equ Startup_HCLKdiv,		1
.equ Startup_MPLLdiv,		1

.equ APLL_MDIV,				266
.equ APLL_PDIV,				3
.equ APLL_SDIV,				1

.equ MPLL_MDIV,				266
.equ MPLL_PDIV,				3
.equ MPLL_SDIV,				1

.equ Startup_APLL,			(12000000/(APLL_PDIV<<APLL_SDIV)*APLL_MDIV)
.equ Startup_HCLK,			(Startup_APLL/(Startup_HCLKx2div+1)/(Startup_HCLKdiv+1))
.equ CLK_DIV_VAL,			((Startup_PCLKdiv<<12)|(Startup_HCLKx2div<<9)|(Startup_HCLKdiv<<8)|(Startup_MPLLdiv<<4)|Startup_APLLdiv)
.equ APLL_VAL,				(1<<31 | APLL_MDIV<<16 | APLL_PDIV<<8 | APLL_SDIV)
.equ MPLL_VAL,				(1<<31 | MPLL_MDIV<<16 | MPLL_PDIV<<8 | MPLL_SDIV)

/*
 * memory parameters, mobile ddr (128MB and 256MB)
 */
.equ DMC1_MEM_CFG,			0x0001001a	/* supports one cke control, chip1, burst4, row/Ccolumn bit */
.equ DMC1_MEM_CFG2,			0xB45
.equ DMC1_CHIP0_CFG,		0x050F8
.equ DMC_DDR_32_CFG,		0x0			/* 32bit, ddr */

.equ DDR_tREFRESH,			7800		/* ns */
.equ DDR_tRAS,				45			/* ns (min: 45ns)*/
.equ DDR_tRC,				68			/* ns (min: 67.5ns)*/
.equ DDR_tRCD,				23			/* ns (min: 22.5ns)*/
.equ DDR_tRFC,				80			/* ns (min: 80ns)*/
.equ DDR_tRP, 				23			/* ns (min: 22.5ns)*/
.equ DDR_tRRD,				15			/* ns (min: 15ns)*/
.equ DDR_tWR, 				15			/* ns (min: 15ns)*/
.equ DDR_tXSR,				120			/* ns (min: 120ns)*/
.equ DDR_CASL,				3			/* CAS Latency 3 */

.equ DMC_DDR_BA_EMRS, 		2
.equ DMC_DDR_MEM_CASLAT,	3
.equ DMC_DDR_CAS_LATENCY,	(DDR_CASL<<1)
.equ DMC_DDR_t_DQSS,		1
.equ DMC_DDR_t_MRD,			2
.equ DMC_DDR_t_RAS,			(((Startup_HCLK / 1000 * DDR_tRAS) - 1) / 1000000 + 1)
.equ DMC_DDR_t_RC,			(((Startup_HCLK / 1000 * DDR_tRC) - 1) / 1000000 + 1)
.equ DMC_DDR_t_RCD,			(((Startup_HCLK / 1000 * DDR_tRCD) - 1) / 1000000 + 1)
.equ DMC_DDR_schedule_RCD,	((DMC_DDR_t_RCD - 3) << 3)
.equ DMC_DDR_t_RFC,			(((Startup_HCLK / 1000 * DDR_tRFC) - 1) / 1000000 + 1)
.equ DMC_DDR_schedule_RFC,	((DMC_DDR_t_RFC - 3) << 5)
.equ DMC_DDR_t_RP,			(((Startup_HCLK / 1000 * DDR_tRP) - 1) / 1000000 + 1)
.equ DMC_DDR_schedule_RP,	((DMC_DDR_t_RP - 3) << 3)
.equ DMC_DDR_t_RRD,			(((Startup_HCLK / 1000 * DDR_tRRD) - 1) / 1000000 + 1)
.equ DMC_DDR_t_WR,			(((Startup_HCLK / 1000 * DDR_tWR) - 1) / 1000000 + 1)
.equ DMC_DDR_t_WTR,			2
.equ DMC_DDR_t_XP,			2
.equ DMC_DDR_t_XSR,			(((Startup_HCLK / 1000 * DDR_tXSR) - 1) / 1000000 + 1)
.equ DMC_DDR_t_ESR,			DMC_DDR_t_XSR
.equ DMC_DDR_REFRESH_PRD,	(((Startup_HCLK / 1000 * DDR_tREFRESH) - 1) / 1000000)

/*
 * memory chip direct command
 */
.equ DMC_NOP0, 				0x0c0000
.equ DMC_NOP1,				0x1c0000
.equ DMC_PA0, 				0x000000
.equ DMC_PA1,				0x100000
.equ DMC_AR0, 				0x040000
.equ DMC_AR1, 				0x140000
.equ DMC_SDR_MR0,			0x080032
.equ DMC_SDR_MR1,			0x180032
.equ DMC_DDR_MR0,			0x080162
.equ DMC_DDR_MR1,			0x180162
.equ DMC_mDDR_MR0,			0x080032
.equ DMC_mDDR_MR1,			0x180032
.equ DMC_mSDR_EMR0,			0x0a0000
.equ DMC_mSDR_EMR1,			0x1a0000
.equ DMC_DDR_EMR0,			0x090000
.equ DMC_DDR_EMR1,			0x190000
.equ DMC_mDDR_EMR0,			0x0a0000
.equ DMC_mDDR_EMR1,			0x1a0000
