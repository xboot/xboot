#ifndef __MMC_HOST_H__
#define __MMC_HOST_H__

#include <configs.h>
#include <default.h>
#include <string.h>
#include <xboot/list.h>
#include <mmc/mmc.h>


#define MMC_VDD_145_150				0x00000001		/* VDD voltage 1.45 - 1.50 */
#define MMC_VDD_150_155				0x00000002		/* VDD voltage 1.50 - 1.55 */
#define MMC_VDD_155_160				0x00000004		/* VDD voltage 1.55 - 1.60 */
#define MMC_VDD_160_165				0x00000008		/* VDD voltage 1.60 - 1.65 */
#define MMC_VDD_165_170				0x00000010		/* VDD voltage 1.65 - 1.70 */
#define MMC_VDD_17_18				0x00000020		/* VDD voltage 1.7 - 1.8 */
#define MMC_VDD_18_19				0x00000040		/* VDD voltage 1.8 - 1.9 */
#define MMC_VDD_19_20				0x00000080		/* VDD voltage 1.9 - 2.0 */
#define MMC_VDD_20_21				0x00000100		/* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22				0x00000200		/* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23				0x00000400		/* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24				0x00000800		/* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25				0x00001000		/* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26				0x00002000		/* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27				0x00004000		/* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28				0x00008000		/* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29				0x00010000		/* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30				0x00020000		/* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31				0x00040000		/* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32				0x00080000		/* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33				0x00100000		/* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34				0x00200000		/* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35				0x00400000		/* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36				0x00800000		/* VDD voltage 3.5 ~ 3.6 */
#define MMC_CARD_BUSY				0x80000000		/* Card Power up status bit */

/*
 * define the struct of mmc host controller.
 */
struct mmc_host
{
	/* the name of mmc host controller */
	char * name;

	/*initialize the mmc host controller */
	void (*init)(void);

	/* private data */
	void * priv;
};

/*
 * the list of mmc host controller.
 */
struct mmc_host_list
{
	struct mmc_host * host;
	struct hlist_node node;
};

struct mmc_host * search_mmc_host(const char * name);
x_bool register_mmc_host(struct mmc_host * host);
x_bool unregister_mmc_host(struct mmc_host * host);

#endif /* __MMC_HOST_H__ */
