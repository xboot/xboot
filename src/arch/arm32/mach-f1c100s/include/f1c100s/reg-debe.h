#ifndef __F1C100S_REG_DEBE_H__
#define __F1C100S_REG_DEBE_H__

#include <types.h>

#define F1C100S_DEBE_BASE			(0x01e60000)

struct f1c100s_debe_reg_t {
	u8_t res0[0x800];				/* 0x000 */
	u32_t mode;						/* 0x800 */
	u32_t backcolor;				/* 0x804 */
	u32_t disp_size;				/* 0x808 */
	u8_t res1[0x4];					/* 0x80c */
	u32_t layer0_size;				/* 0x810 */
	u32_t layer1_size;				/* 0x814 */
	u32_t layer2_size;				/* 0x818 */
	u32_t layer3_size;				/* 0x81c */
	u32_t layer0_pos;				/* 0x820 */
	u32_t layer1_pos;				/* 0x824 */
	u32_t layer2_pos;				/* 0x828 */
	u32_t layer3_pos;				/* 0x82c */
	u8_t res2[0x10];				/* 0x830 */
	u32_t layer0_stride;			/* 0x840 */
	u32_t layer1_stride;			/* 0x844 */
	u32_t layer2_stride;			/* 0x848 */
	u32_t layer3_stride;			/* 0x84c */
	u32_t layer0_addr_low32b;		/* 0x850 */
	u32_t layer1_addr_low32b;		/* 0x854 */
	u32_t layer2_addr_low32b;		/* 0x858 */
	u32_t layer3_addr_low32b;		/* 0x85c */
	u32_t layer0_addr_high4b;		/* 0x860 */
	u32_t layer1_addr_high4b;		/* 0x864 */
	u32_t layer2_addr_high4b;		/* 0x868 */
	u32_t layer3_addr_high4b;		/* 0x86c */
	u32_t reg_ctrl;					/* 0x870 */
	u8_t res3[0xc];					/* 0x874 */
	u32_t color_key_max;			/* 0x880 */
	u32_t color_key_min;			/* 0x884 */
	u32_t color_key_config;			/* 0x888 */
	u8_t res4[0x4];					/* 0x88c */
	u32_t layer0_attr0_ctrl;		/* 0x890 */
	u32_t layer1_attr0_ctrl;		/* 0x894 */
	u32_t layer2_attr0_ctrl;		/* 0x898 */
	u32_t layer3_attr0_ctrl;		/* 0x89c */
	u32_t layer0_attr1_ctrl;		/* 0x8a0 */
	u32_t layer1_attr1_ctrl;		/* 0x8a4 */
	u32_t layer2_attr1_ctrl;		/* 0x8a8 */
	u32_t layer3_attr1_ctrl;		/* 0x8ac */
	u8_t res5[0x110];				/* 0x8b0 */
	u32_t output_color_ctrl;		/* 0x9c0 */
	u8_t res6[0xc];					/* 0x9c4 */
	u32_t output_color_coef[12];	/* 0x9d0 */
};

#endif /* __F1C100S_REG_DEBE_H__ */
