/*
 * sys-dram.S
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

.equ DMC0_MEMCONFIG_0,	0x30F01313
.equ DMC0_MEMCONFIG_1,	0x40F01323
.equ DMC0_TIMINGA_REF,	0x00000618
.equ DMC0_TIMING_ROW,	0x28233287
.equ DMC0_TIMING_DATA,	0x23240304
.equ DMC0_TIMING_PWR,	0x09C80232

.equ DMC1_MEMCONFIG_0,	0x40F01313
.equ DMC1_MEMCONFIG_1,	0x00E01323
.equ DMC1_TIMINGA_REF,	0x00000618
.equ DMC1_TIMING_ROW,	0x28233289
.equ DMC1_TIMING_DATA,	0x23240304
.equ DMC1_TIMING_PWR,	0x08280232

    .global sys_dram_init
sys_dram_init:
	ldr	r0, =0xf1e00000
	ldr	r1, =0x0
	str	r1, [r0, #0x0]

	/*
	 * dmc0 drive strength (setting 2x)
	 */
	ldr	r0, =0xe0200000

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x3cc]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x3ec]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x40c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x42c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x44c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x46c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x48c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x4ac]

	ldr	r1, =0x00002aaa
	str	r1, [r0, #0x4cc]

	/*
	 * dmc1 drive strength (setting 2x)
	 */
	ldr	r0, =0xe0200000

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x4ec]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x50c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x52c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x54c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x56C]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x58c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x5ac]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x5cc]

	ldr	r1, =0x00002aaa
	str	r1, [r0, #0x5ec]

	/*
	 * dmc0 initialization at single type
	 */
	ldr	r0, =0xf0000000

	ldr	r1, =0x00101000			/* phycontrol0 dll parameter setting */
	str	r1, [r0, #0x18]

	ldr	r1, =0x00000086			/* phycontrol1 dll parameter setting */
	str	r1, [r0, #0x1c]

	ldr	r1, =0x00101002			/* phycontrol0 dll on */
	str	r1, [r0, #0x18]

	ldr	r1, =0x00101003			/* phycontrol0 dll start */
	str	r1, [r0, #0x18]

dmc0_lock:
	ldr	r1, [r0, #0x40]			/* load phystatus register value */
	and	r2, r1, #0x7
	cmp	r2, #0x7				/* loop until dll is locked */
	bne	dmc0_lock

	and	r1, #0x3fc0
	mov	r2, r1, LSL #18
	orr	r2, r2, #0x100000
	orr	r2 ,r2, #0x1000

	orr	r1, r2, #0x3			/* force value locking */
	str	r1, [r0, #0x18]

	ldr	r1, =0x0FFF2010			/* concontrol auto refresh off */
	str	r1, [r0, #0x00]

	ldr	r1, =0x00212400			/* memcontrol bl=4, 2 chip, ddr2 type, dynamic self refresh, force precharge, dynamic power down off */
	str	r1, [r0, #0x04]

	ldr	r1, =DMC0_MEMCONFIG_0	/* memconfig0 256MB config, 8 banks, mapping method[12:15] 0:linear, 1:linterleaved, 2:mixed */
	str	r1, [r0, #0x08]

	ldr	r1, =DMC0_MEMCONFIG_1	/* memconfig1 */
	str	r1, [r0, #0x0c]

	ldr	r1, =0xFF000000			/* prech config */
	str	r1, [r0, #0x14]

	ldr	r1, =DMC0_TIMINGA_REF	/* timing aref */
	str	r1, [r0, #0x30]

	ldr	r1, =DMC0_TIMING_ROW	/* timingRow */
	str	r1, [r0, #0x34]

	ldr	r1, =DMC0_TIMING_DATA	/* timing data CL=3 */
	str	r1, [r0, #0x38]

	ldr	r1, =DMC0_TIMING_PWR	/* timing power */
	str	r1, [r0, #0x3c]

	ldr	r1, =0x07000000			/* chip0 deselect */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01000000			/* chip0 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00020000			/* chip0 emrs2 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00030000			/* chip0 emrs3 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010400			/* chip0 emrs1 (mem dll on, dqs disable) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00000542			/* chip0 mrs (mem dll reset) cl=4, bl=4 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01000000			/* chip0 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05000000			/* chip0 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05000000			/* chip0 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00000442			/* chip0 mrs (mem dll unreset) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010780			/* chip0 emrs1 (ocd default) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010400			/* chip0 emrs1 (ocd exit) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x07100000			/* chip1 deselect */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01100000			/* chip1 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00120000			/* chip1 emrs2 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00130000			/* chip1 emrs3 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110400			/* chip1 emrs1 (mem dll on, dqs disable) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00100542			/* chip1 mrs (mem dll reset) cl=4, bl=4 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01100000			/* chip1 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05100000			/* chip1 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05100000			/* chip1 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00100442			/* chip1 mrs (mem dll unreset) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110780			/* chip1 emrs1 (ocd default) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110400			/* chip1 emrs1 (ocd exit) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x0FF02030			/* concontrol auto refresh on */
	str	r1, [r0, #0x00]

	ldr	r1, =0xFFFF00FF			/* pwrdn config */
	str	r1, [r0, #0x28]

	ldr	r1, =0x00202400			/* memcontrol bl=4, 2 chip, ddr2 type, dynamic self refresh, force precharge, dynamic power down off */
	str	r1, [r0, #0x04]

	/*
	 * dmc1 initialization at single type
	 */
	ldr	r0, =0xf1400000

	ldr	r1, =0x00101000			/* phycontrol0 dll parameter setting */
	str	r1, [r0, #0x18]

	ldr	r1, =0x00000086			/* phycontrol1 dll parameter setting */
	str	r1, [r0, #0x1c]

	ldr	r1, =0x00101002			/* phycontrol0 dll on */
	str	r1, [r0, #0x18]

	ldr	r1, =0x00101003			/* phycontrol0 dll start */
	str	r1, [r0, #0x18]

dmc1_lock:
	ldr	r1, [r0, #0x40]			/* load phystatus register value */
	and	r2, r1, #0x7
	cmp	r2, #0x7				/* loop until dll is locked */
	bne	dmc1_lock

	and	r1, #0x3fc0
	mov	r2, r1, LSL #18
	orr	r2, r2, #0x100000
	orr	r2 ,r2, #0x1000

	orr	r1, r2, #0x3			/* force value locking */
	str	r1, [r0, #0x18]

	ldr	r1, =0x0FFF2010			/* concontrol auto refresh off */
	str	r1, [r0, #0x00]

	ldr	r1, =0x00212400			/* memcontrol bl=4, 2 chip, ddr2 type, dynamic self refresh, force precharge, dynamic power down off */
	str	r1, [r0, #0x04]

	ldr	r1, =DMC1_MEMCONFIG_0	/* memconfig0 256MB config, 8 banks, mapping method[12:15] 0:linear, 1:linterleaved, 2:mixed */
	str	r1, [r0, #0x08]

	ldr	r1, =DMC1_MEMCONFIG_1	/* memconfig1 */
	str	r1, [r0, #0x0c]

	ldr	r1, =0xFF000000			/* prech config */
	str	r1, [r0, #0x14]

	ldr	r1, =DMC1_TIMINGA_REF	/* timing aref */
	str	r1, [r0, #0x30]

	ldr	r1, =DMC1_TIMING_ROW	/* timingRow */
	str	r1, [r0, #0x34]

	ldr	r1, =DMC1_TIMING_DATA	/* timing data CL=3 */
	str	r1, [r0, #0x38]

	ldr	r1, =DMC1_TIMING_PWR	/* timing power */
	str	r1, [r0, #0x3c]

	ldr	r1, =0x07000000			/* chip0 deselect */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01000000			/* chip0 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00020000			/* chip0 emrs2 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00030000			/* chip0 emrs3 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010400			/* chip0 emrs1 (mem dll on, dqs disable) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00000542			/* chip0 mrs (mem dll reset) cl=4, bl=4 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01000000			/* chip0 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05000000			/* chip0 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05000000			/* chip0 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00000442			/* chip0 mrs (mem dll unreset) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010780			/* chip0 emrs1 (ocd default) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010400			/* chip0 emrs1 (ocd exit) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x07100000			/* chip1 deselect */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01100000			/* chip1 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00120000			/* chip1 emrs2 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00130000			/* chip1 emrs3 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110400			/* chip1 emrs1 (mem dll on, dqs disable) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00100542			/* chip1 mrs (mem dll reset) cl=4, bl=4 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01100000			/* chip1 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05100000			/* chip1 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05100000			/* chip1 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00100442			/* chip1 mrs (mem dll unreset) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110780			/* chip1 emrs1 (ocd default) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110400			/* chip1 emrs1 (ocd exit) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x0FF02030			/* concontrol auto refresh on */
	str	r1, [r0, #0x00]

	ldr	r1, =0xFFFF00FF			/* pwrdn config */
	str	r1, [r0, #0x28]

	ldr	r1, =0x00202400			/* memcontrol bl=4, 2 chip, ddr2 type, dynamic self refresh, force precharge, dynamic power down off */
	str	r1, [r0, #0x04]

	mov	pc, lr
