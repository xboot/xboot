#ifndef __T113_REG_TCONLCD_H__
#define __T113_REG_TCONLCD_H__

#include <types.h>

#define T113_TCONLCD_BASE		(0x05461000)

struct t113_tconlcd_reg_t {
	u32_t gctrl;				/* 0x00 */
	u32_t gint0;				/* 0x04 */
	u32_t gint1;				/* 0x08 */
	u32_t res_0c;
	u32_t frm_ctrl;				/* 0x10 */
	u32_t frm_seed[6];			/* 0x14 */
	u32_t frm_table[4];			/* 0x2c */
	u32_t fifo_3d;				/* 0x3c */
	u32_t ctrl;					/* 0x40 */
	u32_t dclk;					/* 0x44 */
	u32_t timing0;				/* 0x48 */
	u32_t timing1;				/* 0x4c */
	u32_t timing2;				/* 0x50 */
	u32_t timing3;				/* 0x54 */
	u32_t hv_intf;				/* 0x58 */
	u32_t res_5c;
	u32_t cpu_intf;				/* 0x60 */
	u32_t cpu_wr;				/* 0x64 */
	u32_t cpu_rd0;				/* 0x68 */
	u32_t cpu_rd1;				/* 0x6c */
	u32_t res_70_80[5];			/* 0x70 */
	u32_t lvds_intf;			/* 0x84 */
	u32_t io_polarity;			/* 0x88 */
	u32_t io_tristate;			/* 0x8c */
	u32_t res_90_f8[27];
	u32_t debug;				/* 0xfc */
	u32_t ceu_ctl;				/* 0x100 */
	u32_t res_104_10c[3];
	u32_t ceu_coef[20];			/* 0x110 */
	u32_t cpu_tri0;				/* 0x160 */
	u32_t cpu_tri1;				/* 0x164 */
	u32_t cpu_tri2;				/* 0x168 */
	u32_t cpu_tri3;				/* 0x16c */
	u32_t cpu_tri4;				/* 0x170 */
	u32_t cpu_tri5;				/* 0x174 */
	u32_t res_178_17c[2];
	u32_t cmap_ctl;				/* 0x180 */
	u32_t res_184_18c[3];
	u32_t cmap_odd0;			/* 0x190 */
	u32_t cmap_odd1;			/* 0x194 */
	u32_t cmap_even0;			/* 0x198 */
	u32_t cmap_even1;			/* 0x19c */
	u32_t res_1a0_1ec[20];
	u32_t safe_period;			/* 0x1f0 */
	u32_t res_1f4_21c[11];
	u32_t lvds_ana0;			/* 0x220 */
	u32_t lvds_ana1;			/* 0x224 */
	u32_t res_228_22c[2];
	u32_t sync_ctl;				/* 0x230 */
	u32_t sync_pos;				/* 0x234 */
	u32_t slave_stop_pos;		/* 0x238 */
	u32_t res_23c_3fc[113];
	u32_t gamma_table[256];		/* 0x400 */
};

#endif /* __T113_REG_TCONLCD_H__ */
