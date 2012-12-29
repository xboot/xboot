#ifndef __NAND_H__
#define __NAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <string.h>
#include <xboot/list.h>
#include <mtd/nand/nand_ids.h>
#include <mtd/nand/nfc.h>

/*
 * nand flash commands
 */
enum
{
	/* standard */
	NAND_CMD_READ0			= 0x00,
	NAND_CMD_READ1			= 0x01,
	NAND_CMD_RNDOUT			= 0x05,
	NAND_CMD_PAGEPROG		= 0x10,
	NAND_CMD_READOOB		= 0x50,
	NAND_CMD_ERASE1			= 0x60,
	NAND_CMD_STATUS			= 0x70,
	NAND_CMD_STATUS_MULTI	= 0x71,
	NAND_CMD_SEQIN			= 0x80,
	NAND_CMD_RNDIN			= 0x85,
	NAND_CMD_READID			= 0x90,
	NAND_CMD_ERASE2			= 0xd0,
	NAND_CMD_RESET			= 0xff,

	/* extended */
	NAND_CMD_READSTART		= 0x30,
	NAND_CMD_RNDOUTSTART	= 0xE0,
	NAND_CMD_CACHEDPROG		= 0x15,
};

/* status bits */
enum
{
	NAND_STATUS_FAIL		= 0x01,
	NAND_STATUS_FAIL_N1		= 0x02,
	NAND_STATUS_TRUE_READY	= 0x20,
	NAND_STATUS_READY		= 0x40,
	NAND_STATUS_WP			= 0x80,
};

/* oob data formats */
enum oob_formats
{
	NAND_OOB_NONE			= 0x000,	/* no oob data at all */
	NAND_OOB_RAW			= 0x001,	/* raw oob data (16 bytes for 512b page sizes, 64 bytes for 2048b page sizes) */
	NAND_OOB_ONLY			= 0x002,	/* only oob data */
	NAND_OOB_SW_ECC			= 0x010,	/* when writing, use software ecc */
	NAND_OOB_HW_ECC			= 0x020, 	/* when writing, use hardware ecc */
	NAND_OOB_SW_ECC_KW		= 0x040, 	/* when writing, use marvell's kirkwood bootrom format */
	NAND_OOB_JFFS2			= 0x100,	/* when writing, use jffs2 oob layout */
	NAND_OOB_YAFFS2			= 0x200,	/* when writing, use yaffs2 oob layout */
};

/*
 * representation of a single nand block in a nand device.
 */
struct nand_block
{
	/* offset to the block */
	loff_t offset;

	/* size of the block. */
	loff_t size;

	/* true if the block has been erased */
	bool_t is_erased;

	/* true if the block is bad */
	bool_t is_bad;
};

/*
 * ecc layout structure
 */
struct nand_ecclayout
{
	s32_t eccbytes;
	s32_t eccpos[64];
	s32_t oobavail;

	struct nand_oobfree	{
		s32_t offset;
		s32_t length;
	} oobfree[2];
};

/*
 * nand device of structure
 */
struct nand_device
{
	/* the name of nand flash */
	char name[32 + 1];

	/* nand information */
	struct nand_info * info;

	/* nand manufacturer */
	struct nand_manufacturer * manufacturer;

	/* bus width */
	s32_t bus_width;

	/* address cycles */
	s32_t addr_cycles;

	/* page size */
	s32_t page_size;

	/* erase size */
	s32_t erase_size;

	/* the number of blocks */
	s32_t num_blocks;

	/* nand blocks */
	struct nand_block * blocks;

	/* the driver of nand flash controller */
	struct nfc * nfc;

	/* priv data */
	void * priv;
};

/*
 * the list of nand device
 */
struct nand_list
{
	struct nand_device * nand;
	struct list_head entry;
};


void nand_flash_probe(void);
void nand_flash_remove(void);

struct nand_device * search_nand_device(const char * name);
s32_t nand_read(struct nand_device * nand, u8_t * buf, u32_t addr, u32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __NAND_H__ */
