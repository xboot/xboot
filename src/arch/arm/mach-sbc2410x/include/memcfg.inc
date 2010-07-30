/*
 * mach/mach-sbc2410x/memcfg.inc
 *
 * memory configure for mach-sbc2410x.
 *
 * Copyright (c) 2007-2009  jianjun jiang <jiangjianjun20000@126.com>
 *
 * Change Logs:
 * Date			Author			Notes
 * 2009-01-15	jianjun jiang	the first version
 *
 */

/* bwscon parameter */
	.equ	B0_BWSCON,	0x0		/* read only */
	.equ	B1_BWSCON,	0x1		/* 16 bit */
	.equ	B2_BWSCON,	0x1		/* 16 bit */
	.equ	B3_BWSCON,	0x1		/* 16 bit */
	.equ	B4_BWSCON,	0x1		/* 16 bit */
	.equ	B5_BWSCON,	0x1		/* 16 bit */
	.equ	B6_BWSCON,	0x2		/* 32 bit */
	.equ	B7_BWSCON,	0x2		/* 32 bit */

/* bank0 parameter */
	.equ 	B0_Tacs	,	0x0		/* 0 clk */
	.equ 	B0_Tcos	,	0x0		/* 0 clk */
	.equ 	B0_Tacc	,	0x7		/* 14clk */
	.equ 	B0_Tcoh	,	0x0		/* 0 clk */
	.equ 	B0_Tah	,	0x0		/* 0 clk */
	.equ 	B0_Tacp	,	0x0
	.equ 	B0_PMC	,	0x0		/* normal */

/* bank1 parameter */
	.equ 	B1_Tacs	,	0x0		/* 0 clk */
	.equ 	B1_Tcos	,	0x0		/* 0 clk */
	.equ 	B1_Tacc	,	0x7		/* 14clk */
	.equ 	B1_Tcoh	,	0x0		/* 0 clk */
	.equ 	B1_Tah	,	0x0		/* 0 clk */
	.equ 	B1_Tacp	,	0x0
	.equ 	B1_PMC	,	0x0		/* normal */

/* bank2 parameter */
	.equ 	B2_Tacs	,	0x0		/* 0 clk */
	.equ 	B2_Tcos	,	0x0		/* 0 clk */
	.equ 	B2_Tacc	,	0x7		/* 14clk */
	.equ 	B2_Tcoh	,	0x0		/* 0 clk */
	.equ 	B2_Tah	,	0x0		/* 0 clk */
	.equ 	B2_Tacp	,	0x0
	.equ 	B2_PMC	,	0x0		/* normal */

/* bank3 parameter */
	.equ 	B3_Tacs	,	0x0		/* 0 clk */
	.equ 	B3_Tcos	,	0x0		/* 0 clk */
	.equ 	B3_Tacc	,	0x7		/* 14clk */
	.equ 	B3_Tcoh	,	0x0		/* 0 clk */
	.equ 	B3_Tah	,	0x0		/* 0 clk */
	.equ 	B3_Tacp	,	0x0
	.equ 	B3_PMC	,	0x0		/* normal */

/* bank4 parameter */
	.equ 	B4_Tacs	,	0x0		/* 0 clk */
	.equ 	B4_Tcos	,	0x0		/* 0 clk */
	.equ 	B4_Tacc	,	0x7		/* 14clk */
	.equ 	B4_Tcoh	,	0x0		/* 0 clk */
	.equ 	B4_Tah	,	0x0		/* 0 clk */
	.equ 	B4_Tacp	,	0x0
	.equ 	B4_PMC	,	0x0		/* normal */

/* bank5 parameter */
	.equ 	B5_Tacs	,	0x0		/* 0 clk */
	.equ 	B5_Tcos	,	0x0		/* 0 clk */
	.equ 	B5_Tacc	,	0x7		/* 14clk */
	.equ 	B5_Tcoh	,	0x0		/* 0 clk */
	.equ 	B5_Tah	,	0x0		/* 0 clk */
	.equ 	B5_Tacp	,	0x0
	.equ 	B5_PMC	,	0x0		/* normal */

/* bank6 parameter */
	.equ	B6_MT	,	0x3		/* sdram */
	.equ	B6_Trcd	,	0x1		/* 3clk */
	.equ	B6_SCAN	,	0x1		/* 9bit */

/* bank7 parameter */
	.equ	B7_MT	,	0x3		/* sdram */
	.equ	B7_Trcd	,	0x1		/* 3clk */
	.equ	B7_SCAN	,	0x1		/* 9bit */

/* refresh parameter */
	.equ	REFEN	,	0x1		/* refresh enable */
	.equ	TREFMD	,	0x0		/* CBR(CAS before RAS)/Auto refresh */
	.equ	Trp		,	0x0		/* 2clk */
	.equ	Trc		,	0x3		/* 7clk */
	.equ	Tchr	,	0x2		/* 3clk */
	.equ	REFCNT	,	1113	/* period=15.6us, HCLK=60Mhz, (2048+1-15.6*60) */
