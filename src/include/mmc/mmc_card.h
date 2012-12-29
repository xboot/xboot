#ifndef __MMC_CARD_H__
#define __MMC_CARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
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
	u8_t	mid;			/* card manufacturer id */
	u16_t	oid;			/* oem / application id */
	u8_t	pnm[8];			/* product name */
	u8_t	hwrev;			/* product hardware revision */
	u8_t	fwrev;			/* product firmware revision */
	u32_t	serial;			/* serial number */
	u16_t	year;			/* year of manufacture date */
	u16_t	month;			/* month of manufacture date */
};

/*
 * mmc's csd
 */
struct mmc_csd {
	u8_t	mmca_vsn;
	u16_t	cmdclass;
	u16_t	tacc_clks;
	u32_t	tacc_ns;
	u32_t	r2w_factor;
	u32_t	max_dtr;
	u32_t	read_blkbits;
	u32_t	write_blkbits;
	u32_t	capacity;
	u32_t	read_partial	:1,
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
	u16_t				rca;
	u32_t				raw_cid[4];
	u32_t				raw_csd[4];

	/*
	 * auto decoded when probed
	 */
	struct mmc_cid 		cid;
	struct mmc_csd 		csd;

	size_t				sector_size;
	size_t				sector_count;
	loff_t				capacity;
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

#ifdef __cplusplus
}
#endif

#endif /* __MMC_CARD_H__ */

