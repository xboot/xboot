#ifndef __NFC_H__
#define __NFC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
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
	void (*init)(void);

	/* clean up the nand flash controller */
	void (*exit)(void);

	/* control nand flash controller */
	bool_t (*control)(struct nand_device * nand, enum nand_control ctl);

	/* issue a command to the nand device */
	bool_t (*command)(struct nand_device * nand, u32_t cmd);

	/* write an address to the nand device */
	bool_t (*address)(struct nand_device * nand, u32_t addr);

	/* read a data from nand device */
	bool_t (*read_data)(struct nand_device * nand, u32_t * data);

	/* write a data to the nand device */
	bool_t (*write_data)(struct nand_device * nand, u32_t data);

	/* check nand flash is ready */
	bool_t (*nand_ready)(struct nand_device * nand, s32_t timeout);
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
bool_t register_nfc(struct nfc * nfc);
bool_t unregister_nfc(struct nfc * nfc);

#ifdef __cplusplus
}
#endif

#endif /* __NFC_H__ */
