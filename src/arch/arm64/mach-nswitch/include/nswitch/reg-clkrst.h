#ifndef __NSWITCH_REG_CLKRST_H__
#define __NSWITCH_REG_CLKRST_H__

#define NSWITCH_CLKRST_BASE				(0x60006000)

struct clkrst_reg_t {
	uint32_t rst_src;					/* _RST_SOURCE,             0x000 */
	uint32_t rst_dev_l;					/* _RST_DEVICES_L,          0x004 */
	uint32_t rst_dev_h;					/* _RST_DEVICES_H,          0x008 */
	uint32_t rst_dev_u;					/* _RST_DEVICES_U,          0x00c */
	uint32_t clk_out_enb_l;				/* _CLK_OUT_ENB_L,          0x010 */
	uint32_t clk_out_enb_h;				/* _CLK_OUT_ENB_H,          0x014 */
	uint32_t clk_out_enb_u;				/* _CLK_OUT_ENB_U,          0x018 */
	uint32_t _rsv0;						/*                          0x01c */
	uint32_t cclk_brst_pol;				/* _CCLK_BURST_POLICY,      0x020 */
	uint32_t super_cclk_div;			/* _SUPER_CCLK_DIVIDER,     0x024 */
	uint32_t sclk_brst_pol;				/* _SCLK_BURST_POLICY,      0x028 */
	uint32_t super_sclk_div;			/* _SUPER_SCLK_DIVIDER,     0x02C */
	uint32_t clk_sys_rate;				/* _CLK_SYSTEM_RATE,        0x030 */
	uint32_t _rsv1[3];					/*                      0x034-03c */
	uint32_t cop_clk_skip_plcy;			/* _COP_CLK_SKIP_POLICY,    0x040 */
	uint32_t clk_mask_arm;				/* _CLK_MASK_ARM,           0x044 */
	uint32_t misc_clk_enb;				/* _MISC_CLK_ENB,           0x048 */
	uint32_t clk_cpu_cmplx;				/* _CLK_CPU_CMPLX,          0x04C */
	uint32_t osc_ctrl;					/* _OSC_CTRL,               0x050 */
	uint32_t pll_lfsr;					/* _PLL_LFSR,               0x054 */
	uint32_t osc_freq_det;				/* _OSC_FREQ_DET,           0x058 */
	uint32_t osc_freq_det_stat;			/* _OSC_FREQ_DET_STATUS,    0x05C */
	uint32_t _rsv2[8];					/*                      0x060-07C */
	uint32_t pllc_base;					/* _PLLC_BASE,              0x080 */
	uint32_t pllc_out;					/* _PLLC_OUT,               0x084 */
	uint32_t pllc_misc;					/* _PLLC_MISC,              0x088 */
	uint32_t pllc_misc_1;				/* _PLLC_MISC_1,            0x08c */
	uint32_t pllm_base;					/* _PLLM_BASE,              0x090 */
	uint32_t pllm_out;					/* _PLLM_OUT,               0x094 */
	uint32_t pllm_misc1;				/* _PLLM_MISC1,             0x098 */
	uint32_t pllm_misc2;				/* _PLLM_MISC2,             0x09c */
	uint32_t pllp_base;					/* _PLLP_BASE,              0x0a0 */
	uint32_t pllp_outa;					/* _PLLP_OUTA,              0x0a4 */
	uint32_t pllp_outb;					/* _PLLP_OUTB,              0x0a8 */
	uint32_t pllp_misc;					/* _PLLP_MISC,              0x0ac */
	uint32_t plla_base;					/* _PLLA_BASE,              0x0b0 */
	uint32_t plla_out;					/* _PLLA_OUT,               0x0b4 */
	uint32_t _rsv3;						/*                          0x0b8 */
	uint32_t plla_misc;					/* _PLLA_MISC,              0x0bc */
	uint32_t pllu_base;					/* _PLLU_BASE,              0x0c0 */
	uint32_t _rsv4[2];					/*                      0x0c4-0c8 */
	uint32_t pllu_misc;					/* _PLLU_MISC,              0x0cc */
	uint32_t plld_base;					/* _PLLD_BASE,              0x0d0 */
	uint32_t _rsv5[1];					/*                          0x0d4 */
	uint32_t plld_misc1;				/* _PLLD_MISC1,             0x0d8 */
	uint32_t plld_misc;					/* _PLLD_MISC,              0x0dc */
	uint32_t pllx_base;					/* _PLLX_BASE,              0x0e0 */
	uint32_t pllx_misc;					/* _PLLX_MISC,              0x0e4 */
	uint32_t plle_base;					/* _PLLE_BASE,              0x0e8 */
	uint32_t plle_misc;					/* _PLLE_MISC,              0x0ec */
	uint32_t plls_base;					/* _PLLS_BASE,              0x0f0 */
	uint32_t plls_misc;					/* _PLLS_MISC,              0x0f4 */
	uint32_t _rsv6[2];					/*                      0x0f8-0fc */
	uint32_t clk_src_i2s1;				/* _CLK_SOURCE_I2S1,        0x100 */
	uint32_t clk_src_i2s2;				/* _CLK_SOURCE_I2S2,        0x104 */
	uint32_t clk_src_spdif_out;			/* _CLK_SOURCE_SPDIF_OUT,   0x108 */
	uint32_t clk_src_spdif_in;			/* _CLK_SOURCE_SPDIF_IN,    0x10c */
	uint32_t clk_src_pwm;				/* _CLK_SOURCE_PWM,         0x110 */
	uint32_t _rsv7;						/*                          0x114 */
	uint32_t clk_src_sbc2;				/* _CLK_SOURCE_SBC2,        0x118 */
	uint32_t clk_src_sbc3;				/* _CLK_SOURCE_SBC3,        0x11c */
	uint32_t _rsv8;						/*                          0x120 */
	uint32_t clk_src_i2c1;				/* _CLK_SOURCE_I2C1,        0x124 */
	uint32_t clk_src_i2c5;				/* _CLK_SOURCE_I2C5,        0x128 */
	uint32_t _rsv9[2];					/*                      0x12c-130 */
	uint32_t clk_src_sbc1;				/* _CLK_SOURCE_SBC1,        0x134 */
	uint32_t clk_src_disp1;				/* _CLK_SOURCE_DISP1,       0x138 */
	uint32_t clk_src_disp2;				/* _CLK_SOURCE_DISP2,       0x13c */
	uint32_t _rsv10[2];					/*                      0x140-144 */
	uint32_t clk_src_vi;				/* _CLK_SOURCE_VI,          0x148 */
	uint32_t _rsv11;					/*                          0x14c */
	uint32_t clk_src_sdmmc1;			/* _CLK_SOURCE_SDMMC1,      0x150 */
	uint32_t clk_src_sdmmc2;			/* _CLK_SOURCE_SDMMC2,      0x154 */
	uint32_t clk_src_g3d;				/* _CLK_SOURCE_G3D,         0x158 */
	uint32_t clk_src_g2d;				/* _CLK_SOURCE_G2D,         0x15c */
	uint32_t clk_src_ndflash;			/* _CLK_SOURCE_NDFLASH,     0x160 */
	uint32_t clk_src_sdmmc4;			/* _CLK_SOURCE_SDMMC4,      0x164 */
	uint32_t clk_src_vfir;				/* _CLK_SOURCE_VFIR,        0x168 */
	uint32_t clk_src_epp;				/* _CLK_SOURCE_EPP,         0x16c */
	uint32_t clk_src_mpe;				/* _CLK_SOURCE_MPE,         0x170 */
	uint32_t clk_src_hsi;				/* _CLK_SOURCE_HSI,         0x174 */
	uint32_t clk_src_uarta;				/* _CLK_SOURCE_UARTA,       0x178 */
	uint32_t clk_src_uartb;				/* _CLK_SOURCE_UARTB,       0x17c */
	uint32_t clk_src_host1x;			/* _CLK_SOURCE_HOST1X,      0x180 */
	uint32_t _rsv12[2];					/*                      0x184-188 */
	uint32_t clk_src_hdmi;				/* _CLK_SOURCE_HDMI,        0x18c */
	uint32_t _rsv13[2];					/*                      0x190-194 */
	uint32_t clk_src_i2c2;				/* _CLK_SOURCE_I2C2,        0x198 */
	uint32_t clk_src_emc;				/* _CLK_SOURCE_EMC,         0x19c */
	uint32_t clk_src_uartc;				/* _CLK_SOURCE_UARTC,       0x1a0 */
	uint32_t _rsv14;					/*                          0x1a4 */
	uint32_t clk_src_vi_sensor;			/* _CLK_SOURCE_VI_SENSOR,   0x1a8 */
	uint32_t _rsv15[2];					/*                      0x1ac-1b0 */
	uint32_t clk_src_sbc4;				/* _CLK_SOURCE_SBC4,        0x1b4 */
	uint32_t clk_src_i2c3;				/* _CLK_SOURCE_I2C3,        0x1b8 */
	uint32_t clk_src_sdmmc3;			/* _CLK_SOURCE_SDMMC3,      0x1bc */
	uint32_t clk_src_uartd;				/* _CLK_SOURCE_UARTD,       0x1c0 */
	uint32_t clk_src_uarte;				/* _CLK_SOURCE_UARTE,       0x1c4 */
	uint32_t clk_src_vde;				/* _CLK_SOURCE_VDE,         0x1c8 */
	uint32_t clk_src_owr;				/* _CLK_SOURCE_OWR,         0x1cc */
	uint32_t clk_src_nor;				/* _CLK_SOURCE_NOR,         0x1d0 */
	uint32_t clk_src_csite;				/* _CLK_SOURCE_CSITE,       0x1d4 */
	uint32_t clk_src_i2s0;				/* _CLK_SOURCE_I2S0,        0x1d8 */
	uint32_t clk_src_dtv;				/* _CLK_SOURCE_DTV,         0x1dc */
	uint32_t _rsv16[4];					/*                      0x1e0-1ec */
	uint32_t clk_src_msenc;				/* _CLK_SOURCE_MSENC,       0x1f0 */
	uint32_t clk_src_tsec;				/* _CLK_SOURCE_TSEC,        0x1f4 */
	uint32_t _rsv17;					/*                          0x1f8 */
	uint32_t clk_src_osc;				/* _CLK_SOURCE_OSC,         0x1fc */
	uint32_t _rsv18[32];				/*                      0x200-27c */
	uint32_t clk_out_enb_x;				/* _CLK_OUT_ENB_X_0,        0x280 */
	uint32_t clk_enb_x_set;				/* _CLK_ENB_X_SET_0,        0x284 */
	uint32_t clk_enb_x_clr;				/* _CLK_ENB_X_CLR_0,        0x288 */
	uint32_t rst_dev_x;					/* _RST_DEVICES_X_0,        0x28c */
	uint32_t rst_dev_x_set;				/* _RST_DEV_X_SET_0,        0x290 */
	uint32_t rst_dev_x_clr;				/* _RST_DEV_X_CLR_0,        0x294 */
	uint32_t clk_out_enb_y;				/* _CLK_OUT_ENB_Y_0,        0x298 */
	uint32_t clk_enb_y_set;				/* _CLK_ENB_Y_SET_0,        0x29C */
	uint32_t clk_enb_y_clr;				/* _CLK_ENB_Y_CLR_0,        0x2A0 */
	uint32_t rst_dev_y;					/* _RST_DEVICES_Y_0,        0x2A4 */
	uint32_t rst_dev_y_set;				/* _RST_DEV_Y_SET_0,        0x2A8 */
	uint32_t rst_dev_y_clr;				/* _RST_DEV_Y_CLR_0,        0x2AC */
	uint32_t _rsv19[17];				/*                      0x2B0-2f0 */
	uint32_t dfll_base;					/* _DFLL_BASE_0,            0x2f4 */
	uint32_t _rsv20[2];					/*                      0x2f8-2fc */
	uint32_t rst_dev_l_set;				/* _RST_DEV_L_SET           0x300 */
	uint32_t rst_dev_l_clr;				/* _RST_DEV_L_CLR           0x304 */
	uint32_t rst_dev_h_set;				/* _RST_DEV_H_SET           0x308 */
	uint32_t rst_dev_h_clr;				/* _RST_DEV_H_CLR           0x30c */
	uint32_t rst_dev_u_set;				/* _RST_DEV_U_SET           0x310 */
	uint32_t rst_dev_u_clr;				/* _RST_DEV_U_CLR           0x314 */
	uint32_t _rsv21[2];					/*                      0x318-31c */
	uint32_t clk_enb_l_set;				/* _CLK_ENB_L_SET           0x320 */
	uint32_t clk_enb_l_clr;				/* _CLK_ENB_L_CLR           0x324 */
	uint32_t clk_enb_h_set;				/* _CLK_ENB_H_SET           0x328 */
	uint32_t clk_enb_h_clr;				/* _CLK_ENB_H_CLR           0x32c */
	uint32_t clk_enb_u_set;				/* _CLK_ENB_U_SET           0x330 */
	uint32_t clk_enb_u_clr;				/* _CLK_ENB_U_CLR           0x334 */
	uint32_t _rsv22;					/*                          0x338 */
	uint32_t ccplex_pg_sm_ovrd;			/* _CCPLEX_PG_SM_OVRD,      0x33c */
	uint32_t rst_cpu_cmplx_set;			/* _RST_CPU_CMPLX_SET,      0x340 */
	uint32_t rst_cpu_cmplx_clr;			/* _RST_CPU_CMPLX_CLR,      0x344 */
	uint32_t clk_cpu_cmplx_set;			/* _CLK_CPU_CMPLX_SET,      0x348 */
	uint32_t clk_cpu_cmplx_clr;			/* _CLK_CPU_CMPLX_SET,      0x34c */
	uint32_t _rsv23[2];					/*                      0x350-354 */
	uint32_t rst_dev_v;					/* _RST_DEVICES_V,          0x358 */
	uint32_t rst_dev_w;					/* _RST_DEVICES_W,          0x35c */
	uint32_t clk_out_enb_v;				/* _CLK_OUT_ENB_V,          0x360 */
	uint32_t clk_out_enb_w;				/* _CLK_OUT_ENB_W,          0x364 */
	uint32_t cclkg_brst_pol;			/* _CCLKG_BURST_POLICY,     0x368 */
	uint32_t super_cclkg_div;			/* _SUPER_CCLKG_DIVIDER,    0x36c */
	uint32_t cclklp_brst_pol;			/* _CCLKLP_BURST_POLICY,    0x370 */
	uint32_t super_cclkp_div;			/* _SUPER_CCLKLP_DIVIDER,   0x374 */
	uint32_t clk_cpug_cmplx;			/* _CLK_CPUG_CMPLX,         0x378 */
	uint32_t clk_cpulp_cmplx;			/* _CLK_CPULP_CMPLX,        0x37c */
	uint32_t cpu_softrst_ctrl;			/* _CPU_SOFTRST_CTRL,       0x380 */
	uint32_t cpu_softrst_ctrl1;			/* _CPU_SOFTRST_CTRL1,      0x384 */
	uint32_t cpu_softrst_ctrl2;			/* _CPU_SOFTRST_CTRL2,      0x388 */
	uint32_t _rsv24[9];					/*                      0x38c-3ac */
	uint32_t clk_src_g3d2;				/* _CLK_SOURCE_G3D2,        0x3b0 */
	uint32_t clk_src_mselect;			/* _CLK_SOURCE_MSELECT,     0x3b4 */
	uint32_t clk_src_tsensor;			/* _CLK_SOURCE_TSENSOR,     0x3b8 */
	uint32_t clk_src_i2s3;				/* _CLK_SOURCE_I2S3,        0x3bc */
	uint32_t clk_src_i2s4;				/* _CLK_SOURCE_I2S4,        0x3c0 */
	uint32_t clk_src_i2c4;				/* _CLK_SOURCE_I2C4,        0x3c4 */
	uint32_t clk_src_sbc5;				/* _CLK_SOURCE_SBC5,        0x3c8 */
	uint32_t clk_src_sbc6;				/* _CLK_SOURCE_SBC6,        0x3cc */
	uint32_t clk_src_audio;				/* _CLK_SOURCE_AUDIO,       0x3d0 */
	uint32_t _rsv25;					/*                          0x3d4 */
	uint32_t clk_src_dam0;				/* _CLK_SOURCE_DAM0,        0x3d8 */
	uint32_t clk_src_dam1;				/* _CLK_SOURCE_DAM1,        0x3dc */
	uint32_t clk_src_dam2;				/* _CLK_SOURCE_DAM2,        0x3e0 */
	uint32_t clk_src_hda2codec_2x;		/* _CLK_SOURCE_HDA2CODEC_2X,0x3e4 */
	uint32_t clk_src_actmon;			/* _CLK_SOURCE_ACTMON,      0x3e8 */
	uint32_t clk_src_extperiph1;		/* _CLK_SOURCE_EXTPERIPH1,  0x3ec */
	uint32_t clk_src_extperiph2;		/* _CLK_SOURCE_EXTPERIPH2,  0x3f0 */
	uint32_t clk_src_extperiph3;		/* _CLK_SOURCE_EXTPERIPH3,  0x3f4 */
	uint32_t clk_src_nand_speed;		/* _CLK_SOURCE_NAND_SPEED,  0x3f8 */
	uint32_t clk_src_i2c_slow;			/* _CLK_SOURCE_I2C_SLOW,    0x3fc */
	uint32_t clk_src_sys;				/* _CLK_SOURCE_SYS,         0x400 */
	uint32_t _rsv26[4];					/*                      0x404-410 */
	uint32_t clk_src_sor;				/* _CLK_SOURCE_SOR_0,       0x414 */
	uint32_t _rsv261[2];				/*                      0x404-410 */
	uint32_t clk_src_sata_oob;			/* _CLK_SOURCE_SATA_OOB,    0x420 */
	uint32_t clk_src_sata;				/* _CLK_SOURCE_SATA,        0x424 */
	uint32_t clk_src_hda;				/* _CLK_SOURCE_HDA,         0x428 */
	uint32_t _rsv27;					/*                          0x42c */
	uint32_t rst_dev_v_set;				/* _RST_DEV_V_SET,          0x430 */
	uint32_t rst_dev_v_clr;				/* _RST_DEV_V_CLR,          0x434 */
	uint32_t rst_dev_w_set;				/* _RST_DEV_W_SET,          0x438 */
	uint32_t rst_dev_w_clr;				/* _RST_DEV_W_CLR,          0x43c */
	uint32_t clk_enb_v_set;				/* _CLK_ENB_V_SET,          0x440 */
	uint32_t clk_enb_v_clr;				/* _CLK_ENB_V_CLR,          0x444 */
	uint32_t clk_enb_w_set;				/* _CLK_ENB_W_SET,          0x448 */
	uint32_t clk_enb_w_clr;				/* _CLK_ENB_W_CLR,          0x44c */
	uint32_t rst_cpug_cmplx_set;		/* _RST_CPUG_CMPLX_SET,     0x450 */
	uint32_t rst_cpug_cmplx_clr;		/* _RST_CPUG_CMPLX_CLR,     0x454 */
	uint32_t rst_cpulp_cmplx_set;		/* _RST_CPULP_CMPLX_SET,    0x458 */
	uint32_t rst_cpulp_cmplx_clr;		/* _RST_CPULP_CMPLX_CLR,    0x45C */
	uint32_t clk_cpug_cmplx_set;		/* _CLK_CPUG_CMPLX_SET,     0x460 */
	uint32_t clk_cpug_cmplx_clr;		/* _CLK_CPUG_CMPLX_CLR,     0x464 */
	uint32_t clk_cpulp_cmplx_set;		/* _CLK_CPULP_CMPLX_SET,    0x468 */
	uint32_t clk_cpulp_cmplx_clr;		/* _CLK_CPULP_CMPLX_CLR,    0x46c */
	uint32_t cpu_cmplx_status;			/* _CPU_CMPLX_STATUS,       0x470 */
	uint32_t _rsv28;					/*                          0x474 */
	uint32_t intstatus;					/* _INTSTATUS,              0x478 */
	uint32_t intmask;					/* _INTMASK,                0x47c */
	uint32_t utmip_pll_cfg0;			/* _UTMIP_PLL_CFG0,         0x480 */
	uint32_t utmip_pll_cfg1;			/* _UTMIP_PLL_CFG1,         0x484 */
	uint32_t utmip_pll_cfg2;			/* _UTMIP_PLL_CFG2,         0x488 */
	uint32_t plle_aux;					/* _PLLE_AUX,               0x48c */
	uint32_t sata_pll_cfg0;				/* _SATA_PLL_CFG0,          0x490 */
	uint32_t sata_pll_cfg1;				/* _SATA_PLL_CFG1,          0x494 */
	uint32_t pcie_pll_cfg0;				/* _PCIE_PLL_CFG0,          0x498 */
	uint32_t prog_audio_dly_clk;		/* _PROG_AUDIO_DLY_CLK,     0x49c */
	uint32_t audio_sync_clk_i2s0;		/* _AUDIO_SYNC_CLK_I2S0,    0x4a0 */
	uint32_t audio_sync_clk_i2s1;		/* _AUDIO_SYNC_CLK_I2S1,    0x4a4 */
	uint32_t audio_sync_clk_i2s2;		/* _AUDIO_SYNC_CLK_I2S2,    0x4a8 */
	uint32_t audio_sync_clk_i2s3;		/* _AUDIO_SYNC_CLK_I2S3,    0x4ac */
	uint32_t audio_sync_clk_i2s4;		/* _AUDIO_SYNC_CLK_I2S4,    0x4b0 */
	uint32_t audio_sync_clk_spdif;		/* _AUDIO_SYNC_CLK_SPDIF,   0x4b4 */
	uint32_t plld2_base;				/* _PLLD2_BASE,             0x4b8 */
	uint32_t plld2_misc;				/* _PLLD2_MISC,             0x4bc */
	uint32_t utmip_pll_cfg3;			/* _UTMIP_PLL_CFG3,         0x4c0 */
	uint32_t pllrefe_base;				/* _PLLREFE_BASE,           0x4c4 */
	uint32_t pllrefe_misc;				/* _PLLREFE_MISC,           0x4c8 */
	uint32_t pllrefe_out0;				/* _PLLREFE_OUT_0,          0x4cc */
	uint32_t _rsv29[6];					/*                      0x4d0-4e4 */
	uint32_t pllc2_base;				/* _PLLC2_BASE,             0x4e8 */
	uint32_t pllc2_misc0;				/* _PLLC2_MISC_0,           0x4ec */
	uint32_t pllc2_misc1;				/* _PLLC2_MISC_1,           0x4f0 */
	uint32_t pllc2_misc2;				/* _PLLC2_MISC_2,           0x4f4 */
	uint32_t pllc2_misc3;				/* _PLLC2_MISC_3,           0x4f8 */
	uint32_t pllc3_base;				/* _PLLC3_BASE,             0x4fc */
	uint32_t pllc3_misc0;				/* _PLLC3_MISC_0,           0x500 */
	uint32_t pllc3_misc1;				/* _PLLC3_MISC_1,           0x504 */
	uint32_t pllc3_misc2;				/* _PLLC3_MISC_2,           0x508 */
	uint32_t pllc3_misc3;				/* _PLLC3_MISC_3,           0x50c */
	uint32_t pllx_misc1;				/* _PLLX_MISC_1,            0x510 */
	uint32_t pllx_misc2;				/* _PLLX_MISC_2,            0x514 */
	uint32_t pllx_misc3;				/* _PLLX_MISC_3,            0x518 */
	uint32_t xusbio_pll_cfg0;			/* _XUSBIO_PLL_CFG0,        0x51c */
	uint32_t xusbio_pll_cfg1;			/* _XUSBIO_PLL_CFG1,        0x520 */
	uint32_t plle_aux1;					/* _PLLE_AUX1,              0x524 */
	uint32_t pllp_reshift;				/* _PLLP_RESHIFT,           0x528 */
	uint32_t utmipll_hw_pwrdn_cfg0;		/* _UTMIPLL_HW_PWRDN_CFG0,  0x52c */
	uint32_t pllu_hw_pwrdn_cfg0;		/* _PLLU_HW_PWRDN_CFG0,     0x530 */
	uint32_t xusb_pll_cfg0;				/* _XUSB_PLL_CFG0,          0x534 */
	uint32_t _rsv30;					/*                          0x538 */
	uint32_t clk_cpu_misc;				/* _CLK_CPU_MISC,           0x53c */
	uint32_t clk_cpug_misc;				/* _CLK_CPUG_MISC,          0x540 */
	uint32_t clk_cpulp_misc;			/* _CLK_CPULP_MISC,         0x544 */
	uint32_t pllx_hw_ctrl_cfg;			/* _PLLX_HW_CTRL_CFG,       0x548 */
	uint32_t pllx_sw_ramp_cfg;			/* _PLLX_SW_RAMP_CFG,       0x54c */
	uint32_t pllx_hw_ctrl_status;		/* _PLLX_HW_CTRL_STATUS,    0x550 */
	uint32_t _rsv31;					/*                          0x554 */
	uint32_t super_gr3d_clk_div;		/* _SUPER_GR3D_CLK_DIVIDER, 0x558 */
	uint32_t spare_reg0;				/* _SPARE_REG0,             0x55c */
	uint32_t _rsv32[4];					/*                    0x560-0x56c */
	uint32_t plld2_ss_cfg;				/* _PLLD2_SS_CFG            0x570 */
	uint32_t _rsv32_1[7];				/*                      0x574-58c */
	uint32_t plldp_base;				/* _PLLDP_BASE,             0x590 */
	uint32_t plldp_misc;				/* _PLLDP_MISC,             0x594 */
	uint32_t plldp_ss_cfg;				/* _PLLDP_SS_CFG,           0x598 */
	uint32_t plldp_ss_ctrl10;			/* _PLLDP_SS_CTRL1_0,       0x59c */
	uint32_t plldp_ss_ctrl20;			/* _PLLDP_SS_CTRL2_0,       0x5a0 */
	uint32_t pllc4_base0;				/* _PLLC4_BASE_0,           0x5a4 */
	uint32_t pllc4_misc0;				/* _PLLC4_MISC_0,           0x5a8 */
	uint32_t _rsrv32_2[14];
	uint32_t pllc4_out0;				/* _PLLC4_OUT_0,            0x5e4 */
	uint32_t _rsrv32_3[6];
	uint32_t clk_src_xusb_core_host;	/* _CLK_SOURCE_XUSB_CORE_HOST 0x600 */
	uint32_t clk_src_xusb_falcon;		/* _CLK_SOURCE_XUSB_FALCON  0x604 */
	uint32_t clk_src_xusb_fs;			/* _CLK_SOURCE_XUSB_FS      0x608 */
	uint32_t clk_src_xusb_core_dev;		/* _CLK_SOURCE_XUSB_CORE_DEV 0x60c */
	uint32_t clk_src_xusb_ss;			/* _CLK_SOURCE_XUSB_SS      0x610 */
	uint32_t clk_src_cilab;				/* _CLK_SOURCE_CILAB        0x614 */
	uint32_t clk_src_cilcd;				/* _CLK_SOURCE_CILCD        0x618 */
	uint32_t clk_src_cile;				/* _CLK_SOURCE_CILE         0x61c */
	uint32_t clk_src_dsia_lp;			/* _CLK_SOURCE_DSIA_LP      0x620 */
	uint32_t clk_src_dsib_lp;			/* _CLK_SOURCE_DSIB_LP      0x624 */
	uint32_t clk_src_entropy;			/* _CLK_SOURCE_ENTROPY      0x628 */
	uint32_t clk_src_dvfs_ref;			/* _CLK_SOURCE_DVFS_REF     0x62c */
	uint32_t clk_src_dvfs_soc;			/* _CLK_SOURCE_DVFS_SOC     0x630 */
	uint32_t clk_src_traceclkin;		/* _CLK_SOURCE_TRACECLKIN   0x634 */
	uint32_t clk_src_adx0;				/* _CLK_SOURCE_ADX0         0x638 */
	uint32_t clk_src_amx0;				/* _CLK_SOURCE_AMX0         0x63c */
	uint32_t clk_src_emc_latency;		/* _CLK_SOURCE_EMC_LATENCY  0x640 */
	uint32_t clk_src_soc_therm;			/* _CLK_SOURCE_SOC_THERM    0x644 */
	uint32_t _rsv33[5];					/*                      0x648-658 */
	uint32_t clk_src_i2c6;				/* _CLK_SOURCE_I2C6,        0x65c */
	uint32_t clk_src_mipibif;			/* _CLK_SOURCE_MIPIBIF,     0x660 */
	uint32_t clk_src_emc_dll;			/* _CLK_SOURCE_EMC_DLL,     0x664 */
	uint32_t _rsv34;					/*                          0x668 */
	uint32_t clk_src_uart_fst_mipi_cal;	/* _CLK_SOURCE_UART_FST_MIP_CAL, 0x66c */
	uint32_t _rsv35[2];					/*                      0x670-674 */
	uint32_t clk_src_vic0;				/* _CLK_SOURCE_VIC_0,       0x678 */
	uint32_t pllp_outc0;				/* _PLLP_OUTC_0,            0x67c */
	uint32_t pllp_misc10;				/* _PLLP_MISC1_0,           0x680 */
	uint32_t _rsv36[2];					/*                      0x684-688 */
	uint32_t emc_div_clk_shaper_ctrl0;	/* _EMC_DIV_CLK_SHAPER_CTRL_0, 0x68c */
	uint32_t emc_pllc_shaper_ctrl0;		/* _EMC_PLLC_SHAPER_CTRL_0, 0x690 */
	uint32_t clk_src_sdmmc_legacy_tm0;	/* _CLK_SOURCE_SDMMC_LEGACY_TM_0, 0x694 */
	uint32_t clk_src_nvdec0;			/* _CLK_SOURCE_NVDEC_0, 	0x698 */
	uint32_t clk_src_nvjpg0;			/* _CLK_SOURCE_NVJPG_0, 	0x69c */
	uint32_t clk_src_nvenc0;			/* _CLK_SOURCE_NVENC_0,		0x6a0 */
	uint32_t plla1_base0;				/* _PLLA1_BASE_0,			0x6a4 */
	uint32_t plla1_misc00;				/* _PLLA1_MISC_0_0, 		0x6a8 */
	uint32_t plla1_misc10;				/* _PLLA1_MISC_1_0, 		0x6ac */
	uint32_t plla1_misc20;				/* _PLLA1_MISC_2_0, 		0x6b0 */
	uint32_t plla1_misc30;				/* _PLLA1_MISC_3_0, 		0x6b4 */
	uint32_t audio_sync_clk_dmic30;		/* _AUDIO_SYNC_CLK_DMIC3_0, 0x6b8 */
	uint32_t clk_src_dmic30;			/* _CLK_SOURCE_DMIC3_0, 	0x6bc */
	uint32_t clk_src_ape0;				/* _CLK_SOURCE_APE_0, 		0x6c0 */
	uint32_t clk_src_qspi;				/* _CLK_SOURCE_QSPI         0x6c4 */
};

#endif /* __NSWITCH_REG_CLKRST_H__ */
