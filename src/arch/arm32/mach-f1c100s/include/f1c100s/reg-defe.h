#ifndef __F1C100S_REG_DEFE_H__
#define __F1C100S_REG_DEFE_H__

#include <types.h>

#define F1C100S_DEFE_BASE			(0x01e00000)

struct f1c100s_defe_reg_t {
	u32_t enable;					/* 0x000 */
	u32_t frame_ctrl;				/* 0x004 */
	u32_t bypass;					/* 0x008 */
	u32_t algorithm_sel;			/* 0x00c */
	u32_t line_int_ctrl;			/* 0x010 */
	u8_t res0[0x0c];				/* 0x014 */
	u32_t ch0_addr;					/* 0x020 */
	u32_t ch1_addr;					/* 0x024 */
	u32_t ch2_addr;					/* 0x028 */
	u32_t field_sequence;			/* 0x02c */
	u32_t ch0_offset;				/* 0x030 */
	u32_t ch1_offset;				/* 0x034 */
	u32_t ch2_offset;				/* 0x038 */
	u8_t res1[0x04];				/* 0x03c */
	u32_t ch0_stride;				/* 0x040 */
	u32_t ch1_stride;				/* 0x044 */
	u32_t ch2_stride;				/* 0x048 */
	u32_t input_fmt;				/* 0x04c */
	u32_t ch3_addr;					/* 0x050 */
	u32_t ch4_addr;					/* 0x054 */
	u32_t ch5_addr;					/* 0x058 */
	u32_t output_fmt;				/* 0x05c */
	u32_t int_enable;				/* 0x060 */
	u32_t int_status;				/* 0x064 */
	u32_t status;					/* 0x068 */
	u8_t res2[0x04];				/* 0x06c */
	u32_t csc_coef00;				/* 0x070 */
	u32_t csc_coef01;				/* 0x074 */
	u32_t csc_coef02;				/* 0x078 */
	u32_t csc_coef03;				/* 0x07c */
	u32_t csc_coef10;				/* 0x080 */
	u32_t csc_coef11;				/* 0x084 */
	u32_t csc_coef12;				/* 0x088 */
	u32_t csc_coef13;				/* 0x08c */
	u32_t csc_coef20;				/* 0x090 */
	u32_t csc_coef21;				/* 0x094 */
	u32_t csc_coef22;				/* 0x098 */
	u32_t csc_coef23;				/* 0x09c */
	u32_t deinterlace_ctrl;			/* 0x0a0 */
	u32_t deinterlace_diag;			/* 0x0a4 */
	u32_t deinterlace_tempdiff;		/* 0x0a8 */
	u32_t deinterlace_sawtooth;		/* 0x0ac */
	u32_t deinterlace_spatcomp;		/* 0x0b0 */
	u32_t deinterlace_burstlen;		/* 0x0b4 */
	u32_t deinterlace_preluma;		/* 0x0b8 */
	u32_t deinterlace_tile_addr;	/* 0x0bc */
	u32_t deinterlace_tile_stride;	/* 0x0c0 */
	u8_t res3[0x0c];				/* 0x0c4 */
	u32_t wb_stride_enable;			/* 0x0d0 */
	u32_t ch3_stride;				/* 0x0d4 */
	u32_t ch4_stride;				/* 0x0d8 */
	u32_t ch5_stride;				/* 0x0dc */
	u32_t fe_3d_ctrl;				/* 0x0e0 */
	u32_t fe_3d_ch0_addr;			/* 0x0e4 */
	u32_t fe_3d_ch1_addr;			/* 0x0e8 */
	u32_t fe_3d_ch2_addr;			/* 0x0ec */
	u32_t fe_3d_ch0_offset;			/* 0x0f0 */
	u32_t fe_3d_ch1_offset;			/* 0x0f4 */
	u32_t fe_3d_ch2_offset;			/* 0x0f8 */
	u8_t res4[0x04];				/* 0x0fc */
	u32_t ch0_insize;				/* 0x100 */
	u32_t ch0_outsize;				/* 0x104 */
	u32_t ch0_horzfact;				/* 0x108 */
	u32_t ch0_vertfact;				/* 0x10c */
	u32_t ch0_horzphase;			/* 0x110 */
	u32_t ch0_vertphase0;			/* 0x114 */
	u32_t ch0_vertphase1;			/* 0x118 */
	u8_t res5[0x04];				/* 0x11c */
	u32_t ch0_horztapoffset0;		/* 0x120 */
	u32_t ch0_horztapoffset1;		/* 0x124 */
	u32_t ch0_verttapoffset;		/* 0x128 */
	u8_t res6[0xd4];				/* 0x12c */
	u32_t ch1_insize;				/* 0x200 */
	u32_t ch1_outsize;				/* 0x204 */
	u32_t ch1_horzfact;				/* 0x208 */
	u32_t ch1_vertfact;				/* 0x20c */
	u32_t ch1_horzphase;			/* 0x210 */
	u32_t ch1_vertphase0;			/* 0x214 */
	u32_t ch1_vertphase1;			/* 0x218 */
	u8_t res7[0x04];				/* 0x21c */
	u32_t ch1_horztapoffset0;		/* 0x220 */
	u32_t ch1_horztapoffset1;		/* 0x224 */
	u32_t ch1_verttapoffset;		/* 0x228 */
	u8_t res8[0x1d4];				/* 0x22c */
	u32_t ch0_horzcoef0[32];		/* 0x400 */
	u32_t ch0_horzcoef1[32];		/* 0x480 */
	u32_t ch0_vertcoef[32];			/* 0x500 */
	u8_t res9[0x80];				/* 0x580 */
	u32_t ch1_horzcoef0[32];		/* 0x600 */
	u32_t ch1_horzcoef1[32];		/* 0x680 */
	u32_t ch1_vertcoef[32];			/* 0x700 */
	u8_t res10[0x280];				/* 0x780 */
	u32_t vpp_enable;				/* 0xa00 */
	u32_t vpp_dcti;					/* 0xa04 */
	u32_t vpp_lp1;					/* 0xa08 */
	u32_t vpp_lp2;					/* 0xa0c */
	u32_t vpp_wle;					/* 0xa10 */
	u32_t vpp_ble;					/* 0xa14 */
};

#endif /* __F1C100S_REG_DEFE_H__ */
