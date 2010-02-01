#ifndef __MMC_CARD_H__
#define __MMC_CARD_H__

#include <configs.h>
#include <default.h>
#include <string.h>
#include <xboot/list.h>

/*
 * mmc_card device of structure
 */
struct mmc_card
{
	/* the name of mmc card */
	char name[32 + 1];

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

