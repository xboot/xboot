/*
 * include/syscfg.inc
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 */

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
	.equ DMC0_MEMCONFIG_0,		0x30F01313
	.equ DMC0_MEMCONFIG_1,		0x40F01323
	.equ DMC0_TIMINGA_REF,		0x00000618
	.equ DMC0_TIMING_ROW,		0x28233287
	.equ DMC0_TIMING_DATA,		0x23240304
	.equ DMC0_TIMING_PWR,		0x09C80232

	.equ DMC1_MEMCONFIG_0,		0x40F01313
	.equ DMC1_MEMCONFIG_1,		0x00E01323
	.equ DMC1_TIMINGA_REF,		0x00000618
	.equ DMC1_TIMING_ROW,		0x28233289
	.equ DMC1_TIMING_DATA,		0x23240304
	.equ DMC1_TIMING_PWR,		0x08280232

.elseif (CLK_400_100_166_133 == 1) || (CLK_800_100_166_133 == 1)
	.equ DMC0_MEMCONFIG_0,		0x30F01313
	.equ DMC0_MEMCONFIG_1,		0x40F01323
	.equ DMC0_TIMINGA_REF,		0x0000030C
	.equ DMC0_TIMING_ROW,		0x28233287
	.equ DMC0_TIMING_DATA,		0x23240304
	.equ DMC0_TIMING_PWR,		0x09C80232

	.equ DMC1_MEMCONFIG_0,		0x40F01313
	.equ DMC1_MEMCONFIG_1,		0x00E01323
	.equ DMC1_TIMINGA_REF,		0x0000030C
	.equ DMC1_TIMING_ROW,		0x28233289
	.equ DMC1_TIMING_DATA,		0x23240304
	.equ DMC1_TIMING_PWR,		0x08280232

.elseif (CLK_533_133_100_100 == 1)
	.equ DMC0_MEMCONFIG_0,		0x30F01313
	.equ DMC0_MEMCONFIG_1,		0x40F01323
	.equ DMC0_TIMINGA_REF,		0x0000040e
	.equ DMC0_TIMING_ROW,		0x10233206
	.equ DMC0_TIMING_DATA,		0x12130005
	.equDMC0_TIMING_PWR,		0x0E100222

	.equ DMC1_MEMCONFIG_0,		0x40F01313
	.equ DMC1_MEMCONFIG_1,		0x00E01323
	.equ DMC1_TIMINGA_REF,		0x0000040e
	.equ DMC1_TIMING_ROW,		0x10233206
	.equ DMC1_TIMING_DATA,		0x12130005
	.equ DMC1_TIMING_PWR,		0x0E100222

.elseif (CLK_667_166_166_133 == 1)
	.equ DMC0_MEMCONFIG_0,		0x30F01313
	.equ DMC0_MEMCONFIG_1,		0x40F01323
	.equ DMC0_TIMINGA_REF,		0x0000050e
	.equ DMC0_TIMING_ROW,		0x14233287
	.equ DMC0_TIMING_DATA,		0x12130005
	.equ DMC0_TIMING_PWR,		0x0E140222

	.equ DMC1_MEMCONFIG_0,		0x40F01313
	.equ DMC1_MEMCONFIG_1,		0x00E01323
	.equ DMC1_TIMINGA_REF,		0x00000618
	.equ DMC1_TIMING_ROW,		0x11344309
	.equ DMC1_TIMING_DATA,		0x12130005
	.equ DMC1_TIMING_PWR,		0x0E190222

.endif
