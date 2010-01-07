#ifndef __NFC_H__
#define __NFC_H__

#include <configs.h>
#include <default.h>
#include <string.h>
#include <xboot/list.h>
#include <mtd/nand/nand.h>

/*
 * keep gcc happy.
 */
struct nand_device;

enum nand_control
{
	NAND_ENABLE_CONTROLLER,
	NAND_DISABLE_CONTROLLER,
	NAND_ENABLE_CE,
	NAND_DISABLE_CE,
};

/*
 * define the struct of nand flash controller.
 */
struct nfc
{
	/* the name of nand flash controller */
	char * name;

	/* initialize the nand flash controller */
	void (*init)(struct nand_device * nand);

	/* clean up the nand flash controller */
	void (*exit)(struct nand_device * nand);

	/* control nand flash controller */
	x_bool (*control)(struct nand_device * nand, enum nand_control ctl);

	/* issue a command to the nand device */
	x_bool (*command)(struct nand_device * nand, x_u32 cmd);

	/* write an address to the nand device */
	x_bool (*address)(struct nand_device * nand, x_u32 addr);

	/* write data to the nand device */
	x_bool (*write_data)(struct nand_device * nand, x_u32 data);

	/* read data from nand device */
	x_bool (*read_data)(struct nand_device * nand, x_u32 * data);

	/* write a page to the nand device */
	x_bool (*write_page)(struct nand_device * nand, x_u32 page, x_u8 * data, x_u32 data_size, x_u8 * oob, x_u32 oob_size);

	/* read a page from the nand device */
	x_bool (*read_page)(struct nand_device * nand, x_u32 page, x_u8 * data, x_u32 data_size, x_u8 * oob, x_u32 oob_size);

	/* write block data to nand device */
	x_bool (*write_block_data)(struct nand_device * nand, x_u8 * data, x_size size);

	/* read block data from nand device */
	x_bool (*read_block_data)(struct nand_device * nand, x_u8 * data, x_size size);

	/* check nand flash controller is ready */
	x_bool (*controller_ready)(struct nand_device * nand, x_s32 timeout);

	/* check nand flash is ready */
	x_bool (*nand_ready)(struct nand_device * nand, x_s32 timeout);
};


/*
 * the list of nand flash controller
 */
struct nfc_list
{
	struct nfc * nfc;
	struct list_head entry;
};


struct nfc * search_nfc(const char *name);
x_bool register_nfc(struct nfc * nfc);
x_bool unregister_nfc(struct nfc * nfc);

#endif /* __NFC_H__ */
