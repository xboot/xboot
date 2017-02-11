#ifndef __BUS_MMC_H__
#define __BUS_MMC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct mmc_msg_t {
	void * txbuf;
	void * rxbuf;
	int len;
	int mode;
	int bits;
	int speed;
};

struct mmc_cmd_t {
	u32_t cmdidx;
	u32_t cmdarg;
	u32_t resptype;
	u32_t response[4];
};

struct mmc_data_t {
	union {
		u8_t * dest;
		const u8_t * src;
	};
	u32_t flag;
	u32_t blocks;
	u32_t blocksize;
};

struct mmc_ios_t {
	u32_t buswidth;
	u32_t clock;
};

struct mmc_t
{
	char * name;

	void (*reset)(struct mmc_t * mmc);
	bool_t (*getcd)(struct mmc_t * mmc);
	bool_t (*getwp)(struct mmc_t * mmc);
	bool_t (*setios)(struct mmc_t * mmc, struct mmc_ios_t * ios);
	bool_t (*request)(struct mmc_t * mmc, struct mmc_cmd_t * cmd, struct mmc_data_t * dat);
	void * priv;
};

struct mmc_t * search_mmc(const char * name);
bool_t register_mmc(struct device_t ** device, struct mmc_t * mmc);
bool_t unregister_mmc(struct mmc_t * mmc);

#ifdef __cplusplus
}
#endif

#endif /* __BUS_MMC_H__ */
