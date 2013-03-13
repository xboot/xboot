/*
 * driver/s3c6410-mmc.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include <xboot.h>
#include <sizes.h>
#include <types.h>
#include <div64.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <mmc/mmc_host.h>
#include <s3c6410/reg-clk.h>
#include <s3c6410/reg-gpio.h>
#include <s3c6410/reg-mmc.h>

# if 0
#define MMC_RSP_PRESENT	(1 << 0)
#define MMC_RSP_136	(1 << 1)		/* 136 bit response */
#define MMC_RSP_CRC	(1 << 2)		/* expect valid crc */
#define MMC_RSP_BUSY	(1 << 3)		/* card may send busy */
#define MMC_RSP_OPCODE	(1 << 4)		/* response contains opcode */
#define MMC_CMD_MASK	(3 << 5)		/* command type */
#define MMC_CMD_AC	(0 << 5)
#define MMC_CMD_ADTC	(1 << 5)
#define MMC_CMD_BC	(2 << 5)
#define MMC_CMD_BCR	(3 << 5)

/*
 * These are the response types, and correspond to valid bit
 * patterns of the above flags.  One additional valid pattern
 * is all zeros, which means we don't expect a response.
 */
#define MMC_RSP_NONE	(0)
#define MMC_RSP_R1	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1B	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)
#define MMC_RSP_R2	(MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3	(MMC_RSP_PRESENT)
#define MMC_RSP_R6	(MMC_RSP_PRESENT|MMC_RSP_CRC)

#define mmc_resp_type(cmd)	((cmd)->flags & (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC|MMC_RSP_BUSY|MMC_RSP_OPCODE))

/*
 * These are the command types.
 */
#define mmc_cmd_type(cmd)	((cmd)->flags & MMC_CMD_MASK)

#define MMC_ERR_NONE	0
#define MMC_ERR_TIMEOUT	1
#define MMC_ERR_BADCRC	2
#define MMC_ERR_FIFO	3
#define MMC_ERR_FAILED	4
#define MMC_ERR_INVALID	5

#define	NORMAL	0
#define	HIGH	1

static u32_t ocr_check = 0;
static u32_t mmc_card = 0;
static u32_t rca = 0;
static u32_t mmc_spec, sd_spec;
static u32_t card_mid = 0;

static void set_cmd_register (u16_t cmd, u32_t data, u32_t flags)
{
	u16_t val = (cmd << 8);

	if (cmd == 12)
		val |= (3 << 6);

	if (flags & MMC_RSP_136)	/* Long RSP */
		val |= 0x01;
	else if (flags & MMC_RSP_BUSY)	/* R1B */
		val |= 0x03;
	else if (flags & MMC_RSP_PRESENT)	/* Normal RSP */
		val |= 0x02;

	if (flags & MMC_RSP_OPCODE)
		val |= (1<<4);

	if (flags & MMC_RSP_CRC)
		val |= (1<<3);

	if (data)
		val |= (1<<5);

	printk("cmdreg =  0x%04x\r\n", val);
	writew(S3C6410_HM_CMDREG1, val);
}

static int wait_for_cmd_done (void)
{
	u32_t i;
	u16_t n_int, e_int;

	udelay(5000);

	printk("wait_for_cmd_done\r\n");
	for (i = 0; i < 0x20000000; i++) {
		n_int = readw(S3C6410_HM_NORINTSTS1);
		printk("  HM_NORINTSTS: 0x%04x\r\n", n_int);
		if (n_int & 0x8000) break;
		if (n_int & 0x0001) return 0;
	}

	e_int = readw(S3C6410_HM_ERRINTSTS1);
	writew(S3C6410_HM_ERRINTSTS1,e_int);
	writew(S3C6410_HM_NORINTSTS1, n_int);
	printk("cmd error1: 0x%04x, HM_NORINTSTS: 0x%04x\r\n", e_int, n_int);
	return 1;
}

static void ClearErrInterruptStatus(void)
{
	while (readw(S3C6410_HM_NORINTSTS1) & (0x1 << 15))
	{
		writew(S3C6410_HM_NORINTSTS1, readw(S3C6410_HM_NORINTSTS1));
		writew(S3C6410_HM_ERRINTSTS1, readw(S3C6410_HM_ERRINTSTS1));
	}
}

static void InterruptEnable(u16_t NormalIntEn, u16_t ErrorIntEn)
{
	ClearErrInterruptStatus();
	writew(S3C6410_HM_NORINTSTSEN1, NormalIntEn);
	writew(S3C6410_HM_ERRINTSTSEN1, ErrorIntEn);
}

static void set_hostctl_speed (u8_t mode)
{
	u8_t reg8;

	reg8 = readb(S3C6410_HM_HOSTCTL1) & ~(0x1<<2);
	writeb(S3C6410_HM_HOSTCTL1, reg8 | (mode<<2));
}

static void hsmmc_clock_onoff(bool_t on)
{
	if(on)
	{
		writew(S3C6410_HM_CLKCON1, (readw(S3C6410_HM_CLKCON1) & ~(0x1<<2)) | (0x1<<2));
	}
	else
	{
		writew(S3C6410_HM_CLKCON1, (readw(S3C6410_HM_CLKCON1) & ~(0x1<<2)) | (0x0<<2));
	}
}

static void set_clock (u32_t clksrc, u32_t div)
{
	s32_t i;

	writel(S3C6410_HM_CONTROL2_1, (0xC0004100 | (clksrc << 4)));	// rx feedback control
	writel(S3C6410_HM_CONTROL3_1, 0x00008080); 			// Low clock: 00008080
	writel(S3C6410_HM_CONTROL4_1, (0x3 << 16));

	writew(S3C6410_HM_CLKCON1, (readw(S3C6410_HM_CLKCON1) & ~(0xff << 8)) );

	/* SDCLK Value Setting + Internal Clock Enable */
	writew(S3C6410_HM_CLKCON1, ((div<<8) | 0x1));

	/* CheckInternalClockStable */
	for (i=0; i<0x10000; i++)
	{
		if (readw(S3C6410_HM_CLKCON1) & 0x2)
			break;
	}
	if (i == 0x10000)
		printk("internal clock stabilization failed\r\n");

	printk("HM_CONTROL2(0x80) = 0x%08lx\r\n", readl(S3C6410_HM_CONTROL2_1));
	printk("HM_CONTROL3(0x84) = 0x%08lx\r\n", readl(S3C6410_HM_CONTROL3_1));
	printk("HM_CLKCON  (0x2c) = 0x%04lx\r\n", readw(S3C6410_HM_CLKCON1));

	hsmmc_clock_onoff(TRUE);
}

static void ClearCommandCompleteStatus(void)
{
	writew(S3C6410_HM_NORINTSTS1, 1 << 0);
	while (readw(S3C6410_HM_NORINTSTS1) & 0x1) {
		writew(S3C6410_HM_NORINTSTS1, 1 << 0);
	}
}

static int issue_command (u16_t cmd, u32_t arg, u32_t data, u32_t flags)
{
	int i;

	printk("### issue_command: %d, %08x, %d, %08x\r\n", cmd, arg, data, flags);
	/* Check CommandInhibit_CMD */
	for (i=0; i<0x1000000; i++) {
		if (!(readl(S3C6410_HM_PRNSTS1) & 0x1))
			break;
	}
	if (i == 0x1000000) {
		printk("@@@@@@1 rHM_PRNSTS: %08x\r\n", readl(S3C6410_HM_PRNSTS1));
	}

	/* Check CommandInhibit_DAT */
	if (flags & MMC_RSP_BUSY) {
		for (i=0; i<0x1000000; i++) {
			if (!(readl(S3C6410_HM_PRNSTS1) & 0x2))
				break;
		}
		if (i == 0x1000000) {
			printk("@@@@@@2 rHM_PRNSTS: %08x\n", readl(S3C6410_HM_PRNSTS1));
		}
	}

	writel(S3C6410_HM_ARGUMENT1, arg);

	set_cmd_register(cmd, data, flags);

	if (wait_for_cmd_done())
		return 0;

	ClearCommandCompleteStatus();

	if (!(readw(S3C6410_HM_NORINTSTS1) & 0x8000)) {
		return 1;
	} else {
		if (ocr_check == 1)
			return 0;
		else {
			printk("Command = %d, Error Stat = 0x%04x\r\n", (readw(S3C6410_HM_CMDREG1) >> 8), readw(S3C6410_HM_ERRINTSTS1));
			return 0;
		}
	}

}

static int set_mmc_ocr (void)
{
	u32_t i, ocr;

	for (i = 0; i < 100; i++) {
		issue_command(MMC_SEND_OP_COND, 0x40FF8000, 0, MMC_RSP_R3);

		ocr = readl(S3C6410_HM_RSPREG0_1);
		printk("ocr1: %08x\r\n", ocr);

		if (ocr & (0x1 << 31)) {
			printk("Voltage range: ");
			if (ocr & (1 << 21))
				printk("2.7V ~ 3.4V");
			else if (ocr & (1 << 20))
				printk("2.7V ~ 3.3V");
			else if (ocr & (1 << 19))
				printk("2.7V ~ 3.2V");
			else if (ocr & (1 << 18))
				printk("2.7V ~ 3.1V");
			mmc_card = 1;
			if (ocr & (1 << 7))
				printk(", 1.65V ~ 1.95V\n");
			else
				printk("\r\n");
			return 1;
		}
	}

	// The current card is SD card, then there's time out error, need to be cleared.
	ClearErrInterruptStatus();
	return 0;
}

static int set_sd_ocr (void)
{
	u32_t i, ocr;

	issue_command(MMC_APP_CMD, 0x0, 0, MMC_RSP_R1);
	issue_command(SD_APP_OP_COND, 0x0, 0, MMC_RSP_R3);
	ocr = readl(S3C6410_HM_RSPREG0_1);
	printk("ocr1: %08x\n", ocr);

	for (i = 0; i < 100; i++) {
		issue_command(MMC_APP_CMD, 0x0, 0, MMC_RSP_R1);
		issue_command(SD_APP_OP_COND, ocr, 0, MMC_RSP_R3);

		ocr = readl(S3C6410_HM_RSPREG0_1);
		printk("ocr2: %08x\n", ocr);
		if (ocr & (0x1 << 31)) {
			printk("Voltage range: ");
			if (ocr & (1 << 21))
				printk("2.7V ~ 3.4V");
			else if (ocr & (1 << 20))
				printk("2.7V ~ 3.3V");
			else if (ocr & (1 << 19))
				printk("2.7V ~ 3.2V");
			else if (ocr & (1 << 18))
				printk("2.7V ~ 3.1V");

			if (ocr & (1 << 7))
				printk(", 1.65V ~ 1.95V\n");
			else
				printk("\r\n");

			mmc_card = 0;
			return 1;
		}
		udelay(1000);
	}

	// The current card is MMC card, then there's time out error, need to be cleared.
	ClearErrInterruptStatus();
	return 0;
}

static void display_card_info (void)
{
	u32_t card_size;

	if (0) {
		//card_size = process_ext_csd();
	} else {
		u32_t i, resp[4];
		u32_t c_size, c_size_multi, read_bl_len, read_bl_partial, blk_size;

		for (i=0; i<4; i++) {
			resp[i] = readl(S3C6410_HM_RSPREG0_1 + i*4);
			printk("%08x\r\n", resp[i]);
		}

		read_bl_len = ((resp[2] >> 8) & 0xf);
		read_bl_partial = ((resp[2] >> 7) & 0x1);
		blk_size = (1 << read_bl_len);

		printk(" read_bl_len: %d\r\n", read_bl_len);
		printk(" read_bl_partial: %d\r\n", read_bl_partial);
		printk(" One Block Size: %d Byte\r\n", blk_size);

		if (0) {
			card_size = ((resp[1] >> 8) & 0x3fffff) / 2;
		} else {
			c_size_multi = ((resp[1] >> 7) & 0x7);
			c_size = ((resp[2] & 0x3) << 10) | ((resp[1] >> 22) & 0x3ff);
			card_size = (1 << read_bl_len) * (c_size + 1) * (1 << (c_size_multi + 2)) / 1048576;

			printk(" c_size: %d\r\n", c_size);
			printk(" c_size_multi: %d\r\n", c_size_multi);
		}

		printk(" Total Card Size: %d MByte\r\n", card_size + 1);
	}

	printk("%d MB ", card_size + 1);

	if (!mmc_card)
		printk("(SDHC)");

	if (card_mid == 0x15)
		printk("(moviNAND)");

	printk("\r\n");
}

static void mmc_init(void)
{
	u64_t hclk;

	/* ch 1 */
	writel(S3C6410_GPHCON0, (readl(S3C6410_GPHCON0) & ~(0x00ffffff<<0)) | (0x00222222<<0));
	writel(S3C6410_GPHCON1, (readl(S3C6410_GPHCON1)));
	writel(S3C6410_GPHPUD, (readl(S3C6410_GPHPUD) & ~(0xfff<<0)) | (0x000<<0));

	/* reset ch 1 */
	writeb(S3C6410_HM_SWRST1, 0x3);

	clk_get_rate("hclk", &hclk);

	hsmmc_clock_onoff(FALSE);

	//sclk_mmc0_48
	writel(S3C6410_SCLK_GATE, (readl(S3C6410_SCLK_GATE) & ~(1<<27)) | (0x1<<27));

	set_clock(2, 0x80);

	writeb(S3C6410_HM_TIMEOUTCON1, 0xe);

	set_hostctl_speed(NORMAL);

	InterruptEnable(0xff, 0xff);

	printk("HM_NORINTSTS = 0x%lx\r\n", readw(S3C6410_HM_NORINTSTS1));

	/* MMC_GO_IDLE_STATE */
	issue_command(MMC_GO_IDLE_STATE, 0x00, 0, 0);

	ocr_check = 1;

	if (set_mmc_ocr()) {
		mmc_card = 1;
		printk("MMC card is detected\r\n");
	} else if (set_sd_ocr()) {
		mmc_card = 0;
		printk("SD card is detected\r\n");
	} else {
		printk("0 MB\r\n");
		return;
	}

	ocr_check = 0;

	/* Check the attached card and place the card
	 * in the IDENT state rHM_RSPREG0
	 */
	issue_command(MMC_ALL_SEND_CID, 0, 0, MMC_RSP_R2);

	/* Manufacturer ID */
	card_mid = (readl(S3C6410_HM_RSPREG3_1) >> 16) & 0xFF;

	printk("Product Name : %c%c%c%c%c%c\r\n", ((readl(S3C6410_HM_RSPREG2_1) >> 24) & 0xFF),
	       ((readl(S3C6410_HM_RSPREG2_1) >> 16) & 0xFF), ((readl(S3C6410_HM_RSPREG2_1) >> 8) & 0xFF), (readl(S3C6410_HM_RSPREG2_1) & 0xFF),
	       ((readl(S3C6410_HM_RSPREG1_1) >> 24) & 0xFF), ((readl(S3C6410_HM_RSPREG1_1) >> 16) & 0xFF));

	// Send RCA(Relative Card Address). It places the card in the STBY state
	rca = (mmc_card) ? 0x0001 : 0x0000;
	issue_command(MMC_SET_RELATIVE_ADDR, rca<<16, 0, MMC_RSP_R1);

	if (!mmc_card)
		rca = (readl(S3C6410_HM_RSPREG0_1) >> 16) & 0xFFFF;

	printk("Enter to the Stand-by State\r\n");

	issue_command(MMC_SEND_CSD, rca<<16, 0, MMC_RSP_R2);

	if (mmc_card) {
		mmc_spec = (readl(S3C6410_HM_RSPREG3_1) >> 18) & 0xF;
		printk("mmc_spec=%d\r\n", mmc_spec);
	}

	issue_command(MMC_SELECT_CARD, rca<<16, 0, MMC_RSP_R1);
	printk("Enter to the Transfer State\r\n");

	display_card_info();
}

#endif

static __init void s3c6410_mmc_init(void)
{
//	mmc_init();

}

static __exit void s3c6410_mmc_exit(void)
{

}

device_initcall(s3c6410_mmc_init);
device_exitcall(s3c6410_mmc_exit);
