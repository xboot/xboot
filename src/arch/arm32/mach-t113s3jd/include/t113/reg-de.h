#ifndef __T113_REG_DE_H__
#define __T113_REG_DE_H__

#include <types.h>

#define T113_DE_BASE		(0x05000000)

#define T113_DE_MUX_GLB		(0x00100000 + 0x00000)
#define T113_DE_MUX_BLD		(0x00100000 + 0x01000)
#define T113_DE_MUX_CHAN	(0x00100000 + 0x02000)
#define T113_DE_MUX_VSU		(0x00100000 + 0x20000)
#define T113_DE_MUX_GSU1	(0x00100000 + 0x30000)
#define T113_DE_MUX_GSU2	(0x00100000 + 0x40000)
#define T113_DE_MUX_GSU3	(0x00100000 + 0x50000)
#define T113_DE_MUX_FCE		(0x00100000 + 0xa0000)
#define T113_DE_MUX_BWS		(0x00100000 + 0xa2000)
#define T113_DE_MUX_LTI		(0x00100000 + 0xa4000)
#define T113_DE_MUX_PEAK	(0x00100000 + 0xa6000)
#define T113_DE_MUX_ASE		(0x00100000 + 0xa8000)
#define T113_DE_MUX_FCC		(0x00100000 + 0xaa000)
#define T113_DE_MUX_DCSC	(0x00100000 + 0xb0000)

struct de_clk_t {
	u32_t gate_cfg;
	u32_t bus_cfg;
	u32_t rst_cfg;
	u32_t div_cfg;
	u32_t sel_cfg;
};

struct de_glb_t {
	u32_t ctl;
	u32_t status;
	u32_t dbuff;
	u32_t size;
};

struct de_bld_t {
	u32_t fcolor_ctl;
	struct {
		u32_t fcolor;
		u32_t insize;
		u32_t offset;
		u32_t dum;
	} attr[4];
	u32_t dum0[15];
	u32_t route;
	u32_t premultiply;
	u32_t bkcolor;
	u32_t output_size;
	u32_t bld_mode[4];
	u32_t dum1[4];
	u32_t ck_ctl;
	u32_t ck_cfg;
	u32_t dum2[2];
	u32_t ck_max[4];
	u32_t dum3[4];
	u32_t ck_min[4];
	u32_t dum4[3];
	u32_t out_ctl;
};

struct de_vi_t {
	struct {
		u32_t attr;
		u32_t size;
		u32_t coord;
		u32_t pitch[3];
		u32_t top_laddr[3];
		u32_t bot_laddr[3];
	} cfg[4];
	u32_t fcolor[4];
	u32_t top_haddr[3];
	u32_t bot_haddr[3];
	u32_t ovl_size[2];
	u32_t hori[2];
	u32_t vert[2];
};

struct de_ui_t {
	struct {
		u32_t attr;
		u32_t size;
		u32_t coord;
		u32_t pitch;
		u32_t top_laddr;
		u32_t bot_laddr;
		u32_t fcolor;
		u32_t dum;
	} cfg[4];
	u32_t top_haddr;
	u32_t bot_haddr;
	u32_t ovl_size;
};

#endif /* __T113_REG_DE_H__ */
