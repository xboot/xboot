/*
 * driver/sdhci-t113.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <reset/reset.h>
#include <sd/sdhci.h>

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
 * Clock control bits
 */
#define SDXC_CARD_CLOCK_ON			(1 << 16)
#define SDXC_LOW_POWER_ON			(1 << 17)

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

/*
 * Function select
 */
#define SDXC_CEATA_ON				(0xceaa << 16)
#define SDXC_SEND_IRQ_RESPONSE		(1 << 0)
#define SDXC_SDIO_READ_WAIT			(1 << 1)
#define SDXC_ABORT_READ_DATA		(1 << 2)
#define SDXC_SEND_CCSD				(1 << 8)
#define SDXC_SEND_AUTO_STOPCCSD		(1 << 9)
#define SDXC_CEATA_DEV_IRQ_ENABLE	(1 << 10)

struct sdhci_t113_pdata_t
{
	virtual_addr_t virt;
	char * pclk;
	int reset;
	int clk;
	int clkcfg;
	int cmd;
	int cmdcfg;
	int dat0;
	int dat0cfg;
	int dat1;
	int dat1cfg;
	int dat2;
	int dat2cfg;
	int dat3;
	int dat3cfg;
	int dat4;
	int dat4cfg;
	int dat5;
	int dat5cfg;
	int dat6;
	int dat6cfg;
	int dat7;
	int dat7cfg;
	int cd;
	int cdcfg;
};

static bool_t t113_transfer_command(struct sdhci_t113_pdata_t * pdat, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	u32_t cmdval = SDXC_START;
	u32_t status = 0;
	ktime_t timeout;

	if(cmd->cmdidx == MMC_STOP_TRANSMISSION)
	{
		timeout = ktime_add_ms(ktime_get(), 1);
		do {
			status = read32(pdat->virt + SD_STAR);
			if(ktime_after(ktime_get(), timeout))
			{
				write32(pdat->virt + SD_GCTL, SDXC_HARDWARE_RESET);
				write32(pdat->virt + SD_RISR, 0xffffffff);
				return FALSE;
			}
		} while(status & SDXC_CARD_DATA_BUSY);
		return TRUE;
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

	write32(pdat->virt + SD_CAGR, cmd->cmdarg);

	if(dat)
		write32(pdat->virt + SD_GCTL, read32(pdat->virt + SD_GCTL) | 0x80000000);
	write32(pdat->virt + SD_CMDR, cmdval | cmd->cmdidx);

	timeout = ktime_add_ms(ktime_get(), 1);
	do {
		status = read32(pdat->virt + SD_RISR);
		if(ktime_after(ktime_get(), timeout) || (status & SDXC_INTERRUPT_ERROR_BIT))
		{
			write32(pdat->virt + SD_GCTL, SDXC_HARDWARE_RESET);
			write32(pdat->virt + SD_RISR, 0xffffffff);
			return FALSE;
		}
	} while(!(status & SDXC_COMMAND_DONE));

	if(cmd->resptype & MMC_RSP_BUSY)
	{
		timeout = ktime_add_ms(ktime_get(), 1);
		do {
			status = read32(pdat->virt + SD_STAR);
			if(ktime_after(ktime_get(), timeout))
			{
				write32(pdat->virt + SD_GCTL, SDXC_HARDWARE_RESET);
				write32(pdat->virt + SD_RISR, 0xffffffff);
				return FALSE;
			}
		} while(status & (1 << 9));
	}

	if(cmd->resptype & MMC_RSP_136)
	{
		cmd->response[0] = read32(pdat->virt + SD_RESP3);
		cmd->response[1] = read32(pdat->virt + SD_RESP2);
		cmd->response[2] = read32(pdat->virt + SD_RESP1);
		cmd->response[3] = read32(pdat->virt + SD_RESP0);
	}
	else
	{
		cmd->response[0] = read32(pdat->virt + SD_RESP0);
	}
	write32(pdat->virt + SD_RISR, 0xffffffff);
	return TRUE;
}

static bool_t read_bytes(struct sdhci_t113_pdata_t * pdat, u32_t * buf, u32_t blkcount, u32_t blksize)
{
	u64_t count = blkcount * blksize;
	u32_t * tmp = buf;
	u32_t status, err, done;

	status = read32(pdat->virt + SD_STAR);
	err = read32(pdat->virt + SD_RISR) & SDXC_INTERRUPT_ERROR_BIT;
	while((!err) && (count >= sizeof(u32_t)))
	{
		if(!(status & SDXC_FIFO_EMPTY))
		{
			*(tmp) = read32(pdat->virt + SD_FIFO);
			tmp++;
			count -= sizeof(u32_t);
		}
		status = read32(pdat->virt + SD_STAR);
		err = read32(pdat->virt + SD_RISR) & SDXC_INTERRUPT_ERROR_BIT;
	}

	do {
		status = read32(pdat->virt + SD_RISR);
		err = status & SDXC_INTERRUPT_ERROR_BIT;
		if(blkcount > 1)
			done = status & SDXC_AUTO_COMMAND_DONE;
		else
			done = status & SDXC_DATA_OVER;
	} while(!done && !err);

	if(err & SDXC_INTERRUPT_ERROR_BIT)
		return FALSE;
	write32(pdat->virt + SD_RISR, 0xffffffff);

	if(count)
		return FALSE;
	return TRUE;
}

static bool_t write_bytes(struct sdhci_t113_pdata_t * pdat, u32_t * buf, u32_t blkcount, u32_t blksize)
{
	u64_t count = blkcount * blksize;
	u32_t * tmp = buf;
	u32_t status, err, done;

	status = read32(pdat->virt + SD_STAR);
	err = read32(pdat->virt + SD_RISR) & SDXC_INTERRUPT_ERROR_BIT;
	while(!err && count)
	{
		if(!(status & SDXC_FIFO_FULL))
		{
			write32(pdat->virt + SD_FIFO, *tmp);
			tmp++;
			count -= sizeof(u32_t);
		}
		status = read32(pdat->virt + SD_STAR);
		err = read32(pdat->virt + SD_RISR) & SDXC_INTERRUPT_ERROR_BIT;
	}

	do {
		status = read32(pdat->virt + SD_RISR);
		err = status & SDXC_INTERRUPT_ERROR_BIT;
		if(blkcount > 1)
			done = status & SDXC_AUTO_COMMAND_DONE;
		else
			done = status & SDXC_DATA_OVER;
	} while(!done && !err);

	if(err & SDXC_INTERRUPT_ERROR_BIT)
		return FALSE;
	write32(pdat->virt + SD_GCTL, read32(pdat->virt + SD_GCTL) | SDXC_FIFO_RESET);
	write32(pdat->virt + SD_RISR, 0xffffffff);

	if(count)
		return FALSE;
	return TRUE;
}

static bool_t t113_transfer_data(struct sdhci_t113_pdata_t * pdat, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	u32_t dlen = (u32_t)(dat->blkcnt * dat->blksz);
	bool_t ret = FALSE;

	write32(pdat->virt + SD_BKSR, dat->blksz);
	write32(pdat->virt + SD_BYCR, dlen);
	if(dat->flag & MMC_DATA_READ)
	{
		if(!t113_transfer_command(pdat, cmd, dat))
			return FALSE;
		ret = read_bytes(pdat, (u32_t *)dat->buf, dat->blkcnt, dat->blksz);
	}
	else if(dat->flag & MMC_DATA_WRITE)
	{
		if(!t113_transfer_command(pdat, cmd, dat))
			return FALSE;
		ret = write_bytes(pdat, (u32_t *)dat->buf, dat->blkcnt, dat->blksz);
	}
	return ret;
}

static bool_t sdhci_t113_detect(struct sdhci_t * sdhci)
{
	struct sdhci_t113_pdata_t * pdat = (struct sdhci_t113_pdata_t *)sdhci->priv;

	if((pdat->cd >= 0) && gpio_get_value(pdat->cd))
		return FALSE;
	return TRUE;
}

static bool_t sdhci_t113_reset(struct sdhci_t * sdhci)
{
	struct sdhci_t113_pdata_t * pdat = (struct sdhci_t113_pdata_t *)sdhci->priv;

	write32(pdat->virt + SD_GCTL, SDXC_HARDWARE_RESET);
	return TRUE;
}

static bool_t sdhci_t113_setvoltage(struct sdhci_t * sdhci, u32_t voltage)
{
	return TRUE;
}

static bool_t sdhci_t113_setwidth(struct sdhci_t * sdhci, u32_t width)
{
	struct sdhci_t113_pdata_t * pdat = (struct sdhci_t113_pdata_t *)sdhci->priv;

	switch(width)
	{
	case MMC_BUS_WIDTH_1:
		write32(pdat->virt + SD_BWDR, SDXC_WIDTH1);
		break;
	case MMC_BUS_WIDTH_4:
		write32(pdat->virt + SD_BWDR, SDXC_WIDTH4);
		break;
	case MMC_BUS_WIDTH_8:
		write32(pdat->virt + SD_BWDR, SDXC_WIDTH8);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static bool_t sdhci_t113_update_clk(struct sdhci_t113_pdata_t * pdat)
{
	u32_t cmd = (1U << 31) | (1 << 21) | (1 << 13);

	write32(pdat->virt + SD_CMDR, cmd);
	ktime_t timeout = ktime_add_ms(ktime_get(), 1);
	do {
		if(ktime_after(ktime_get(), timeout))
			return FALSE;
	} while(read32(pdat->virt + SD_CMDR) & 0x80000000);
	write32(pdat->virt + SD_RISR, read32(pdat->virt + SD_RISR));
	return TRUE;
}

static bool_t sdhci_t113_setclock(struct sdhci_t * sdhci, u32_t clock)
{
	struct sdhci_t113_pdata_t * pdat = (struct sdhci_t113_pdata_t *)sdhci->priv;
	u32_t ratio = udiv32(clk_get_rate(pdat->pclk) + 2 * clock - 1, (2 * clock));

	if((ratio & 0xff) != ratio)
		return FALSE;
	write32(pdat->virt + SD_CKCR, read32(pdat->virt + SD_CKCR) & ~(1 << 16));
	write32(pdat->virt + SD_CKCR, ratio);
	if(!sdhci_t113_update_clk(pdat))
		return FALSE;
	write32(pdat->virt + SD_CKCR, read32(pdat->virt + SD_CKCR) | (3 << 16));
	if(!sdhci_t113_update_clk(pdat))
		return FALSE;
	return TRUE;
}

static bool_t sdhci_t113_transfer(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	struct sdhci_t113_pdata_t * pdat = (struct sdhci_t113_pdata_t *)sdhci->priv;
	if(!dat)
		return t113_transfer_command(pdat, cmd, dat);
	return t113_transfer_data(pdat, cmd, dat);
}

static struct device_t * sdhci_t113_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct sdhci_t113_pdata_t * pdat;
	struct sdhci_t * sdhci;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * pclk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(pclk))
		return NULL;

	pdat = malloc(sizeof(struct sdhci_t113_pdata_t));
	if(!pdat)
		return NULL;

	sdhci = malloc(sizeof(struct sdhci_t));
	if(!sdhci)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->pclk = strdup(pclk);
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->clk = dt_read_int(n, "clk-gpio", -1);
	pdat->clkcfg = dt_read_int(n, "clk-gpio-config", -1);
	pdat->cmd = dt_read_int(n, "cmd-gpio", -1);
	pdat->cmdcfg = dt_read_int(n, "cmd-gpio-config", -1);
	pdat->dat0 = dt_read_int(n, "dat0-gpio", -1);
	pdat->dat0cfg = dt_read_int(n, "dat0-gpio-config", -1);
	pdat->dat1 = dt_read_int(n, "dat1-gpio", -1);
	pdat->dat1cfg = dt_read_int(n, "dat1-gpio-config", -1);
	pdat->dat2 = dt_read_int(n, "dat2-gpio", -1);
	pdat->dat2cfg = dt_read_int(n, "dat2-gpio-config", -1);
	pdat->dat3 = dt_read_int(n, "dat3-gpio", -1);
	pdat->dat3cfg = dt_read_int(n, "dat3-gpio-config", -1);
	pdat->dat4 = dt_read_int(n, "dat4-gpio", -1);
	pdat->dat4cfg = dt_read_int(n, "dat4-gpio-config", -1);
	pdat->dat5 = dt_read_int(n, "dat5-gpio", -1);
	pdat->dat5cfg = dt_read_int(n, "dat5-gpio-config", -1);
	pdat->dat6 = dt_read_int(n, "dat6-gpio", -1);
	pdat->dat6cfg = dt_read_int(n, "dat6-gpio-config", -1);
	pdat->dat7 = dt_read_int(n, "dat7-gpio", -1);
	pdat->dat7cfg = dt_read_int(n, "dat7-gpio-config", -1);
	pdat->cd = dt_read_int(n, "cd-gpio", -1);
	pdat->cdcfg = dt_read_int(n, "cd-gpio-config", -1);

	sdhci->name = alloc_device_name(dt_read_name(n), -1);
	sdhci->voltage = MMC_VDD_27_36;
	sdhci->width = MMC_BUS_WIDTH_4;
	sdhci->clock = (u32_t)dt_read_long(n, "max-clock-frequency", 25 * 1000 * 1000);
	sdhci->removable = (pdat->cd >= 0) ? TRUE : FALSE;
	sdhci->isspi = FALSE;
	sdhci->detect = sdhci_t113_detect;
	sdhci->reset = sdhci_t113_reset;
	sdhci->setvoltage = sdhci_t113_setvoltage;
	sdhci->setwidth = sdhci_t113_setwidth;
	sdhci->setclock = sdhci_t113_setclock;
	sdhci->transfer = sdhci_t113_transfer;
	sdhci->priv = pdat;

	clk_enable(pdat->pclk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);

	if(pdat->clk >= 0)
	{
		if(pdat->clkcfg >= 0)
			gpio_set_cfg(pdat->clk, pdat->clkcfg);
		gpio_set_pull(pdat->clk, GPIO_PULL_UP);
	}
	if(pdat->cmd >= 0)
	{
		if(pdat->cmdcfg >= 0)
			gpio_set_cfg(pdat->cmd, pdat->cmdcfg);
		gpio_set_pull(pdat->cmd, GPIO_PULL_UP);
	}
	if(pdat->dat0 >= 0)
	{
		if(pdat->dat0cfg >= 0)
			gpio_set_cfg(pdat->dat0, pdat->dat0cfg);
		gpio_set_pull(pdat->dat0, GPIO_PULL_UP);
	}
	if(pdat->dat1 >= 0)
	{
		if(pdat->dat1cfg >= 0)
			gpio_set_cfg(pdat->dat1, pdat->dat1cfg);
		gpio_set_pull(pdat->dat1, GPIO_PULL_UP);
	}
	if(pdat->dat2 >= 0)
	{
		if(pdat->dat2cfg >= 0)
			gpio_set_cfg(pdat->dat2, pdat->dat2cfg);
		gpio_set_pull(pdat->dat2, GPIO_PULL_UP);
	}
	if(pdat->dat3 >= 0)
	{
		if(pdat->dat3cfg >= 0)
			gpio_set_cfg(pdat->dat3, pdat->dat3cfg);
		gpio_set_pull(pdat->dat3, GPIO_PULL_UP);
	}
	if(pdat->dat4 >= 0)
	{
		if(pdat->dat4cfg >= 0)
			gpio_set_cfg(pdat->dat4, pdat->dat4cfg);
		gpio_set_pull(pdat->dat4, GPIO_PULL_UP);
	}
	if(pdat->dat5 >= 0)
	{
		if(pdat->dat5cfg >= 0)
			gpio_set_cfg(pdat->dat5, pdat->dat5cfg);
		gpio_set_pull(pdat->dat5, GPIO_PULL_UP);
	}
	if(pdat->dat6 >= 0)
	{
		if(pdat->dat6cfg >= 0)
			gpio_set_cfg(pdat->dat6, pdat->dat6cfg);
		gpio_set_pull(pdat->dat6, GPIO_PULL_UP);
	}
	if(pdat->dat7 >= 0)
	{
		if(pdat->dat7cfg >= 0)
			gpio_set_cfg(pdat->dat7, pdat->dat7cfg);
		gpio_set_pull(pdat->dat7, GPIO_PULL_UP);
	}
	if(pdat->cd >= 0)
	{
		if(pdat->cdcfg >= 0)
			gpio_set_cfg(pdat->cd, pdat->cdcfg);
		gpio_set_pull(pdat->cd, GPIO_PULL_UP);
	}

	if(!(dev = register_sdhci(sdhci, drv)))
	{
		clk_disable(pdat->pclk);
		free(pdat->pclk);
		free_device_name(sdhci->name);
		free(sdhci->priv);
		free(sdhci);
		return NULL;
	}
	return dev;
}

static void sdhci_t113_remove(struct device_t * dev)
{
	struct sdhci_t * sdhci = (struct sdhci_t *)dev->priv;
	struct sdhci_t113_pdata_t * pdat = (struct sdhci_t113_pdata_t *)sdhci->priv;

	if(sdhci)
	{
		unregister_sdhci(sdhci);
		clk_disable(pdat->pclk);
		free(pdat->pclk);
		free_device_name(sdhci->name);
		free(sdhci->priv);
		free(sdhci);
	}
}

static void sdhci_t113_suspend(struct device_t * dev)
{
}

static void sdhci_t113_resume(struct device_t * dev)
{
}

static struct driver_t sdhci_t113 = {
	.name		= "sdhci-t113",
	.probe		= sdhci_t113_probe,
	.remove		= sdhci_t113_remove,
	.suspend	= sdhci_t113_suspend,
	.resume		= sdhci_t113_resume,
};

static __init void sdhci_t113_driver_init(void)
{
	register_driver(&sdhci_t113);
}

static __exit void sdhci_t113_driver_exit(void)
{
	unregister_driver(&sdhci_t113);
}

driver_initcall(sdhci_t113_driver_init);
driver_exitcall(sdhci_t113_driver_exit);
