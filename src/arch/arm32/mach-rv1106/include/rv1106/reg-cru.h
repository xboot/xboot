#ifndef __X1106_REG_CRU_H__
#define __X1106_REG_CRU_H__

#define X1106_CRU_BASE		(0xff3a0000)

struct rv1106_pll_t {
	unsigned int con0;
	unsigned int con1;
	unsigned int con2;
	unsigned int con3;
	unsigned int con4;
	unsigned int reserved0[3];
};

struct rv1106_cru_t {
	unsigned int reserved0[192];
	unsigned int pmu_clksel_con[8];
	unsigned int reserved1[312];
	unsigned int pmu_clkgate_con[3];
	unsigned int reserved2[125];
	unsigned int pmu_softrst_con[3];
	unsigned int reserved3[15741];
	struct rv1106_pll_t pll[4];
	unsigned int reserved4[128];
	unsigned int mode;
	unsigned int reserved5[31];
	unsigned int clksel_con[34];
	unsigned int reserved6[286];
	unsigned int clkgate_con[4];
	unsigned int reserved7[124];
	unsigned int softrst_con[3];
	unsigned int reserved8[125];
	unsigned int glb_cnt_th;
	unsigned int glb_rst_st;
	unsigned int glb_srst_fst;
	unsigned int glb_srst_snd;
	unsigned int glb_rst_con;
	unsigned int con[2];
	unsigned int sdmmc_con[2];
	unsigned int emmc_con[2];
	unsigned int reserved9[1461];
	unsigned int peri_clksel_con[12];
	unsigned int reserved10[308];
	unsigned int peri_clkgate_con[8];
	unsigned int reserved11[120];
	unsigned int peri_softrst_con[8];
	unsigned int reserved12[1592];
	unsigned int vi_clksel_con[4];
	unsigned int reserved13[316];
	unsigned int vi_clkgate_con[3];
	unsigned int reserved14[125];
	unsigned int vi_softrst_con[3];
	unsigned int reserved15[3645];
	unsigned int core_clksel_con[5];
	unsigned int reserved16[2043];
	unsigned int vepu_clksel_con[2];
	unsigned int reserved17[318];
	unsigned int vepu_clkgate_con[3];
	unsigned int reserved18[125];
	unsigned int vepu_softrst_con[2];
	unsigned int reserved19[1598];
	unsigned int vo_clksel_con[4];
	unsigned int reserved20[316];
	unsigned int vo_clkgate_con[3];
	unsigned int reserved21[125];
	unsigned int vo_softrst_con[4];
};

#endif /* __X1106_REG_CRU_H__ */
