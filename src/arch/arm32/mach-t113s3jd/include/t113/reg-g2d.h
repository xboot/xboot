#ifndef __T113_REG_G2D_H__
#define __T113_REG_G2D_H__

#include <types.h>

#define T113_G2D_BASE		(0x05410000)

#define T113_G2D_TOP		(0x00000000)
#define T113_G2D_MIXER		(0x00000100)
#define T113_G2D_BLD		(0x00000400)
#define T113_G2D_V0			(0x00000800)
#define T113_G2D_UI0		(0x00001000)
#define T113_G2D_UI1		(0x00001800)
#define T113_G2D_UI2		(0x00002000)
#define T113_G2D_WB			(0x00003000)
#define T113_G2D_VSU		(0x00008000)
#define T113_G2D_ROT		(0x00028000)
#define T113_G2D_GSU		(0x00030000)

struct g2d_top_t {
	u32_t G2D_SCLK_GATE;
	u32_t G2D_HCLK_GATE;
	u32_t G2D_AHB_RESET;
	u32_t G2D_SCLK_DIV;
	u32_t G2D_VERSION;
};

struct g2d_mixer_t {
	u32_t G2D_MIXER_CTL;
	u32_t G2D_MIXER_INT;
	u32_t G2D_MIXER_CLK;
};

struct g2d_v0_t {
	u32_t V0_ATTR;
	u32_t V0_MBSIZE;
	u32_t V0_COOR;
	u32_t V0_PITCH0;
	u32_t V0_PITCH1;
	u32_t V0_PITCH2;
	u32_t V0_LADD0;
	u32_t V0_LADD1;
	u32_t V0_LADD2;
	u32_t V0_FILLC;
	u32_t V0_HADD;
	u32_t V0_SIZE;
	u32_t V0_HDS_CTL0;
	u32_t V0_HDS_CTL1;
	u32_t V0_VDS_CTL0;
	u32_t V0_VDS_CTL1;
};

struct g2d_ui_t {
	u32_t UI_ATTR;
	u32_t UI_MBSIZE;
	u32_t UI_COOR;
	u32_t UI_PITCH;
	u32_t UI_LADD;
	u32_t UI_FILLC;
	u32_t UI_HADD;
	u32_t UI_SIZE;
};

struct g2d_vsu_t {
	u32_t VS_CTRL;
	u32_t reserved_0x004[15];
	u32_t VS_OUT_SIZE;
	u32_t VS_GLB_ALPHA;
	u32_t reserved_0x048[14];
	u32_t VS_Y_SIZE;
	u32_t reserved_0x084;
	u32_t VS_Y_HSTEP;
	u32_t VS_Y_VSTEP;
	u32_t VS_Y_HPHASE;
	u32_t reserved_0x094;
	u32_t VS_Y_VPHASE0;
	u32_t reserved_0x09C[9];
	u32_t VS_C_SIZE;
	u32_t reserved_0x0C4;
	u32_t VS_C_HSTEP;
	u32_t VS_C_VSTEP;
	u32_t VS_C_HPHASE;
	u32_t reserved_0x0D4;
	u32_t VS_C_VPHASE0;
	u32_t reserved_0x0DC[73];
	u32_t VS_Y_HCOEF[32];
	u32_t reserved_0x220[32];
	u32_t VS_Y_VCOEF[32];
	u32_t reserved_0x320[32];
	u32_t VS_C_HCOEF[32];
};

struct g2d_bld_t {
	u32_t BLD_EN_CTL;
	u32_t reserved_0x004[3];
	u32_t BLD_FILL_COLOR[2];
	u32_t reserved_0x018[2];
	u32_t BLD_CH_ISIZE[2];
	u32_t reserved_0x028[2];
	u32_t BLD_CH_OFFSET[2];
	u32_t reserved_0x038[2];
	u32_t BLD_PREMUL_CTL;
	u32_t BLD_BG_COLOR;
	u32_t BLD_SIZE;
	u32_t BLD_CTL;
	u32_t BLD_KEY_CTL;
	u32_t BLD_KEY_CFG;
	u32_t BLD_KEY_MAX;
	u32_t BLD_KEY_MIN;
	u32_t BLD_OUT_COLOR;
	u32_t reserved_0x064[7];
	u32_t ROP_CTL;
	u32_t ROP_INDEX[2];
	u32_t reserved_0x08C[29];
	u32_t BLD_CSC_CTL;
	u32_t reserved_0x104[3];
	u32_t BLD_CSC0_COEF00;
	u32_t BLD_CSC0_COEF01;
	u32_t BLD_CSC0_COEF02;
	u32_t BLD_CSC0_CONST0;
	u32_t BLD_CSC0_COEF10;
	u32_t BLD_CSC0_COEF11;
	u32_t BLD_CSC0_COEF12;
	u32_t BLD_CSC0_CONST1;
	u32_t BLD_CSC0_COEF20;
	u32_t BLD_CSC0_COEF21;
	u32_t BLD_CSC0_COEF22;
	u32_t BLD_CSC0_CONST2;
	u32_t BLD_CSC1_COEF00;
	u32_t BLD_CSC1_COEF01;
	u32_t BLD_CSC1_COEF02;
	u32_t BLD_CSC1_CONST0;
	u32_t BLD_CSC1_COEF10;
	u32_t BLD_CSC1_COEF11;
	u32_t BLD_CSC1_COEF12;
	u32_t BLD_CSC1_CONST1;
	u32_t BLD_CSC1_COEF20;
	u32_t BLD_CSC1_COEF21;
	u32_t BLD_CSC1_COEF22;
	u32_t BLD_CSC1_CONST2;
	u32_t BLD_CSC2_COEF00;
	u32_t BLD_CSC2_COEF01;
	u32_t BLD_CSC2_COEF02;
	u32_t BLD_CSC2_CONST0;
	u32_t BLD_CSC2_COEF10;
	u32_t BLD_CSC2_COEF11;
	u32_t BLD_CSC2_COEF12;
	u32_t BLD_CSC2_CONST1;
	u32_t BLD_CSC2_COEF20;
	u32_t BLD_CSC2_COEF21;
	u32_t BLD_CSC2_COEF22;
	u32_t BLD_CSC2_CONST2;
};

struct g2d_wb_t {
	u32_t WB_ATTR;
	u32_t WB_SIZE;
	u32_t WB_PITCH0;
	u32_t WB_PITCH1;
	u32_t WB_PITCH2;
	u32_t WB_LADD0;
	u32_t WB_HADD0;
	u32_t WB_LADD1;
	u32_t WB_HADD1;
	u32_t WB_LADD2;
	u32_t WB_HADD2;
};

struct g2d_rot_t {
	u32_t ROT_CTL;
	u32_t ROT_INT;
	u32_t ROT_TIMEOUT;
	u32_t reserved_0x00C[5];
	u32_t ROT_IFMT;
	u32_t ROT_ISIZE;
	u32_t reserved_0x028[2];
	u32_t ROT_IPITCH0;
	u32_t ROT_IPITCH1;
	u32_t ROT_IPITCH2;
	u32_t reserved_0x03C;
	u32_t ROT_ILADD0;
	u32_t ROT_IHADD0;
	u32_t ROT_ILADD1;
	u32_t ROT_IHADD1;
	u32_t ROT_ILADD2;
	u32_t ROT_IHADD2;
	u32_t reserved_0x058[11];
	u32_t ROT_OSIZE;
	u32_t reserved_0x088[2];
	u32_t ROT_OPITCH0;
	u32_t ROT_OPITCH1;
	u32_t ROT_OPITCH2;
	u32_t reserved_0x09C;
	u32_t ROT_OLADD0;
	u32_t ROT_OHADD0;
	u32_t ROT_OLADD1;
	u32_t ROT_OHADD1;
	u32_t ROT_OLADD2;
	u32_t ROT_OHADD2;
};

#endif /* __T113_REG_G2D_H__ */
