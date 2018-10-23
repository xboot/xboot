#ifndef __SDHCI_H__
#define __SDHCI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <sd/sdmmc.h>

struct sdhci_cmd_t {
	u32_t cmdidx;
	u32_t cmdarg;
	u32_t resptype;
	u32_t response[4];
};

struct sdhci_data_t {
	u8_t * buf;
	u32_t flag;
	u32_t blksz;
	u32_t blkcnt;
};

struct sdhci_t
{
	char * name;
	u32_t voltage;
	u32_t width;
	u32_t clock;
	bool_t removable;
	bool_t isspi;
	void * sdcard;

	bool_t (*detect)(struct sdhci_t * hci);
	bool_t (*setvoltage)(struct sdhci_t * hci, u32_t voltage);
	bool_t (*setwidth)(struct sdhci_t * hci, u32_t width);
	bool_t (*setclock)(struct sdhci_t * hci, u32_t clock);
	bool_t (*transfer)(struct sdhci_t * hci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat);
	void * priv;
};

struct sdhci_t * search_sdhci(const char * name);
bool_t register_sdhci(struct device_t ** device, struct sdhci_t * hci);
bool_t unregister_sdhci(struct sdhci_t * hci);

bool_t sdhci_detect(struct sdhci_t * hci);
bool_t sdhci_set_voltage(struct sdhci_t * hci, u32_t voltage);
bool_t sdhci_set_width(struct sdhci_t * hci, u32_t width);
bool_t sdhci_set_clock(struct sdhci_t * hci, u32_t clock);
bool_t sdhci_transfer(struct sdhci_t * hci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat);

#ifdef __cplusplus
}
#endif

#endif /* __SDHCI_H__ */
