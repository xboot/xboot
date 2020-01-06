#ifndef __SDMMC_H__
#define __SDMMC_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	/* Class 1 */
	MMC_GO_IDLE_STATE			= 0,
	MMC_SEND_OP_COND			= 1,
	MMC_ALL_SEND_CID			= 2,
	MMC_SET_RELATIVE_ADDR 		= 3,
	MMC_SET_DSR 				= 4,
	MMC_SWITCH 					= 6,
	MMC_SELECT_CARD 			= 7,
	MMC_SEND_EXT_CSD 			= 8,
	MMC_SEND_CSD 				= 9,
	MMC_SEND_CID				= 10,
	MMC_READ_DAT_UNTIL_STOP		= 11,
	MMC_STOP_TRANSMISSION 		= 12,
	MMC_SEND_STATUS 			= 13,
	MMC_GO_INACTIVE_STATE 		= 15,
	MMC_SPI_READ_OCR 			= 58,
	MMC_SPI_CRC_ON_OFF 			= 59,

	/* Class 2 */
	MMC_SET_BLOCKLEN 			= 16,
	MMC_READ_SINGLE_BLOCK 		= 17,
	MMC_READ_MULTIPLE_BLOCK		= 18,

	/* Class 3 */
	MMC_WRITE_DAT_UNTIL_STOP	= 20,

	/* Class 4 */
	MMC_SET_BLOCK_COUNT 		= 23,
	MMC_WRITE_SINGLE_BLOCK 		= 24,
	MMC_WRITE_MULTIPLE_BLOCK	= 25,
	MMC_PROGRAM_CID 			= 26,
	MMC_PROGRAM_CSD 			= 27,

	/* Class 5 */
	MMC_ERASE_GROUP_START 		= 35,
	MMC_ERASE_GROUP_END			= 36,
	MMC_ERASE					= 38,

	/* Class 6 */
	MMC_SET_WRITE_PROT			= 28,
	MMC_CLR_WRITE_PROT			= 29,
	MMC_SEND_WRITE_PROT			= 30,

	/* Class 7 */
	MMC_LOCK_UNLOCK				= 42,

	/* Class 8 */
	MMC_APP_CMD					= 55,
	MMC_GEN_CMD					= 56,

	/* Class 9 */
	MMC_FAST_IO					= 39,
	MMC_GO_IRQ_STATE			= 40,
};

enum {
	SD_CMD_SEND_RELATIVE_ADDR	= 3,
	SD_CMD_SWITCH_FUNC			= 6,
	SD_CMD_SEND_IF_COND			= 8,
	SD_CMD_APP_SET_BUS_WIDTH	= 6,
	SD_CMD_ERASE_WR_BLK_START	= 32,
	SD_CMD_ERASE_WR_BLK_END		= 33,
	SD_CMD_APP_SEND_OP_COND		= 41,
	SD_CMD_APP_SEND_SCR			= 51,
};

enum {
	MMC_RSP_PRESENT	= (1 << 0),
	MMC_RSP_136		= (1 << 1),
	MMC_RSP_CRC		= (1 << 2),
	MMC_RSP_BUSY	= (1 << 3),
	MMC_RSP_OPCODE	= (1 << 4),
};

enum {
	MMC_RSP_NONE	= (0 << 24),
	MMC_RSP_R1 		= (1 << 24) | (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE),
	MMC_RSP_R1B 	= (1 << 24) | (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE | MMC_RSP_BUSY),
	MMC_RSP_R2 		= (2 << 24) | (MMC_RSP_PRESENT | MMC_RSP_136 | MMC_RSP_CRC),
	MMC_RSP_R3 		= (3 << 24) | (MMC_RSP_PRESENT),
	MMC_RSP_R4 		= (4 << 24) | (MMC_RSP_PRESENT),
	MMC_RSP_R5 		= (5 << 24) | (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE),
	MMC_RSP_R6 		= (6 << 24) | (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE),
	MMC_RSP_R7 		= (7 << 24) | (MMC_RSP_PRESENT | MMC_RSP_CRC | MMC_RSP_OPCODE),
};

enum {
	OCR_BUSY 		= 0x80000000,
	OCR_HCS 		= 0x40000000,
	OCR_VOLTAGE_MASK= 0x00ffff80,
	OCR_ACCESS_MODE	= 0x60000000,
};

enum {
	MMC_DATA_READ	= (1 << 0),
	MMC_DATA_WRITE	= (1 << 1),
};

enum {
	MMC_VDD_27_36	= (1 << 0),
	MMC_VDD_165_195	= (1 << 1),
};

enum {
	MMC_BUS_WIDTH_1 = (1 << 0),
	MMC_BUS_WIDTH_4 = (1 << 2),
	MMC_BUS_WIDTH_8 = (1 << 3),
};

enum {
	SD_VERSION_SD 		= 0x20000,
	SD_VERSION_3 		= (SD_VERSION_SD | 0x300),
	SD_VERSION_2 		= (SD_VERSION_SD | 0x200),
	SD_VERSION_1_0 		= (SD_VERSION_SD | 0x100),
	SD_VERSION_1_10 	= (SD_VERSION_SD | 0x10a),
	MMC_VERSION_MMC 	= 0x10000,
	MMC_VERSION_UNKNOWN	= (MMC_VERSION_MMC),
	MMC_VERSION_1_2 	= (MMC_VERSION_MMC | 0x102),
	MMC_VERSION_1_4 	= (MMC_VERSION_MMC | 0x104),
	MMC_VERSION_2_2 	= (MMC_VERSION_MMC | 0x202),
	MMC_VERSION_3 		= (MMC_VERSION_MMC | 0x300),
	MMC_VERSION_4 		= (MMC_VERSION_MMC | 0x400),
	MMC_VERSION_4_1 	= (MMC_VERSION_MMC | 0x401),
	MMC_VERSION_4_2 	= (MMC_VERSION_MMC | 0x402),
	MMC_VERSION_4_3 	= (MMC_VERSION_MMC | 0x403),
	MMC_VERSION_4_41 	= (MMC_VERSION_MMC | 0x429),
	MMC_VERSION_4_5 	= (MMC_VERSION_MMC | 0x405),
	MMC_VERSION_5_0 	= (MMC_VERSION_MMC | 0x500),
	MMC_VERSION_5_1 	= (MMC_VERSION_MMC | 0x501),
};

/*
  MMC status in R1, for native mode (SPI bits are different)
  Type
	e : error bit
	s : status bit
	r : detected and set for the actual command response
	x : detected and set during command execution. the host must poll
            the card by sending status command in order to read these bits.
  Clear condition
	a : according to the card state
	b : always related to the previous command. Reception of
            a valid command will clear it (with a delay of one command)
	c : clear by read
 */

#define R1_OUT_OF_RANGE		(1 << 31)	/* er, c */
#define R1_ADDRESS_ERROR	(1 << 30)	/* erx, c */
#define R1_BLOCK_LEN_ERROR	(1 << 29)	/* er, c */
#define R1_ERASE_SEQ_ERROR      (1 << 28)	/* er, c */
#define R1_ERASE_PARAM		(1 << 27)	/* ex, c */
#define R1_WP_VIOLATION		(1 << 26)	/* erx, c */
#define R1_CARD_IS_LOCKED	(1 << 25)	/* sx, a */
#define R1_LOCK_UNLOCK_FAILED	(1 << 24)	/* erx, c */
#define R1_COM_CRC_ERROR	(1 << 23)	/* er, b */
#define R1_ILLEGAL_COMMAND	(1 << 22)	/* er, b */
#define R1_CARD_ECC_FAILED	(1 << 21)	/* ex, c */
#define R1_CC_ERROR		(1 << 20)	/* erx, c */
#define R1_ERROR		(1 << 19)	/* erx, c */
#define R1_UNDERRUN		(1 << 18)	/* ex, c */
#define R1_OVERRUN		(1 << 17)	/* ex, c */
#define R1_CID_CSD_OVERWRITE	(1 << 16)	/* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP	(1 << 15)	/* sx, c */
#define R1_CARD_ECC_DISABLED	(1 << 14)	/* sx, a */
#define R1_ERASE_RESET		(1 << 13)	/* sr, c */
#define R1_STATUS(x)            (x & 0xFFFFE000)
#define R1_CURRENT_STATE(x)	((x & 0x00001E00) >> 9)	/* sx, b (4 bits) */
#define R1_READY_FOR_DATA	(1 << 8)	/* sx, a */
#define R1_SWITCH_ERROR		(1 << 7)	/* sx, c */
#define R1_EXCEPTION_EVENT	(1 << 6)	/* sr, a */
#define R1_APP_CMD		(1 << 5)	/* sr, c */

#define R1_STATE_IDLE	0
#define R1_STATE_READY	1
#define R1_STATE_IDENT	2
#define R1_STATE_STBY	3
#define R1_STATE_TRAN	4
#define R1_STATE_DATA	5
#define R1_STATE_RCV	6
#define R1_STATE_PRG	7
#define R1_STATE_DIS	8

#ifdef __cplusplus
}
#endif

#endif /* __SDMMC_H__ */
