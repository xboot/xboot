#ifndef __NAND_IDS_H__
#define __NAND_IDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/*
 * nand flash manufacturer id codes
 */
enum
{
	NAND_MFR_UNKOWN		= 0xff,
	NAND_MFR_TOSHIBA	= 0x98,
	NAND_MFR_SAMSUNG	= 0xec,
	NAND_MFR_FUJITSU	= 0x04,
	NAND_MFR_NATIONAL	= 0x8f,
	NAND_MFR_RENESAS	= 0x07,
	NAND_MFR_STMICRO	= 0x20,
	NAND_MFR_HYNIX		= 0xad,
	NAND_MFR_MICRON		= 0x2c,
	NAND_MFR_AMD		= 0x01,
};

/*
 * nand flash manufacturer id structure
 */
struct nand_manufacturer
{
	/* manufacturer id */
	u32_t id;

	/* nand flash's name */
	char * name;
};

/*
 * option constants for bizarre disfunctionality and real features
 */
enum {
	/* chip can not auto increment pages */
	NAND_NO_AUTOINCR			= 0x00000001,

	/* bus width is 16 bit */
	NAND_BUSWIDTH_16			= 0x00000002,

	/* device supports partial programming without padding */
	NAND_NO_PADDING				= 0x00000004,

	/* chip has cache program function */
	NAND_CACHEPRG				= 0x00000008,

	/* chip has copy back function */
	NAND_COPYBACK				= 0x00000010,

	/* AND chip which has 4 banks and a confusing page / block assignment */
	NAND_IS_AND					= 0x00000020,

	/* chip has a array of 4 pages which can be read without additional ready / busy waits */
	NAND_4PAGE_ARRAY			= 0x00000040,

	/* chip requires that BBT is periodically rewritten */
	BBT_AUTO_REFRESH			= 0x00000080,

	/* chip does not require ready check on read. true for all large page devices, as they do not support autoincrement */
	NAND_NO_READRDY				= 0x00000100,

	/* options valid for samsung large page devices */
	NAND_SAMSUNG_LP_OPTIONS		= (NAND_NO_PADDING | NAND_CACHEPRG | NAND_COPYBACK),

	/* options for new chips with large page size. the pagesize and the erasesize is determined from the extended id bytes */
	LP_OPTIONS					= (NAND_SAMSUNG_LP_OPTIONS | NAND_NO_READRDY | NAND_NO_AUTOINCR),
	LP_OPTIONS16				= (LP_OPTIONS | NAND_BUSWIDTH_16),
};

/*
 * nand flash information
 */
struct nand_info
{
	/* nand flash's name */
	char * name;

	/* device id code */
	u32_t id;

	/* pagesize in bytes */
	s32_t page_size;

	/* total chip size in MB */
	s32_t chip_size;

	/* size of an erase block */
	s32_t erase_size;

	/* bitfield to store chip relevant options */
	u32_t options;
};

/*
 * extern nand_flash_ids and nand_manuf_ids.
 */
extern struct nand_info nand_flash_ids[];
extern struct nand_manufacturer nand_manuf_ids[];

#ifdef __cplusplus
}
#endif

#endif /* __NAND_IDS_H__ */
