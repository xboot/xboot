#ifndef __X1106_REG_GRF_H__
#define __X1106_REG_GRF_H__

#define X1106_GRF_BASE		(0xff000000)

struct rv1106_grf_t {
	unsigned int reserved0[0x20 / 4];
	unsigned int emmc_con0;
	unsigned int emmc_con1;
	unsigned int reserved1[(0x050000 - 0x24) / 4];
	unsigned int sdmmc_con0;
	unsigned int sdmmc_con1;
	unsigned int reserved2[(0x060000 - 0x05000c) / 4];
	unsigned int gmac_con0;
	unsigned int gmac_clk_con;
	unsigned int gmac_st;
	unsigned int tsadc_con;
	unsigned int otp_con;
	unsigned int rga_noc;
	unsigned int vo_mem;
	unsigned int sdio_con0;
	unsigned int sdio_con1;
	unsigned int sdio_ram_clhgat_dis;
	unsigned int macphy_con0;
	unsigned int macphy_con1;
	unsigned int vop_qos;
	unsigned int vop_pipe_bypass;
};

#endif /* __X1106_REG_GRF_H__ */
