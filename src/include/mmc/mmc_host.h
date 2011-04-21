#ifndef __MMC_HOST_H__
#define __MMC_HOST_H__

#include <configs.h>
#include <default.h>
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
	x_bool (*probe)(struct mmc_card_info * info);

	/* read sectors from mmc card */
	x_s32 (*read_sector)(struct mmc_card * card, x_u8 * buf, x_u32 sector, x_u32 count);

	/* write sectors to mmc card */
	x_s32 (*write_sector)(struct mmc_card * card, const x_u8 * buf, x_u32 sector, x_u32 count);
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
x_bool register_mmc_host(struct mmc_host * host);
x_bool unregister_mmc_host(struct mmc_host * host);

#endif /* __MMC_HOST_H__ */
