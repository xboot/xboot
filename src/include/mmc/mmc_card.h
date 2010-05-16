#ifndef __MMC_CARD_H__
#define __MMC_CARD_H__

#include <configs.h>
#include <default.h>
#include <string.h>
#include <xboot/list.h>
#include <mmc/mmc_host.h>

/*
 * card type
 */
enum mmc_card_type {
	MMC_CARD_TYPE_NONE,
	MMC_CARD_TYPE_MMC,
	MMC_CARD_TYPE_SD,
	MMC_CARD_TYPE_SD20,
	MMC_CARD_TYPE_SDHC,
};

/*
 * mmc's cid
 */
struct mmc_cid {
	x_u8	mid;			/* card manufacturer id */
	x_u16	oid;			/* oem / application id */
	x_u8	pnm[8];			/* product name */
	x_u8	hwrev;			/* product hardware revision */
	x_u8	fwrev;			/* product firmware revision */
	x_u32	serial;			/* serial number */
	x_u16	year;			/* year of manufacture date */
	x_u16	month;			/* month of manufacture date */
};

/*
 * mmc's csd
 */
struct mmc_csd {
	x_u8	mmca_vsn;
	x_u16	cmdclass;
	x_u16	tacc_clks;
	x_u32	tacc_ns;
	x_u32	r2w_factor;
	x_u32	max_dtr;
	x_u32	read_blkbits;
	x_u32	write_blkbits;
	x_u32	capacity;
	x_u32	read_partial	:1,
			read_misalign	:1,
			write_partial	:1,
			write_misalign	:1;
};

/*
 * mmc card information
 */
struct mmc_card_info
{
	/*
	 * raw data, filled by mmc host controller.
	 */
	enum mmc_card_type	type;
	x_u16				rca;
	x_u32				raw_cid[4];
	x_u32				raw_csd[4];

	/*
	 * auto decoded when probed
	 */
	struct mmc_cid 		cid;
	struct mmc_csd 		csd;

	x_u32				sector_size;
	x_u32				sector_count;
	x_size				capacity;
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

