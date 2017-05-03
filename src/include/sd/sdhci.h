#ifndef __SDHCI_H__
#define __SDHCI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <sd/mmc.h>

struct sdhci_cmd_t {
	u32_t cmdidx;
	u32_t cmdarg;
	u32_t resptype;
	u32_t response[4];
};

struct sdhci_data_t {
	union {
		u8_t * dest;
		const u8_t * src;
	};
	u32_t flag;
	u32_t blocks;
	u32_t blocksize;
};

struct sdhci_t
{
	char * name;
	u32_t voltages;

	void (*reset)(struct sdhci_t * sdhci);
	bool_t (*detect)(struct sdhci_t * sdhci);
	bool_t (*setwidth)(struct sdhci_t * sdhci, int width);
	bool_t (*setclock)(struct sdhci_t * sdhci, int clock);
	bool_t (*transfer)(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat);
	void * priv;
};

struct sdhci_t * search_sdhci(const char * name);
bool_t register_sdhci(struct device_t ** device, struct sdhci_t * sdhci);
bool_t unregister_sdhci(struct sdhci_t * sdhci);

void sdhci_reset(struct sdhci_t * sdhci);
bool_t sdhci_detect(struct sdhci_t * sdhci);
bool_t sdhci_set_width(struct sdhci_t * sdhci, int width);
bool_t sdhci_set_clock(struct sdhci_t * sdhci, int clock);
bool_t sdhci_transfer(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat);

#ifdef __cplusplus
}
#endif

#endif /* __SDHCI_H__ */
