/*
 * sys-sdhci.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <sd/sdmmc.h>

enum {
	SD_GCTL			= 0x00,
	SD_CKCR			= 0x04,
	SD_TMOR			= 0x08,
	SD_BWDR			= 0x0c,
	SD_BKSR			= 0x10,
	SD_BYCR			= 0x14,
	SD_CMDR			= 0x18,
	SD_CAGR			= 0x1c,
	SD_RESP0		= 0x20,
	SD_RESP1		= 0x24,
	SD_RESP2		= 0x28,
	SD_RESP3		= 0x2c,
	SD_IMKR			= 0x30,
	SD_MISR			= 0x34,
	SD_RISR			= 0x38,
	SD_STAR			= 0x3c,
	SD_FWLR			= 0x40,
	SD_FUNS			= 0x44,
	SD_A12A			= 0x58,
	SD_NTSR			= 0x5c,
	SD_SDBG			= 0x60,
	SD_HWRST		= 0x78,
	SD_DMAC			= 0x80,
	SD_DLBA			= 0x84,
	SD_IDST			= 0x88,
	SD_IDIE			= 0x8c,
	SD_THLDC		= 0x100,
	SD_DSBD			= 0x10c,
	SD_RES_CRC		= 0x110,
	SD_DATA7_CRC	= 0x114,
	SD_DATA6_CRC	= 0x118,
	SD_DATA5_CRC	= 0x11c,
	SD_DATA4_CRC	= 0x120,
	SD_DATA3_CRC	= 0x124,
	SD_DATA2_CRC	= 0x128,
	SD_DATA1_CRC	= 0x12c,
	SD_DATA0_CRC	= 0x130,
	SD_CRC_STA		= 0x134,
	SD_FIFO			= 0x200,
};

/*
 * Global control register bits
 */
#define SDXC_SOFT_RESET				(1 << 0)
#define SDXC_FIFO_RESET				(1 << 1)
#define SDXC_DMA_RESET				(1 << 2)
#define SDXC_INTERRUPT_ENABLE_BIT	(1 << 4)
#define SDXC_DMA_ENABLE_BIT			(1 << 5)
#define SDXC_DEBOUNCE_ENABLE_BIT	(1 << 8)
#define SDXC_POSEDGE_LATCH_DATA		(1 << 9)
#define SDXC_DDR_MODE				(1 << 10)
#define SDXC_MEMORY_ACCESS_DONE		(1 << 29)
#define SDXC_ACCESS_DONE_DIRECT		(1 << 30)
#define SDXC_ACCESS_BY_AHB			(1 << 31)
#define SDXC_ACCESS_BY_DMA			(0 << 31)
#define SDXC_HARDWARE_RESET			(SDXC_SOFT_RESET | SDXC_FIFO_RESET | SDXC_DMA_RESET)

/*
 * Bus width
 */
#define SDXC_WIDTH1					(0)
#define SDXC_WIDTH4					(1)
#define SDXC_WIDTH8					(2)

/*
 * Smc command bits
 */
#define SDXC_RESP_EXPIRE			(1 << 6)
#define SDXC_LONG_RESPONSE			(1 << 7)
#define SDXC_CHECK_RESPONSE_CRC		(1 << 8)
#define SDXC_DATA_EXPIRE			(1 << 9)
#define SDXC_WRITE					(1 << 10)
#define SDXC_SEQUENCE_MODE			(1 << 11)
#define SDXC_SEND_AUTO_STOP			(1 << 12)
#define SDXC_WAIT_PRE_OVER			(1 << 13)
#define SDXC_STOP_ABORT_CMD			(1 << 14)
#define SDXC_SEND_INIT_SEQUENCE		(1 << 15)
#define SDXC_UPCLK_ONLY				(1 << 21)
#define SDXC_READ_CEATA_DEV			(1 << 22)
#define SDXC_CCS_EXPIRE				(1 << 23)
#define SDXC_ENABLE_BIT_BOOT		(1 << 24)
#define SDXC_ALT_BOOT_OPTIONS		(1 << 25)
#define SDXC_BOOT_ACK_EXPIRE		(1 << 26)
#define SDXC_BOOT_ABORT				(1 << 27)
#define SDXC_VOLTAGE_SWITCH			(1 << 28)
#define SDXC_USE_HOLD_REGISTER		(1 << 29)
#define SDXC_START					(1 << 31)

/*
 * Interrupt bits
 */
#define SDXC_RESP_ERROR				(1 << 1)
#define SDXC_COMMAND_DONE			(1 << 2)
#define SDXC_DATA_OVER				(1 << 3)
#define SDXC_TX_DATA_REQUEST		(1 << 4)
#define SDXC_RX_DATA_REQUEST		(1 << 5)
#define SDXC_RESP_CRC_ERROR			(1 << 6)
#define SDXC_DATA_CRC_ERROR			(1 << 7)
#define SDXC_RESP_TIMEOUT			(1 << 8)
#define SDXC_DATA_TIMEOUT			(1 << 9)
#define SDXC_VOLTAGE_CHANGE_DONE	(1 << 10)
#define SDXC_FIFO_RUN_ERROR			(1 << 11)
#define SDXC_HARD_WARE_LOCKED		(1 << 12)
#define SDXC_START_BIT_ERROR		(1 << 13)
#define SDXC_AUTO_COMMAND_DONE		(1 << 14)
#define SDXC_END_BIT_ERROR			(1 << 15)
#define SDXC_SDIO_INTERRUPT			(1 << 16)
#define SDXC_CARD_INSERT			(1 << 30)
#define SDXC_CARD_REMOVE			(1 << 31)
#define SDXC_INTERRUPT_ERROR_BIT	(SDXC_RESP_ERROR | SDXC_RESP_CRC_ERROR | SDXC_DATA_CRC_ERROR | SDXC_RESP_TIMEOUT | SDXC_DATA_TIMEOUT | SDXC_FIFO_RUN_ERROR | SDXC_HARD_WARE_LOCKED | SDXC_START_BIT_ERROR | SDXC_END_BIT_ERROR)
#define SDXC_INTERRUPT_DONE_BIT		(SDXC_AUTO_COMMAND_DONE | SDXC_DATA_OVER | SDXC_COMMAND_DONE | SDXC_VOLTAGE_CHANGE_DONE)

/*
 * Status
 */
#define SDXC_RXWL_FLAG				(1 << 0)
#define SDXC_TXWL_FLAG				(1 << 1)
#define SDXC_FIFO_EMPTY				(1 << 2)
#define SDXC_FIFO_FULL				(1 << 3)
#define SDXC_CARD_PRESENT			(1 << 8)
#define SDXC_CARD_DATA_BUSY			(1 << 9)
#define SDXC_DATA_FSM_BUSY			(1 << 10)
#define SDXC_DMA_REQUEST			(1 << 31)
#define SDXC_FIFO_SIZE				(16)

struct sdhci_cmd_t {
	uint32_t cmdidx;
	uint32_t cmdarg;
	uint32_t resptype;
	uint32_t response[4];
};

struct sdhci_data_t {
	uint8_t * buf;
	uint32_t flag;
	uint32_t blksz;
	uint32_t blkcnt;
};

void sys_sdhci_init(void)
{
	virtual_addr_t addr;
	uint32_t val;

	/* Config GPIOF0, GPIOF1, GPIOF2, GPIOF3, GPIOF4, GPIOF5 */
	addr = 0x020000f0 + 0x00;
	val = read32(addr);
	val &= ~(0xf << ((0 & 0x7) << 2));
	val |= ((0x2 & 0xf) << ((0 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((1 & 0x7) << 2));
	val |= ((0x2 & 0xf) << ((1 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((2 & 0x7) << 2));
	val |= ((0x2 & 0xf) << ((2 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((3 & 0x7) << 2));
	val |= ((0x2 & 0xf) << ((3 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((4 & 0x7) << 2));
	val |= ((0x2 & 0xf) << ((4 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((5 & 0x7) << 2));
	val |= ((0x2 & 0xf) << ((5 & 0x7) << 2));
	write32(addr, val);

	/* Deassert sdhci0 reset */
	addr = 0x0200184c;
	val = read32(addr);
	val |= (1 << 16);
	write32(addr, val);

	/* Open the sdhci0 bus gate */
	addr = 0x0200184c;
	val = read32(addr);
	val |= (1 << 0);
	write32(addr, val);

	/* Open the sdhci0 gate */
	addr = 0x02001830;
	val = read32(addr);
	val |= (1 << 31);
	write32(addr, val);

	/* Select pll-periph0 for sdhci0 clk */
	addr = 0x02001830;
	val = read32(addr);
	val &= ~(0x3 << 24);
	val |= 0x1 << 24;
	write32(addr, val);

	/* Set clock divide, divided by 6 */
	addr = 0x02001830;
	val = read32(addr);
	val &= ~(0xf << 0);
	val |= (6 - 1) << 0;
	write32(addr, val);

	/* Set clock ratio, divided by 1 */
	addr = 0x02001830;
	val = read32(addr);
	val &= ~(0x3 << 8);
	val |= 0x0 << 8;
	write32(addr, val);
}

void sys_sdhci_reset(void)
{
	virtual_addr_t addr = 0x04020000;
	write32(addr + SD_GCTL, SDXC_HARDWARE_RESET);
}

static void sys_sdhci_update_clock(void)
{
	virtual_addr_t addr = 0x04020000;
	uint32_t cmd = (1U << 31) | (1 << 21) | (1 << 13);
	int timeout = 100000;

	write32(addr + SD_CMDR, cmd);
	while((read32(addr + SD_CMDR) & 0x80000000) && timeout--);
	if(!timeout)
		return;
	write32(addr + SD_RISR, read32(addr + SD_RISR));
}

void sys_sdhci_setclock(uint32_t clock)
{
	virtual_addr_t addr = 0x04020000;
	uint32_t ratio = (100 * 1000 * 1000 + 2 * clock - 1) / (2 * clock);

	if((ratio & 0xff) != ratio)
		return;
	write32(addr + SD_CKCR, read32(addr + SD_CKCR) & ~(1 << 16));
	write32(addr + SD_CKCR, ratio);
	sys_sdhci_update_clock();
	write32(addr + SD_CKCR, read32(addr + SD_CKCR) | (3 << 16));
	sys_sdhci_update_clock();
}

void sys_sdhci_setwidth(uint32_t width)
{
	virtual_addr_t addr = 0x04020000;

	if(width == MMC_BUS_WIDTH_1)
		write32(addr + SD_BWDR, SDXC_WIDTH1);
	else if(width == MMC_BUS_WIDTH_4)
		write32(addr + SD_BWDR, SDXC_WIDTH4);
	else if(width == MMC_BUS_WIDTH_8)
		write32(addr + SD_BWDR, SDXC_WIDTH8);
}

int sys_sdhci_transfer_command(struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	virtual_addr_t addr = 0x04020000;
	uint32_t cmdval = SDXC_START;
	uint32_t status = 0;
	int timeout = 0;

	if(cmd->cmdidx == MMC_STOP_TRANSMISSION)
	{
		timeout = 100000;
		do {
			status = read32(addr + SD_STAR);
			if(!timeout--)
			{
				write32(addr + SD_GCTL, SDXC_HARDWARE_RESET);
				write32(addr + SD_RISR, 0xffffffff);
				return 0;
			}
		} while(status & SDXC_CARD_DATA_BUSY);
		return 1;
	}

	if(cmd->cmdidx == MMC_GO_IDLE_STATE)
		cmdval |= SDXC_SEND_INIT_SEQUENCE;
	if(cmd->resptype & MMC_RSP_PRESENT)
	{
		cmdval |= SDXC_RESP_EXPIRE;
		if(cmd->resptype & MMC_RSP_136)
			cmdval |= SDXC_LONG_RESPONSE;
		if(cmd->resptype & MMC_RSP_CRC)
			cmdval |= SDXC_CHECK_RESPONSE_CRC;
	}

	if(dat)
		cmdval |= SDXC_DATA_EXPIRE | SDXC_WAIT_PRE_OVER;

	if(dat && (dat->flag & MMC_DATA_WRITE))
		cmdval |= SDXC_WRITE;

	if(cmd->cmdidx == MMC_WRITE_MULTIPLE_BLOCK || cmd->cmdidx == MMC_READ_MULTIPLE_BLOCK)
		cmdval |= SDXC_SEND_AUTO_STOP;

	write32(addr + SD_CAGR, cmd->cmdarg);

	if(dat)
		write32(addr + SD_GCTL, read32(addr + SD_GCTL) | 0x80000000);
	write32(addr + SD_CMDR, cmdval | cmd->cmdidx);

	timeout = 100000;
	do {
		status = read32(addr + SD_RISR);
		if(!timeout-- || (status & SDXC_INTERRUPT_ERROR_BIT))
		{
			write32(addr + SD_GCTL, SDXC_HARDWARE_RESET);
			write32(addr + SD_RISR, 0xffffffff);
			return 0;
		}
	} while(!(status & SDXC_COMMAND_DONE));

	if(cmd->resptype & MMC_RSP_BUSY)
	{
		timeout = 100000;
		do {
			status = read32(addr + SD_STAR);
			if(!timeout--)
			{
				write32(addr + SD_GCTL, SDXC_HARDWARE_RESET);
				write32(addr + SD_RISR, 0xffffffff);
				return 0;
			}
		} while(status & (1 << 9));
	}

	if(cmd->resptype & MMC_RSP_136)
	{
		cmd->response[0] = read32(addr + SD_RESP3);
		cmd->response[1] = read32(addr + SD_RESP2);
		cmd->response[2] = read32(addr + SD_RESP1);
		cmd->response[3] = read32(addr + SD_RESP0);
	}
	else
	{
		cmd->response[0] = read32(addr + SD_RESP0);
	}
	write32(addr + SD_RISR, 0xffffffff);
	return 1;
}

static int read_bytes(virtual_addr_t addr, uint32_t * buf, uint32_t blkcount, uint32_t blksize)
{
	uint64_t count = blkcount * blksize;
	uint32_t * tmp = buf;
	uint32_t status, err, done;

	status = read32(addr + SD_STAR);
	err = read32(addr + SD_RISR) & SDXC_INTERRUPT_ERROR_BIT;
	while((!err) && (count >= sizeof(uint32_t)))
	{
		if(!(status & SDXC_FIFO_EMPTY))
		{
			*(tmp) = read32(addr + SD_FIFO);
			tmp++;
			count -= sizeof(uint32_t);
		}
		status = read32(addr + SD_STAR);
		err = read32(addr + SD_RISR) & SDXC_INTERRUPT_ERROR_BIT;
	}

	do {
		status = read32(addr + SD_RISR);
		err = status & SDXC_INTERRUPT_ERROR_BIT;
		if(blkcount > 1)
			done = status & SDXC_AUTO_COMMAND_DONE;
		else
			done = status & SDXC_DATA_OVER;
	} while(!done && !err);

	if(err & SDXC_INTERRUPT_ERROR_BIT)
		return 0;
	write32(addr + SD_RISR, 0xffffffff);

	if(count)
		return 0;
	return 1;
}

int sys_sdhci_transfer_data(struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	virtual_addr_t addr = 0x04020000;
	uint32_t dlen = (u32_t)(dat->blkcnt * dat->blksz);
	bool_t ret = 0;

	write32(addr + SD_BKSR, dat->blksz);
	write32(addr + SD_BYCR, dlen);
	if(dat->flag & MMC_DATA_READ)
	{
		if(!sys_sdhci_transfer_command(cmd, dat))
			return 0;
		ret = read_bytes(addr, (u32_t *)dat->buf, dat->blkcnt, dat->blksz);
	}
	return ret;
}
