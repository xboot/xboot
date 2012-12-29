#ifndef __MMC_HOST_H__
#define __MMC_HOST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <string.h>
#include <xboot/list.h>
#include <mmc/mmc_card.h>

/*
 * define mmc host controller
 */
struct mmc_host
{
	/* the name of mmc host controller */
	char * name;

	/* initialize the mmc host controller */
	void (*init)(void);

	/* clean up the mmc host controller */
	void (*exit)(void);

	/* probe mmc card's information */
	bool_t (*probe)(struct mmc_card_info * info);

	/* read sectors from mmc card */
	bool_t (*read_sectors)(struct mmc_card * card, u8_t * buf, u32_t sector, u32_t count);

	/* write sectors to mmc card */
	bool_t (*write_sectors)(struct mmc_card * card, const u8_t * buf, u32_t sector, u32_t count);
};

/*
 * the list of mmc host controller.
 */
struct mmc_host_list
{
	struct mmc_host * host;
	struct list_head entry;
};

struct mmc_host * search_mmc_host(const char * name);
bool_t register_mmc_host(struct mmc_host * host);
bool_t unregister_mmc_host(struct mmc_host * host);

#ifdef __cplusplus
}
#endif

#endif /* __MMC_HOST_H__ */
