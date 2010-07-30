/*
 * mach/mach-smdkc100/syscfg.inc
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 */


.equ APLL_MDIV,				(444)
.equ APLL_PDIV,				(4)
.equ APLL_SDIV,				(0)
.equ MPLL_MDIV,				(89)
.equ MPLL_PDIV,				(2)
.equ MPLL_SDIV,				(1)
.equ EPLL_MDIV,				(135)
.equ EPLL_PDIV,				(3)
.equ EPLL_SDIV,				(3)
.equ HPLL_MDIV,				(96)
.equ HPLL_PDIV,				(6)
.equ HPLL_SDIV,				(3)

.equ APLL_VAL,				((1<<31) | (APLL_MDIV<<16) | (APLL_PDIV<<8) | (APLL_SDIV))
.equ MPLL_VAL,				((1<<31) | (MPLL_MDIV<<16) | (MPLL_PDIV<<8) | (MPLL_SDIV))
.equ EPLL_VAL,				((1<<31) | (EPLL_MDIV<<16) | (EPLL_PDIV<<8) | (EPLL_SDIV))
.equ HPLL_VAL,				((1<<31) | (HPLL_MDIV<<16) | (HPLL_PDIV<<8) | (HPLL_SDIV))

.equ CLK_DIV0_VAL,			((1<<16) | (1<<12) | (3<<8) | (0<<4) | (1<<0))
.equ CLK_DIV1_VAL,			((1<<24) | (0<<20) | (1<<16) | (1<<12) | (1<<8) | (1<<4) | (0<<0))
.equ CLK_DIV2_VAL,			((0<<20) | (0<<16) | (1<<12) | (1<<8) | (1<<4) | (1<<0))
.equ CLK_DIV3_VAL,			((0<<28) | (0<<24) | (0<<20) | (0<<16) | (0<<12) | (1<<8) | (1<<4) | (2<<0))
.equ CLK_DIV4_VAL,			((0<<20) | (0<<16) | (0<<12) | (0<<8) | (0<<4) | (0<<0))

.equ CLK_SRC0_VAL,			((1<<24) | (1<<20) | (0<<16) | (1<<12) | (1<<8) | (1<<4) | (1<<0))
