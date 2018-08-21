/*
 * sys-dram.c
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

#include <xboot.h>
#include <h2/reg-ccu.h>
#include <h2/reg-dram.h>

#define CONFIG_DRAM_BASE	(0x40000000)
#define CONFIG_DRAM_CLK		(624)
#define CONFIG_DRAM_ZQ 		(3881979)

#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })

#define clrbits_le32(addr, clear) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) & ~(clear))

#define setbits_le32(addr, set) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) | (set))

#define clrsetbits_le32(addr, clear, set) \
	write32(((virtual_addr_t)(addr)), (read32(((virtual_addr_t)(addr))) & ~(clear)) | (set))

#define clamp(val, lo, hi)	min((typeof(val))max(val, lo), hi)
#define DIV_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))
#define REPEAT_BYTE(x)		((~0ul / 0xff) * (x))

struct h3_dram_com_reg_t {
	u32_t cr;			/* 0x00 control register */
	u32_t cr_r1;		/* 0x04 rank 1 control register (R40 only) */
	u8_t res0[0x4];		/* 0x08 */
	u32_t tmr;			/* 0x0c (unused on H3) */
	u32_t mcr[16][2];	/* 0x10 */
	u32_t bwcr;			/* 0x90 bandwidth control register */
	u32_t maer;			/* 0x94 master enable register */
	u32_t mapr;			/* 0x98 master priority register */
	u32_t mcgcr;		/* 0x9c */
	u32_t cpu_bwcr;		/* 0xa0 */
	u32_t gpu_bwcr;		/* 0xa4 */
	u32_t ve_bwcr;		/* 0xa8 */
	u32_t disp_bwcr;	/* 0xac */
	u32_t other_bwcr;	/* 0xb0 */
	u32_t total_bwcr;	/* 0xb4 */
	u8_t res1[0x8];		/* 0xb8 */
	u32_t swonr;		/* 0xc0 */
	u32_t swoffr;		/* 0xc4 */
	u8_t res2[0x8];		/* 0xc8 */
	u32_t cccr;			/* 0xd0 */
	u8_t res3[0x54];	/* 0xd4 */
	u32_t mdfs_bwlr[3];	/* 0x128 (unused on H3) */
	u8_t res4[0x6cc];	/* 0x134 */
	u32_t protect;		/* 0x800 */
};

struct h3_dram_ctl_reg_t {
	u32_t pir;			/* 0x00 PHY initialization register */
	u32_t pwrctl;		/* 0x04 */
	u32_t mrctrl;		/* 0x08 */
	u32_t clken;		/* 0x0c */
	u32_t pgsr[2];		/* 0x10 PHY general status registers */
	u32_t statr;		/* 0x18 */
	u8_t res1[0x10];	/* 0x1c */
	u32_t lp3mr11;		/* 0x2c */
	u32_t mr[4];		/* 0x30 mode registers */
	u32_t pllgcr;		/* 0x40 */
	u32_t ptr[5];		/* 0x44 PHY timing registers */
	u32_t dramtmg[9];	/* 0x58 DRAM timing registers */
	u32_t odtcfg;		/* 0x7c */
	u32_t pitmg[2];		/* 0x80 PHY interface timing registers */
	u8_t res2[0x4];		/* 0x88 */
	u32_t rfshctl0;		/* 0x8c */
	u32_t rfshtmg;		/* 0x90 refresh timing */
	u32_t rfshctl1;		/* 0x94 */
	u32_t pwrtmg;		/* 0x98 */
	u8_t res3[0x1c];	/* 0x9c */
	u32_t vtfcr;		/* 0xb8 (unused on H3) */
	u32_t dqsgmr;		/* 0xbc */
	u32_t dtcr;			/* 0xc0 */
	u32_t dtar[4];		/* 0xc4 */
	u32_t dtdr[2];		/* 0xd4 */
	u32_t dtmr[2];		/* 0xdc */
	u32_t dtbmr;		/* 0xe4 */
	u32_t catr[2];		/* 0xe8 */
	u32_t dtedr[2];		/* 0xf0 */
	u8_t res4[0x8];		/* 0xf8 */
	u32_t pgcr[4];		/* 0x100 PHY general configuration registers */
	u32_t iovcr[2];		/* 0x110 */
	u32_t dqsdr;		/* 0x118 */
	u32_t dxccr;		/* 0x11c */
	u32_t odtmap;		/* 0x120 */
	u32_t zqctl[2];		/* 0x124 */
	u8_t res6[0x14];	/* 0x12c */
	u32_t zqcr;			/* 0x140 ZQ control register */
	u32_t zqsr;			/* 0x144 ZQ status register */
	u32_t zqdr[3];		/* 0x148 ZQ data registers */
	u8_t res7[0x6c];	/* 0x154 */
	u32_t sched;		/* 0x1c0 */
	u32_t perfhpr[2];	/* 0x1c4 */
	u32_t perflpr[2];	/* 0x1cc */
	u32_t perfwr[2];	/* 0x1d4 */
	u8_t res8[0x24];	/* 0x1dc */
	u32_t acmdlr;		/* 0x200 AC master delay line register */
	u32_t aclcdlr;		/* 0x204 AC local calibrated delay line register */
	u32_t aciocr;		/* 0x208 AC I/O configuration register */
	u8_t res9[0x4];		/* 0x20c */
	u32_t acbdlr[31];	/* 0x210 AC bit delay line registers */
	u8_t res10[0x74];	/* 0x28c */
	struct {			/* 0x300 DATX8 modules*/
		u32_t mdlr;		/* 0x00 master delay line register */
		u32_t lcdlr[3];	/* 0x04 local calibrated delay line registers */
		u32_t bdlr[11];	/* 0x10 bit delay line registers */
		u32_t sdlr;		/* 0x3c output enable bit delay registers */
		u32_t gtr;		/* 0x40 general timing register */
		u32_t gcr;		/* 0x44 general configuration register */
		u32_t gsr[3];	/* 0x48 general status registers */
		u8_t res0[0x2c];/* 0x54 */
	} dx[4];
	u8_t res11[0x388];	/* 0x500 */
	u32_t upd2;			/* 0x888 */
};

struct dram_para_t {
	u16_t page_size;
	u8_t bus_full_width;
	u8_t dual_rank;
	u8_t row_bits;
	u8_t bank_bits;
	u8_t dx_read_delays[4][11];
	u8_t dx_write_delays[4][11];
	u8_t ac_delays[31];
	u8_t res[3];
};

static inline void sdelay(int loops)
{
	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0"(loops));
}

static inline int gfls(int x)
{
	int r = 32;

	if(!x)
		return 0;
	if(!(x & 0xffff0000u))
	{
		x <<= 16;
		r -= 16;
	}
	if(!(x & 0xff000000u))
	{
		x <<= 8;
		r -= 8;
	}
	if(!(x & 0xf0000000u))
	{
		x <<= 4;
		r -= 4;
	}
	if(!(x & 0xc0000000u))
	{
		x <<= 2;
		r -= 2;
	}
	if(!(x & 0x80000000u))
	{
		x <<= 1;
		r -= 1;
	}
	return r;
}

static inline int ns_to_t(int ns)
{
	unsigned int freq = CONFIG_DRAM_CLK / 2;
	return DIV_ROUND_UP(freq * ns, 1000);
}

static u32_t bin_to_mgray(int val)
{
	u8_t table[32];

	table[0] = 0x00;
	table[1] = 0x01;
	table[2] = 0x02;
	table[3] = 0x03;
	table[4] = 0x06;
	table[5] = 0x07;
	table[6] = 0x04;
	table[7] = 0x05;
	table[8] = 0x0c;
	table[9] = 0x0d;
	table[10] = 0x0e;
	table[11] = 0x0f;
	table[12] = 0x0a;
	table[13] = 0x0b;
	table[14] = 0x08;
	table[15] = 0x09;
	table[16] = 0x18;
	table[17] = 0x19;
	table[18] = 0x1a;
	table[19] = 0x1b;
	table[20] = 0x1e;
	table[21] = 0x1f;
	table[22] = 0x1c;
	table[23] = 0x1d;
	table[24] = 0x14;
	table[25] = 0x15;
	table[26] = 0x16;
	table[27] = 0x17;
	table[28] = 0x12;
	table[29] = 0x13;
	table[30] = 0x10;
	table[31] = 0x11;
	return table[clamp(val, 0, 31)];
}

static int mgray_to_bin(u32_t val)
{
	u8_t table[32];

	table[0] = 0x00;
	table[1] = 0x01;
	table[2] = 0x02;
	table[3] = 0x03;
	table[4] = 0x06;
	table[5] = 0x07;
	table[6] = 0x04;
	table[7] = 0x05;
	table[8] = 0x0e;
	table[9] = 0x0f;
	table[10] = 0x0c;
	table[11] = 0x0d;
	table[12] = 0x08;
	table[13] = 0x09;
	table[14] = 0x0a;
	table[15] = 0x0b;
	table[16] = 0x1e;
	table[17] = 0x1f;
	table[18] = 0x1c;
	table[19] = 0x1d;
	table[20] = 0x18;
	table[21] = 0x19;
	table[22] = 0x1a;
	table[23] = 0x1b;
	table[24] = 0x10;
	table[25] = 0x11;
	table[26] = 0x12;
	table[27] = 0x13;
	table[28] = 0x16;
	table[29] = 0x17;
	table[30] = 0x14;
	table[31] = 0x15;
	return table[val & 0x1f];
}

static void clock_set_pll_ddr(u32_t clk)
{
	int n = 32;
	int k = 1;
	int m = 2;
	u32_t val;

	clrsetbits_le32(H3_CCU_BASE + CCU_PLL_DDR_TUN, (0x7 << 24) | (0x7f << 16), ((2 & 0x7) << 24) | ((16 & 0x7f) << 16));

	/* ddr pll rate = 24000000 * n * k / m */
	if(clk > 24000000 * k * n / m)
	{
		m = 1;
		if(clk > 24000000 * k * n / m)
		{
			k = 2;
		}
	}

	val = (0x1 << 31);
	val |= (0x0 << 24);
	val |= (0x1 << 20);
	val |= ((((clk / (24000000 * k / m)) - 1) & 0x1f) << 8);
	val |= (((k - 1) & 0x3) << 4);
	val |= (((m - 1) & 0x3) << 0);
	write32(H3_CCU_BASE + CCU_PLL_DDR_CTRL, val);
	sdelay(5500);
}

static void mctl_await_completion(u32_t * reg, u32_t mask, u32_t val)
{
	int timeout = 0;

	while((read32((virtual_addr_t)reg) & mask) != val)
	{
		if(timeout++ > 10000)
			break;
	}
}

static int mctl_mem_matches(u32_t offset)
{
	write32(CONFIG_DRAM_BASE, 0);
	write32(CONFIG_DRAM_BASE + offset, 0xaa55aa55);
	__asm__ __volatile__ ("dsb" : : : "memory");
	return (read32(CONFIG_DRAM_BASE) == read32(CONFIG_DRAM_BASE + offset)) ? 1 : 0;
}

static void mctl_phy_init(u32_t val)
{
	struct h3_dram_ctl_reg_t * ctl = (struct h3_dram_ctl_reg_t *)H3_DRAM_CTL0_BASE;

	write32((virtual_addr_t)&ctl->pir, val | PIR_INIT);
	mctl_await_completion(&ctl->pgsr[0], PGSR_INIT_DONE, 0x1);
}

static void mctl_set_bit_delays(struct dram_para_t * para)
{
	struct h3_dram_ctl_reg_t * ctl = (struct h3_dram_ctl_reg_t *)H3_DRAM_CTL0_BASE;
	int i, j;
	u32_t val;
 	u32_t read = 0x00007979, write = 0x6aaa0000;

	for(i = 0; i < 4; i++)
	{
		val = DATX_IOCR_WRITE_DELAY((write >> (i * 4)) & 0xf) |
		DATX_IOCR_READ_DELAY(((read >> (i * 4)) & 0xf) * 2);

		for(j = DATX_IOCR_DQ(0); j <= DATX_IOCR_DM; j++)
			write32((virtual_addr_t) &ctl->dx[i].bdlr[j], val);
	} 

	clrbits_le32(&ctl->pgcr[0], 1 << 26);

	for(i = 0; i < 4; i++)
	{
		val = DATX_IOCR_WRITE_DELAY((write >> (16 + i * 4)) & 0xf) |
		DATX_IOCR_READ_DELAY((read >> (16 + i * 4)) & 0xf);

		write32((virtual_addr_t) &ctl->dx[i].bdlr[DATX_IOCR_DQS], val);
		write32((virtual_addr_t) &ctl->dx[i].bdlr[DATX_IOCR_DQSN], val);
	}

	setbits_le32(&ctl->pgcr[0], 1 << 26);
	sdelay(1);
}

enum {
	MBUS_PORT_CPU 		= 0,
	MBUS_PORT_GPU 		= 1,
	MBUS_PORT_UNUSED	= 2,
	MBUS_PORT_DMA 		= 3,
	MBUS_PORT_VE 		= 4,
	MBUS_PORT_CSI 		= 5,
	MBUS_PORT_NAND 		= 6,
	MBUS_PORT_SS 		= 7,
	MBUS_PORT_TS 		= 8,
	MBUS_PORT_DI 		= 9,
	MBUS_PORT_DE 		= 10,
	MBUS_PORT_DE_CFD 	= 11,
	MBUS_PORT_UNKNOWN1	= 12,
	MBUS_PORT_UNKNOWN2	= 13,
	MBUS_PORT_UNKNOWN3	= 14,
};

enum {
	MBUS_QOS_LOWEST 	= 0,
	MBUS_QOS_LOW 		= 1,
	MBUS_QOS_HIGH 		= 2,
	MBUS_QOS_HIGHEST 	= 3,
};

static inline void mbus_configure_port(u8_t port,
				int bwlimit,
				int priority,
				u8_t qos,
				u8_t waittime,
				u8_t acs,
				u16_t bwl0,
				u16_t bwl1,
				u16_t bwl2)
{
	struct h3_dram_com_reg_t * com = (struct h3_dram_com_reg_t *)H3_DRAM_COM_BASE;

	u32_t cfg0 = ( (bwlimit ? (1 << 0) : 0)
			   | (priority ? (1 << 1) : 0)
			   | ((qos & 0x3) << 2)
			   | ((waittime & 0xf) << 4)
			   | ((acs & 0xff) << 8)
			   | (bwl0 << 16) );
	u32_t cfg1 = ((u32_t)bwl2 << 16) | (bwl1 & 0xffff);

	write32((virtual_addr_t)&com->mcr[port][0], cfg0);
	write32((virtual_addr_t)&com->mcr[port][1], cfg1);
}

#define MBUS_CONF(port, bwlimit, qos, acs, bwl0, bwl1, bwl2)	\
	mbus_configure_port(MBUS_PORT_ ## port, bwlimit, 0, 		\
			    MBUS_QOS_ ## qos, 0, acs, bwl0, bwl1, bwl2)

static void mctl_set_master_priority(void)
{
	struct h3_dram_com_reg_t * com = (struct h3_dram_com_reg_t *)H3_DRAM_COM_BASE;

	write32((virtual_addr_t)&com->bwcr, (1 << 16) | (400 << 0));
	write32((virtual_addr_t)&com->mapr, 0x00000001);

	write32((virtual_addr_t)&com->mcr[0][0] , 0x0200000d);
	write32((virtual_addr_t)&com->mcr[0][1] , 0x00800100);
	write32((virtual_addr_t)&com->mcr[1][0] , 0x06000009);
	write32((virtual_addr_t)&com->mcr[1][1] , 0x01000400);
	write32((virtual_addr_t)&com->mcr[2][0] , 0x0200000d);
	write32((virtual_addr_t)&com->mcr[2][1] , 0x00600100);
	write32((virtual_addr_t)&com->mcr[3][0] , 0x0100000d);
	write32((virtual_addr_t)&com->mcr[3][1] , 0x00200080);
	write32((virtual_addr_t)&com->mcr[4][0] , 0x07000009);
	write32((virtual_addr_t)&com->mcr[4][1] , 0x01000640);
	write32((virtual_addr_t)&com->mcr[5][0] , 0x0100000d);
	write32((virtual_addr_t)&com->mcr[5][1] , 0x00200080);
	write32((virtual_addr_t)&com->mcr[6][0] , 0x01000009);
	write32((virtual_addr_t)&com->mcr[6][1] , 0x00400080);
	write32((virtual_addr_t)&com->mcr[7][0] , 0x0100000d);
	write32((virtual_addr_t)&com->mcr[7][1] , 0x00400080);
	write32((virtual_addr_t)&com->mcr[8][0] , 0x0100000d);
	write32((virtual_addr_t)&com->mcr[8][1] , 0x00400080);
	write32((virtual_addr_t)&com->mcr[9][0] , 0x04000009);
	write32((virtual_addr_t)&com->mcr[9][1] , 0x00400100);
	write32((virtual_addr_t)&com->mcr[10][0], 0x2000030d);
	write32((virtual_addr_t)&com->mcr[10][1], 0x04001800);
	write32((virtual_addr_t)&com->mcr[11][0], 0x04000009);
	write32((virtual_addr_t)&com->mcr[11][1], 0x00400120);
}

static void mctl_set_timing_params(struct dram_para_t * para)
{
	struct h3_dram_ctl_reg_t * ctl = (struct h3_dram_ctl_reg_t *)H3_DRAM_CTL0_BASE;
	u8_t tccd = 2;
	u8_t tfaw = ns_to_t(50);
	u8_t trrd	 = max(ns_to_t(10), 4);
	u8_t trcd = ns_to_t(15);
	u8_t trc = ns_to_t(53);
	u8_t txp = max(ns_to_t(8), 3);
	u8_t twtr = max(ns_to_t(8), 4);
	u8_t trtp = max(ns_to_t(8), 4);
	u8_t twr = max(ns_to_t(15), 3);
	u8_t trp = ns_to_t(15);
	u8_t tras = ns_to_t(38);
	u16_t trefi = ns_to_t(7800) / 32;
	u16_t trfc = ns_to_t(350);
	u8_t tmrw = 0;
	u8_t tmrd = 4;
	u8_t tmod = 12;
	u8_t tcke = 3;
	u8_t tcksrx = 5;
	u8_t tcksre = 5;
	u8_t tckesr = 4;
	u8_t trasmax = 24;
	u8_t tcl = 6;										/* CL 12 */
	u8_t tcwl = 4;										/* CWL 8 */
	u8_t t_rdata_en = 4;
	u8_t wr_latency = 2;
	u32_t tdinit0 = (500 * CONFIG_DRAM_CLK) + 1;		/* 500us */
	u32_t tdinit1 = (360 * CONFIG_DRAM_CLK) / 1000 + 1;	/* 360ns */
	u32_t tdinit2 = (200 * CONFIG_DRAM_CLK) + 1;		/* 200us */
	u32_t tdinit3 = (1 * CONFIG_DRAM_CLK) + 1;			/* 1us */
	u8_t twtp = tcwl + 2 + twr;							/* WL + BL / 2 + tWR */
	u8_t twr2rd = tcwl + 2 + twtr;						/* WL + BL / 2 + tWTR */
	u8_t trd2wr = tcl + 2 + 1 - tcwl;					/* RL + BL / 2 + 2 - WL */

	/* Set mode register */
	write32((virtual_addr_t)&ctl->mr[0], 0x1c70);		/* CL=11, WR=12 */
	write32((virtual_addr_t)&ctl->mr[1], 0x40);
	write32((virtual_addr_t)&ctl->mr[2], 0x18);			/* CWL=8 */
	write32((virtual_addr_t)&ctl->mr[3], 0x0);

	/* Set DRAM timing */
	write32((virtual_addr_t)&ctl->dramtmg[0], DRAMTMG0_TWTP(twtp) | DRAMTMG0_TFAW(tfaw) | DRAMTMG0_TRAS_MAX(trasmax) | DRAMTMG0_TRAS(tras));
	write32((virtual_addr_t)&ctl->dramtmg[1], DRAMTMG1_TXP(txp) | DRAMTMG1_TRTP(trtp) | DRAMTMG1_TRC(trc));
	write32((virtual_addr_t)&ctl->dramtmg[2], DRAMTMG2_TCWL(tcwl) | DRAMTMG2_TCL(tcl) | DRAMTMG2_TRD2WR(trd2wr) | DRAMTMG2_TWR2RD(twr2rd));
	write32((virtual_addr_t)&ctl->dramtmg[3], DRAMTMG3_TMRW(tmrw) | DRAMTMG3_TMRD(tmrd) | DRAMTMG3_TMOD(tmod));
	write32((virtual_addr_t)&ctl->dramtmg[4], DRAMTMG4_TRCD(trcd) | DRAMTMG4_TCCD(tccd) | DRAMTMG4_TRRD(trrd) | DRAMTMG4_TRP(trp));
	write32((virtual_addr_t)&ctl->dramtmg[5], DRAMTMG5_TCKSRX(tcksrx) | DRAMTMG5_TCKSRE(tcksre) | DRAMTMG5_TCKESR(tckesr) | DRAMTMG5_TCKE(tcke));

	/* Set two rank timing */
	clrsetbits_le32(&ctl->dramtmg[8], (0xff << 8) | (0xff << 0), (0x66 << 8) | (0x10 << 0));

	/* Set PHY interface timing, write latency and read latency configure */
	write32((virtual_addr_t)&ctl->pitmg[0], (0x2 << 24) | (t_rdata_en << 16) | (0x1 << 8) | (wr_latency << 0));

	/* Set PHY timing, PTR0-2 use default */
	write32((virtual_addr_t)&ctl->ptr[3], PTR3_TDINIT0(tdinit0) | PTR3_TDINIT1(tdinit1));
	write32((virtual_addr_t)&ctl->ptr[4], PTR4_TDINIT2(tdinit2) | PTR4_TDINIT3(tdinit3));

	/* Set refresh timing */
	write32((virtual_addr_t)&ctl->rfshtmg, RFSHTMG_TREFI(trefi) | RFSHTMG_TRFC(trfc));
}

static void mctl_zq_calibration(struct dram_para_t * para)
{
	struct h3_dram_ctl_reg_t * ctl = (struct h3_dram_ctl_reg_t *)H3_DRAM_CTL0_BASE;
	u16_t zq_val[6];
	u32_t val;
	u8_t zq;
	int i;

	if((read32((virtual_addr_t)(0x01c00000 + 0x24)) & 0xff) == 0 &&
	    (read32((virtual_addr_t)(0x01c00000 + 0xf0)) & 0x1) == 0)
	{
		clrsetbits_le32(&ctl->zqcr, 0xffff, CONFIG_DRAM_ZQ & 0xffff);

		write32((virtual_addr_t)&ctl->pir, PIR_CLRSR);
		mctl_phy_init(PIR_ZCAL);

		val = read32((virtual_addr_t)&ctl->zqdr[0]);
		val &= (0x1f << 16) | (0x1f << 0);
		val |= val << 8;
		write32((virtual_addr_t)&ctl->zqdr[0], val);

		val = read32((virtual_addr_t)&ctl->zqdr[1]);
		val &= (0x1f << 16) | (0x1f << 0);
		val |= val << 8;
		write32((virtual_addr_t)&ctl->zqdr[1], val);
		write32((virtual_addr_t)&ctl->zqdr[2], val);
	}
	else
	{
		write32((virtual_addr_t)&ctl->zqdr[2], 0x0a0a0a0a);

		for(i = 0; i < 6; i++)
		{
			zq = (CONFIG_DRAM_ZQ >> (i * 4)) & 0xf;
			write32((virtual_addr_t)&ctl->zqcr, (zq << 20) | (zq << 16) | (zq << 12) | (zq << 8) | (zq << 4) | (zq << 0));

			write32((virtual_addr_t)&ctl->pir, PIR_CLRSR);
			mctl_phy_init(PIR_ZCAL);

			zq_val[i] = read32((virtual_addr_t)&ctl->zqdr[0]) & 0xff;
			write32((virtual_addr_t)&ctl->zqdr[2], REPEAT_BYTE(zq_val[i]));

			write32((virtual_addr_t)&ctl->pir, PIR_CLRSR);
			mctl_phy_init(PIR_ZCAL);

			val = read32((virtual_addr_t)&ctl->zqdr[0]) >> 24;
			zq_val[i] |= bin_to_mgray(mgray_to_bin(val) - 1) << 8;
		}

		write32((virtual_addr_t)&ctl->zqdr[0], (zq_val[1] << 16) | zq_val[0]);
		write32((virtual_addr_t)&ctl->zqdr[1], (zq_val[3] << 16) | zq_val[2]);
		write32((virtual_addr_t)&ctl->zqdr[2], (zq_val[5] << 16) | zq_val[4]);
	}
}

static void mctl_set_cr(struct dram_para_t * para)
{
	struct h3_dram_com_reg_t * com = (struct h3_dram_com_reg_t *)H3_DRAM_COM_BASE;

	write32((virtual_addr_t)&com->cr, MCTL_CR_BL8 | MCTL_CR_INTERLEAVED |
		MCTL_CR_DDR3 | MCTL_CR_2T |
		(para->bank_bits == 3 ? MCTL_CR_EIGHT_BANKS : MCTL_CR_FOUR_BANKS) |
		MCTL_CR_BUS_FULL_WIDTH(para->bus_full_width) |
		(para->dual_rank ? MCTL_CR_DUAL_RANK : MCTL_CR_SINGLE_RANK) |
		MCTL_CR_PAGE_SIZE(para->page_size) |
		MCTL_CR_ROW_BITS(para->row_bits));
}

static void mctl_sys_init(struct dram_para_t * para)
{
	struct h3_dram_ctl_reg_t * ctl = (struct h3_dram_ctl_reg_t *)H3_DRAM_CTL0_BASE;
	u32_t val;

	val = read32(H3_CCU_BASE + CCU_MBUS_CLK);
	val &= ~(0x1 << 31);
	write32(H3_CCU_BASE + CCU_MBUS_CLK, val);

	val = read32(H3_CCU_BASE + CCU_MBUS_RST);
	val &= ~(0x1 << 31);
	write32(H3_CCU_BASE + CCU_MBUS_RST, val);

	val = read32(H3_CCU_BASE + CCU_BUS_CLK_GATE0);
	val &= ~(0x1 << 14);
	write32(H3_CCU_BASE + CCU_BUS_CLK_GATE0, val);

	val = read32(H3_CCU_BASE + CCU_BUS_SOFT_RST0);
	val &= ~(0x1 << 14);
	write32(H3_CCU_BASE + CCU_BUS_SOFT_RST0, val);

	val = read32(H3_CCU_BASE + CCU_PLL_DDR_CTRL);
	val &= ~(0x1 << 31);
	write32(H3_CCU_BASE + CCU_PLL_DDR_CTRL, val);
	sdelay(10);

	val = read32(H3_CCU_BASE + CCU_DRAM_CFG);
	val &= ~(0x1 << 31);
	write32(H3_CCU_BASE + CCU_DRAM_CFG, val);
	sdelay(1000);

	clock_set_pll_ddr(CONFIG_DRAM_CLK * 2 * 1000000);

	val = read32(H3_CCU_BASE + CCU_DRAM_CFG);
	val &= ~(0xf << 0);
	val &= ~(0x3 << 20);
	val |= ((1 - 1) << 0);
	val |= (0x0 << 20);
	val |= (0x1 << 16);
	write32(H3_CCU_BASE + CCU_DRAM_CFG, val);
	mctl_await_completion((u32_t *)(H3_CCU_BASE + CCU_DRAM_CFG), 0x1 << 16, 0);

	val = read32(H3_CCU_BASE + CCU_BUS_SOFT_RST0);
	val |= (0x1 << 14);
	write32(H3_CCU_BASE + CCU_BUS_SOFT_RST0, val);

	val = read32(H3_CCU_BASE + CCU_BUS_CLK_GATE0);
	val |= (0x1 << 14);
	write32(H3_CCU_BASE + CCU_BUS_CLK_GATE0, val);

	val = read32(H3_CCU_BASE + CCU_MBUS_RST);
	val |= (0x1 << 31);
	write32(H3_CCU_BASE + CCU_MBUS_RST, val);

	val = read32(H3_CCU_BASE + CCU_MBUS_CLK);
	val |= (0x1 << 31);
	write32(H3_CCU_BASE + CCU_MBUS_CLK, val);

	val = read32(H3_CCU_BASE + CCU_DRAM_CFG);
	val |= (0x1 << 31);
	write32(H3_CCU_BASE + CCU_DRAM_CFG, val);
	sdelay(10);

	write32((virtual_addr_t)&ctl->clken, 0xc00e);
	sdelay(500);
}

static int mctl_channel_init(struct dram_para_t * para)
{
	struct h3_dram_com_reg_t * com = (struct h3_dram_com_reg_t *)H3_DRAM_COM_BASE;
	struct h3_dram_ctl_reg_t * ctl = (struct h3_dram_ctl_reg_t *)H3_DRAM_CTL0_BASE;
	int i;

	mctl_set_cr(para);
	mctl_set_timing_params(para);
	mctl_set_master_priority();

	clrbits_le32(&ctl->pgcr[0], (1 << 30) | 0x3f);
	clrsetbits_le32(&ctl->pgcr[1], 1 << 24, 1 << 26);

	write32((virtual_addr_t)&com->protect, 0x94be6fa3);
	sdelay(100);
	clrsetbits_le32(&ctl->upd2, 0xfff << 16, 0x50 << 16);
	write32((virtual_addr_t)&com->protect, 0x0);
	sdelay(100);

	for(i = 0; i < 4; i++)
		clrsetbits_le32(&ctl->dx[i].gcr, (0x3 << 4) | (0x1 << 1) | (0x3 << 2) | (0x3 << 12) | (0x3 << 14), (0x0 << 4));

	clrsetbits_le32(&ctl->aciocr, 0, 0x1 << 1);
	setbits_le32(&ctl->pgcr[2], 0x3 << 6);
	clrbits_le32(&ctl->pgcr[0], (0x3 << 14) | (0x3 << 12));
	clrsetbits_le32(&ctl->pgcr[2], (0x3 << 10) | (0x3 << 8), (0x1 << 10) | (0x2 << 8));

	if(!para->bus_full_width)
	{
		write32((virtual_addr_t)&ctl->dx[2].gcr, 0x0);
		write32((virtual_addr_t)&ctl->dx[3].gcr, 0x0);
	}

	clrsetbits_le32(&ctl->dtcr, 0xf << 24, (para->dual_rank ? 0x3 : 0x1) << 24);
	mctl_set_bit_delays(para);
	sdelay(50);

	mctl_zq_calibration(para);
	mctl_phy_init(PIR_PLLINIT | PIR_DCAL | PIR_PHYRST | PIR_DRAMRST | PIR_DRAMINIT | PIR_QSGATE);

	if(read32((virtual_addr_t)&ctl->pgsr[0]) & (0xfe << 20))
	{
		if(((read32((virtual_addr_t)&ctl->dx[0].gsr[0]) >> 24) & 0x2) ||
			((read32((virtual_addr_t)&ctl->dx[1].gsr[0]) >> 24) & 0x2))
		{
			clrsetbits_le32(&ctl->dtcr, 0xf << 24, 0x1 << 24);
			para->dual_rank = 0;
		}

		if(((read32((virtual_addr_t)&ctl->dx[2].gsr[0]) >> 24) & 0x1) ||
			((read32((virtual_addr_t)&ctl->dx[3].gsr[0]) >> 24) & 0x1))
		{
			write32((virtual_addr_t)&ctl->dx[2].gcr, 0x0);
			write32((virtual_addr_t)&ctl->dx[3].gcr, 0x0);
			para->bus_full_width = 0;
		}

		mctl_set_cr(para);
		sdelay(20);

		mctl_phy_init(PIR_QSGATE);
		if(read32((virtual_addr_t)&ctl->pgsr[0]) & (0xfe << 20))
			return 1;
	}
	mctl_await_completion(&ctl->statr, 0x1, 0x1);

	setbits_le32(&ctl->rfshctl0, 0x1 << 31);
	sdelay(10);
	clrbits_le32(&ctl->rfshctl0, 0x1 << 31);
	sdelay(10);

	write32((virtual_addr_t)&ctl->pgcr[3], 0x00aa0060);
	setbits_le32(&ctl->zqcr, ZQCR_PWRDOWN);
	write32((virtual_addr_t)&com->maer, 0xffffffff);

	return 0;
}

static void mctl_auto_detect_dram_size(struct dram_para_t * para)
{
	para->page_size = 512;
	para->row_bits = 16;
	/*
	para->bank_bits = 2;
	mctl_set_cr(para);
	for(para->row_bits = 11; para->row_bits < 16; para->row_bits++)
	{
		if(mctl_mem_matches((1 << (para->row_bits + para->bank_bits)) * para->page_size))
			break;
	}
	 */

	para->bank_bits = 3;
	mctl_set_cr(para);
	for(para->bank_bits = 2; para->bank_bits < 3; para->bank_bits++)
	{
		if (mctl_mem_matches((1 << para->bank_bits) * para->page_size))
			break;
	}

	para->page_size = 8192;
	mctl_set_cr(para);
	for(para->page_size = 512; para->page_size < 8192; para->page_size *= 2)
	{
		if(mctl_mem_matches(para->page_size))
			break;
	}
}

void sys_dram_init(void)
{
	struct h3_dram_com_reg_t * com = (struct h3_dram_com_reg_t *)H3_DRAM_COM_BASE;
	struct h3_dram_ctl_reg_t * ctl = (struct h3_dram_ctl_reg_t *)H3_DRAM_CTL0_BASE;
	struct dram_para_t para = {
		.dual_rank = 0,
		.bus_full_width = 1,
		.row_bits = 15,
		.bank_bits = 3,
		.page_size = 4096,
		.dx_read_delays = {
			{ 18, 18, 18, 18, 18, 18, 18, 18, 18, 0, 0 },
			{ 14, 14, 14, 14, 14, 14, 14, 14, 14, 0, 0 },
			{ 18, 18, 18, 18, 18, 18, 18, 18, 18, 0, 0 },
			{ 14, 14, 14, 14, 14, 14, 14, 14, 14, 0, 0 }
		},
		.dx_write_delays = {
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0,  6,  6 }
		},
		.ac_delays = {
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0
		},
	};
	u32_t * dsz = (void *)0x00000020;

	if(dsz[0] != 0)
		return;

	mctl_sys_init(&para);
	if(mctl_channel_init(&para))
	{
		dsz[0] = 0;
		return;
	}

	if(para.dual_rank)
		write32((virtual_addr_t)&ctl->odtmap, 0x00000303);
	else
		write32((virtual_addr_t)&ctl->odtmap, 0x00000201);
	sdelay(1);
	write32((virtual_addr_t)&ctl->odtcfg, 0x0c000400);

	setbits_le32(&com->cccr, 1 << 31);
	sdelay(10);
	mctl_auto_detect_dram_size(&para);
	mctl_set_cr(&para);

	dsz[0] = (1UL << (para.row_bits + para.bank_bits)) * para.page_size * (para.dual_rank ? 2 : 1);
}
