#ifndef __MMC_H__
#define __MMC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <string.h>
#include <xboot/list.h>

/*
 * standard mmc commands
 */

/* class 1 */
#define MMC_GO_IDLE_STATE			0	/* bc                          */
#define MMC_SEND_OP_COND			1	/* bcr  [31:0] OCR         R3  */
#define MMC_ALL_SEND_CID			2	/* bcr                     R2  */
#define MMC_SET_RELATIVE_ADDR		3	/* ac   [31:16] RCA        R1  */
#define MMC_SET_DSR					4	/* bc   [31:16] RCA            */
#define MMC_SWITCH					6	/* ac   [31:0] See below   R1b */
#define MMC_SELECT_CARD				7	/* ac   [31:16] RCA        R1  */
#define MMC_SEND_EXT_CSD			8	/* adtc                    R1  */
#define MMC_SEND_CSD				9	/* ac   [31:16] RCA        R2  */
#define MMC_SEND_CID				10	/* ac   [31:16] RCA        R2  */
#define MMC_READ_DAT_UNTIL_STOP		11	/* adtc [31:0] dadr        R1  */
#define MMC_STOP_TRANSMISSION		12	/* ac                      R1b */
#define MMC_SEND_STATUS				13	/* ac   [31:16] RCA        R1  */
#define MMC_GO_INACTIVE_STATE		15	/* ac   [31:16] RCA            */
#define MMC_SPI_READ_OCR			58	/* spi                  spi_R3 */
#define MMC_SPI_CRC_ON_OFF			59	/* spi  [0:0] flag      spi_R1 */

/* class 2 */
#define MMC_SET_BLOCKLEN			16	/* ac   [31:0] block len   R1  */
#define MMC_READ_SINGLE_BLOCK		17	/* adtc [31:0] data addr   R1  */
#define MMC_READ_MULTIPLE_BLOCK		18	/* adtc [31:0] data addr   R1  */

/* class 3 */
#define MMC_WRITE_DAT_UNTIL_STOP 	20	/* adtc [31:0] data addr   R1  */

/* class 4 */
#define MMC_SET_BLOCK_COUNT			23	/* adtc [31:0] data addr   R1  */
#define MMC_WRITE_SINGLE_BLOCK		24	/* adtc [31:0] data addr   R1  */
#define MMC_WRITE_MULTIPLE_BLOCK	25	/* adtc                    R1  */
#define MMC_PROGRAM_CID				26	/* adtc                    R1  */
#define MMC_PROGRAM_CSD				27	/* adtc                    R1  */

/* class 5 */
#define MMC_ERASE_GROUP_START		35	/* ac   [31:0] data addr   R1  */
#define MMC_ERASE_GROUP_END			36	/* ac   [31:0] data addr   R1  */
#define MMC_ERASE					38	/* ac                      R1b */

/* class 6 */
#define MMC_SET_WRITE_PROT			28	/* ac   [31:0] data addr   R1b */
#define MMC_CLR_WRITE_PROT			29	/* ac   [31:0] data addr   R1b */
#define MMC_SEND_WRITE_PROT			30	/* adtc [31:0] wpdata addr R1  */

/* class 7 */
#define MMC_LOCK_UNLOCK				42	/* adtc                    R1b */

/* class 8 */
#define MMC_APP_CMD					55	/* ac   [31:16] RCA        R1  */
#define MMC_GEN_CMD					56	/* adtc [0] RD/WR          R1  */

/* class 9 */
#define MMC_FAST_IO					39	/* ac   <Complex>          R4  */
#define MMC_GO_IRQ_STATE			40	/* bcr                     R5  */


/*
 * standard sd commands
 */
#define SD_SEND_RELATIVE_ADDR		3
#define SD_SWITCH_FUNC				6
#define SD_SEND_IF_COND				8

#define SD_APP_SET_BUS_WIDTH		6
#define SD_APP_SEND_OP_COND			41
#define SD_APP_SEND_SCR				51

/*
 * define mmc's voltage
 */
#define MMC_VDD_165_195				0x00000080	/* vdd voltage 1.65 - 1.95 */
#define MMC_VDD_20_21				0x00000100	/* vdd voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22				0x00000200	/* vdd voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23				0x00000400	/* vdd voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24				0x00000800	/* vdd voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25				0x00001000	/* vdd voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26				0x00002000	/* vdd voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27				0x00004000	/* vdd voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28				0x00008000	/* vdd voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29				0x00010000	/* vdd voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30				0x00020000	/* vdd voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31				0x00040000	/* vdd voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32				0x00080000	/* vdd voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33				0x00100000	/* vdd voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34				0x00200000	/* vdd voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35				0x00400000	/* vdd voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36				0x00800000	/* vdd voltage 3.5 ~ 3.6 */

#ifdef __cplusplus
}
#endif

#endif /* __MMC_H__ */
