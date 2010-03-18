#ifndef __MMC_CARD_H__
#define __MMC_CARD_H__

#include <configs.h>
#include <default.h>
#include <string.h>
#include <xboot/list.h>
#include <mmc/mmc_host.h>

/*
 * mmc's cid
 */
struct mmc_cid {
	x_u8	mid;
	x_u8	oid[2];
	x_u8	pnm[5];
	x_u8	prev;
	x_u8	psn;
	x_u8	hwrev[2];
	x_u8	fwrev[2];
	x_u8	year;
	x_u8	month;
};

/*
 * mmc's csd
 */
struct mmc_csd {
	x_u8	mmca_vsn;
};

/*
 * mmc's scr
 */
struct mmc_scr {
	x_u8	sda_vsn;
	x_u8	bus_widths;
};

/*
 * mmc card information
 */
struct mmc_card_info
{
	struct mmc_cid 		cid;
	struct mmc_csd 		csd;
	struct mmc_scr		scr;
};

/*
 * mmc_card device of structure
 */
struct mmc_card
{
	/* the name of mmc card */
	char name[32 + 1];

	/* mmc card information */
	struct mmc_card_info * info;

	/* the driver of mmc host controller */
	struct mmc_host * host;

	/* priv data */
	void * priv;
};

/*
 * the list of mmc card
 */
struct mmc_card_list
{
	struct mmc_card * card;
	struct list_head entry;
};


void mmc_card_probe(void);
void mmc_card_remove(void);

struct mmc_card * search_mmc_card(const char * name);


#endif /* __MMC_CARD_H__ */

