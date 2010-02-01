#ifndef __MMC_HOST_H__
#define __MMC_HOST_H__

#include <configs.h>
#include <default.h>
#include <string.h>
#include <xboot/list.h>

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

	/* private data */
	void * priv;
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
