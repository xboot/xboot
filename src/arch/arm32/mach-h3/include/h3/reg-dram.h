#ifndef __H3_REG_DRAM_H__
#define __H3_REG_DRAM_H__

#define H3_DRAM_COM_BASE			(0x01c62000)
#define H3_DRAM_CTL0_BASE			(0x01c63000)
#define H3_DRAM_CTL1_BASE			(0x01c64000)
#define H3_DRAM_PHY0_BASE			(0x01c65000)
#define H3_DRAM_PHY1_BASE			(0x01c66000)

#define MCTL_CR_BL8					(0x4 << 20)
#define MCTL_CR_1T					(0x1 << 19)
#define MCTL_CR_2T					(0x0 << 19)
#define MCTL_CR_LPDDR3				(0x7 << 16)
#define MCTL_CR_LPDDR2				(0x6 << 16)
#define MCTL_CR_DDR3				(0x3 << 16)
#define MCTL_CR_DDR2				(0x2 << 16)
#define MCTL_CR_SEQUENTIAL			(0x1 << 15)
#define MCTL_CR_INTERLEAVED			(0x0 << 15)
#define MCTL_CR_FULL_WIDTH			(0x1 << 12)
#define MCTL_CR_HALF_WIDTH			(0x0 << 12)
#define MCTL_CR_BUS_FULL_WIDTH(x)	((x) << 12)
#define MCTL_CR_PAGE_SIZE(x)		((gfls(x) - 4) << 8)
#define MCTL_CR_ROW_BITS(x)			(((x) - 1) << 4)
#define MCTL_CR_EIGHT_BANKS			(0x1 << 2)
#define MCTL_CR_FOUR_BANKS			(0x0 << 2)
#define MCTL_CR_DUAL_RANK			(0x1 << 0)
#define MCTL_CR_SINGLE_RANK			(0x0 << 0)

#define DRAMTMG0_TWTP(x)			((x) << 24)
#define DRAMTMG0_TFAW(x)			((x) << 16)
#define DRAMTMG0_TRAS_MAX(x)		((x) << 8)
#define DRAMTMG0_TRAS(x)			((x) << 0)

#define DRAMTMG1_TXP(x)				((x) << 16)
#define DRAMTMG1_TRTP(x)			((x) << 8)
#define DRAMTMG1_TRC(x)				((x) << 0)

#define DRAMTMG2_TCWL(x)			((x) << 24)
#define DRAMTMG2_TCL(x)				((x) << 16)
#define DRAMTMG2_TRD2WR(x)			((x) << 8)
#define DRAMTMG2_TWR2RD(x)			((x) << 0)

#define DRAMTMG3_TMRW(x)			((x) << 16)
#define DRAMTMG3_TMRD(x)			((x) << 12)
#define DRAMTMG3_TMOD(x)			((x) << 0)

#define DRAMTMG4_TRCD(x)			((x) << 24)
#define DRAMTMG4_TCCD(x)			((x) << 16)
#define DRAMTMG4_TRRD(x)			((x) << 8)
#define DRAMTMG4_TRP(x)				((x) << 0)

#define DRAMTMG5_TCKSRX(x)			((x) << 24)
#define DRAMTMG5_TCKSRE(x)			((x) << 16)
#define DRAMTMG5_TCKESR(x)			((x) << 8)
#define DRAMTMG5_TCKE(x)			((x) << 0)

#define PTR3_TDINIT1(x)				((x) << 20)
#define PTR3_TDINIT0(x)				((x) << 0)

#define PTR4_TDINIT3(x)				((x) << 20)
#define PTR4_TDINIT2(x)				((x) << 0)

#define RFSHTMG_TREFI(x)			((x) << 16)
#define RFSHTMG_TRFC(x)				((x) << 0)

#define PIR_CLRSR					(0x1 << 27)	/* Clear status registers */
#define PIR_QSGATE					(0x1 << 10)	/* Read DQS gate training */
#define PIR_DRAMINIT				(0x1 << 8)	/* DRAM initialization */
#define PIR_DRAMRST					(0x1 << 7)	/* DRAM reset */
#define PIR_PHYRST					(0x1 << 6)	/* PHY reset */
#define PIR_DCAL					(0x1 << 5)	/* DDL calibration */
#define PIR_PLLINIT					(0x1 << 4)	/* PLL initialization */
#define PIR_ZCAL					(0x1 << 1)	/* ZQ calibration */
#define PIR_INIT					(0x1 << 0)	/* PHY initialization trigger */
#define PGSR_INIT_DONE				(0x1 << 0)	/* PHY init done */
#define ZQCR_PWRDOWN				(0x1 << 31)	/* ZQ power down */

#define DXBDLR_DQ(x)				(x)			/* DQ0-7 BDLR index */
#define DXBDLR_DM					(8)			/* DM BDLR index */
#define DXBDLR_DQS					(9)			/* DQS BDLR index */
#define DXBDLR_DQSN					(10)		/* DQSN BDLR index */

#define DXBDLR_WRITE_DELAY(x)		((x) << 8)
#define DXBDLR_READ_DELAY(x)		((x) << 0)
#define ACBDLR_WRITE_DELAY(x)		((x) << 8)

#endif /* __H3_REG_DRAM_H__ */
