/*
 * driver/sdhci-pl180.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <sd/sdhci.h>

#define PL180_POWER				(0x00)
#define PL180_CLOCK				(0x04)
#define PL180_ARGUMENT			(0x08)
#define PL180_COMMAND			(0x0c)
#define PL180_RESPCMD			(0x10)
#define PL180_RESP0				(0x14)
#define PL180_RESP1				(0x18)
#define PL180_RESP2				(0x1c)
#define PL180_RESP3				(0x20)
#define PL180_DATA_TIMER		(0x24)
#define PL180_DATA_LENGTH		(0x28)
#define PL180_DATA_CTRL			(0x2c)
#define PL180_DATA_CNT			(0x30)
#define PL180_STATUS			(0x34)
#define PL180_CLEAR				(0x38)
#define PL180_MASK0				(0x3c)
#define PL180_MASK1				(0x40)
#define PL180_SELECT			(0x44)
#define PL180_FIFO_CNT			(0x48)
#define PL180_FIFO				(0x80)

#define PL180_CMD_WAITRESP		(1 << 6)
#define PL180_CMD_LONGRSP		(1 << 7)
#define PL180_CMD_WAITINT		(1 << 8)
#define PL180_CMD_WAITPEND		(1 << 9)
#define PL180_CMD_ENABLE		(1 << 10)

#define	PL180_STAT_CMD_CRC_FAIL	(1 << 0)
#define	PL180_STAT_DAT_CRC_FAIL	(1 << 1)
#define	PL180_STAT_CMD_TIME_OUT	(1 << 2)
#define	PL180_STAT_DAT_TIME_OUT	(1 << 3)
#define	PL180_STAT_TX_UNDERRUN	(1 << 4)
#define	PL180_STAT_RX_OVERRUN	(1 << 5)
#define	PL180_STAT_CMD_RESP_END	(1 << 6)
#define	PL180_STAT_CMD_SENT		(1 << 7)
#define	PL180_STAT_DAT_END		(1 << 8)
#define	PL180_STAT_DAT_BLK_END	(1 << 10)
#define	PL180_STAT_CMD_ACT		(1 << 11)
#define	PL180_STAT_TX_ACT		(1 << 12)
#define	PL180_STAT_RX_ACT		(1 << 13)
#define	PL180_STAT_TX_FIFO_HALF	(1 << 14)
#define	PL180_STAT_RX_FIFO_HALF	(1 << 15)
#define	PL180_STAT_TX_FIFO_FULL	(1 << 16)
#define	PL180_STAT_RX_FIFO_FULL	(1 << 17)
#define	PL180_STAT_TX_FIFO_ZERO	(1 << 18)
#define	PL180_STAT_RX_DAT_ZERO	(1 << 19)
#define	PL180_STAT_TX_DAT_AVL	(1 << 20)
#define	PL180_STAT_RX_FIFO_AVL	(1 << 21)

#define PL180_CLR_CMD_CRC_FAIL	(1 << 0)
#define PL180_CLR_DAT_CRC_FAIL	(1 << 1)
#define PL180_CLR_CMD_TIMEOUT	(1 << 2)
#define PL180_CLR_DAT_TIMEOUT	(1 << 3)
#define PL180_CLR_TX_UNDERRUN	(1 << 4)
#define PL180_CLR_RX_OVERRUN	(1 << 5)
#define PL180_CLR_CMD_RESP_END	(1 << 6)
#define PL180_CLR_CMD_SENT		(1 << 7)
#define PL180_CLR_DAT_END		(1 << 8)
#define PL180_CLR_DAT_BLK_END	(1 << 10)

struct sdhci_pl180_pdata_t {
	virtual_addr_t virt;
};

static bool_t pl180_transfer_command(struct sdhci_pl180_pdata_t * pdat, struct sdhci_cmd_t * cmd)
{
	u32_t cmdidx;
	u32_t status;
	bool_t ret = TRUE;

	if(read32(pdat->virt + PL180_COMMAND) & PL180_CMD_ENABLE)
		write32(pdat->virt + PL180_COMMAND, 0x0);

	cmdidx = (cmd->cmdidx & 0xff) | PL180_CMD_ENABLE;
	if(cmd->resptype)
	{
		cmdidx |= PL180_CMD_WAITRESP;
		if(cmd->resptype & MMC_RSP_136)
			cmdidx |= PL180_CMD_LONGRSP;
	}

	write32(pdat->virt + PL180_ARGUMENT, cmd->cmdarg);
	write32(pdat->virt + PL180_COMMAND, cmdidx);

	do {
		status = read32(pdat->virt + PL180_STATUS);
	} while(!(status & (PL180_STAT_CMD_SENT | PL180_STAT_CMD_RESP_END | PL180_STAT_CMD_TIME_OUT | PL180_STAT_CMD_CRC_FAIL)));

	if(cmd->resptype & MMC_RSP_PRESENT)
	{
		cmd->response[0] = read32(pdat->virt + PL180_RESP0);
		if(cmd->resptype & MMC_RSP_136)
		{
			cmd->response[1] = read32(pdat->virt + PL180_RESP1);
			cmd->response[2] = read32(pdat->virt + PL180_RESP2);
			cmd->response[3] = read32(pdat->virt + PL180_RESP3);
		}
	}

	if(status & PL180_STAT_CMD_TIME_OUT)
		ret = FALSE;
	else if ((status & PL180_STAT_CMD_CRC_FAIL) && (cmd->resptype & MMC_RSP_CRC))
		ret = FALSE;

	write32(pdat->virt + PL180_CLEAR, (PL180_CLR_CMD_SENT | PL180_CLR_CMD_RESP_END | PL180_CLR_CMD_TIMEOUT | PL180_CLR_CMD_CRC_FAIL));
	return ret;
}

static bool_t read_bytes(struct sdhci_pl180_pdata_t * pdat, u32_t * buf, u32_t blkcount, u32_t blksize)
{
	u32_t * tmp = buf;
	u64_t count = blkcount * blksize;
	u32_t status, err;

	status = read32(pdat->virt + PL180_STATUS);
	err = status & (PL180_STAT_DAT_CRC_FAIL | PL180_STAT_DAT_TIME_OUT | PL180_STAT_RX_OVERRUN);
	while((!err) && (count >= sizeof(u32_t)))
	{
		if(status & PL180_STAT_RX_FIFO_AVL)
		{
			*(tmp) = read32(pdat->virt + PL180_FIFO);
			tmp++;
			count -= sizeof(u32_t);
		}
		status = read32(pdat->virt + PL180_STATUS);
		err = status & (PL180_STAT_DAT_CRC_FAIL | PL180_STAT_DAT_TIME_OUT | PL180_STAT_RX_OVERRUN);
	}

	err = status & (PL180_STAT_DAT_CRC_FAIL | PL180_STAT_DAT_TIME_OUT | PL180_STAT_DAT_BLK_END | PL180_STAT_RX_OVERRUN);
	while(!err)
	{
		status = read32(pdat->virt + PL180_STATUS);
		err = status & (PL180_STAT_DAT_CRC_FAIL | PL180_STAT_DAT_TIME_OUT | PL180_STAT_DAT_BLK_END | PL180_STAT_RX_OVERRUN);
	}

	if(status & PL180_STAT_DAT_TIME_OUT)
		return FALSE;
	else if (status & PL180_STAT_DAT_CRC_FAIL)
		return FALSE;
	else if (status & PL180_STAT_RX_OVERRUN)
		return FALSE;
	write32(pdat->virt + PL180_CLEAR, 0x1DC007FF);

	if(count)
		return FALSE;
	return TRUE;
}

static bool_t write_bytes(struct sdhci_pl180_pdata_t * pdat, u32_t * buf, u32_t blkcount, u32_t blksize)
{
	u32_t * tmp = buf;
	u64_t count = blkcount * blksize;
	u32_t status, err;
	int i;

	status = read32(pdat->virt + PL180_STATUS);
	err = status & (PL180_STAT_DAT_CRC_FAIL | PL180_STAT_DAT_TIME_OUT);
	while(!err && count)
	{
		if(status & PL180_STAT_TX_FIFO_HALF)
		{
			if(count >= 8 * sizeof(u32_t))
			{
				for(i = 0; i < 8; i++)
					write32(pdat->virt + PL180_FIFO, *(tmp + i));
				tmp += 8;
				count -= 8 * sizeof(u32_t);
			}
			else
			{
				while(count >= sizeof(u32_t))
				{
					write32(pdat->virt + PL180_FIFO, *tmp);
					tmp++;
					count -= sizeof(u32_t);
				}
			}
		}
		status = read32(pdat->virt + PL180_STATUS);
		err = status & (PL180_STAT_DAT_CRC_FAIL | PL180_STAT_DAT_TIME_OUT);
	}

	err = status & (PL180_STAT_DAT_CRC_FAIL | PL180_STAT_DAT_TIME_OUT | PL180_STAT_DAT_BLK_END);
	while(!err)
	{
		status = read32(pdat->virt + PL180_STATUS);
		err = status & (PL180_STAT_DAT_CRC_FAIL | PL180_STAT_DAT_TIME_OUT | PL180_STAT_DAT_BLK_END);
	}

	if(status & PL180_STAT_DAT_TIME_OUT)
		return FALSE;
	else if (status & PL180_STAT_DAT_CRC_FAIL)
		return FALSE;
	write32(pdat->virt + PL180_CLEAR, 0x1DC007FF);

	if(count)
		return FALSE;
	return TRUE;
}

static bool_t pl180_transfer_data(struct sdhci_pl180_pdata_t * pdat, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	u32_t dlen = (u32_t)(dat->blkcnt * dat->blksz);
	u32_t blksz_bits = ffs(dat->blksz) - 1;
	u32_t dctrl = (blksz_bits << 4) | (0x1 << 0) | (0x1 << 14);
	bool_t ret = FALSE;

	write32(pdat->virt + PL180_DATA_TIMER, 0xffff);
	write32(pdat->virt + PL180_DATA_LENGTH, dlen);

	if(dat->flag & MMC_DATA_READ)
	{
		dctrl |= (0x1 << 1);
		write32(pdat->virt + PL180_DATA_CTRL, dctrl);
		if(!pl180_transfer_command(pdat, cmd))
			return FALSE;
		ret = read_bytes(pdat, (u32_t *)dat->buf, dat->blkcnt, dat->blksz);
	}
	else if(dat->flag & MMC_DATA_WRITE)
	{
		if(!pl180_transfer_command(pdat, cmd))
			return FALSE;
		write32(pdat->virt + PL180_DATA_CTRL, dctrl);
		ret = write_bytes(pdat, (u32_t *)dat->buf, dat->blkcnt, dat->blksz);
	}
	return ret;
}

static bool_t sdhci_pl180_detect(struct sdhci_t * sdhci)
{
	return TRUE;
}

static bool_t sdhci_pl180_reset(struct sdhci_t * sdhci)
{
	return TRUE;
}

static bool_t sdhci_pl180_setwidth(struct sdhci_t * sdhci, u32_t width)
{
	return TRUE;
}

static bool_t sdhci_pl180_setclock(struct sdhci_t * sdhci, u32_t clock)
{
	return TRUE;
}

static bool_t sdhci_pl180_transfer(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	struct sdhci_pl180_pdata_t * pdat = (struct sdhci_pl180_pdata_t *)sdhci->priv;

	if(!dat)
		return pl180_transfer_command(pdat, cmd);
	return pl180_transfer_data(pdat, cmd, dat);
}

static struct device_t * sdhci_pl180_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct sdhci_pl180_pdata_t * pdat;
	struct sdhci_t * sdhci;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x181)
		return NULL;

	pdat = malloc(sizeof(struct sdhci_pl180_pdata_t));
	if(!pdat)
		return NULL;

	sdhci = malloc(sizeof(struct sdhci_t));
	if(!sdhci)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;

	sdhci->name = alloc_device_name(dt_read_name(n), -1);
	sdhci->voltage = MMC_VDD_27_36;
	sdhci->width = MMC_BUS_WIDTH_4;
	sdhci->clock = (u32_t)dt_read_long(n, "max-clock-frequency", 25 * 1000 * 1000);
	sdhci->removable = TRUE;
	sdhci->detect = sdhci_pl180_detect;
	sdhci->reset = sdhci_pl180_reset;
	sdhci->setwidth = sdhci_pl180_setwidth;
	sdhci->setclock = sdhci_pl180_setclock;
	sdhci->transfer = sdhci_pl180_transfer;
	sdhci->priv = pdat;
	write32(pdat->virt + PL180_POWER, 0xbf);

	if(!(dev = register_sdhci(sdhci, drv)))
	{
		free_device_name(sdhci->name);
		free(sdhci->priv);
		free(sdhci);
		return NULL;
	}
	return dev;
}

static void sdhci_pl180_remove(struct device_t * dev)
{
	struct sdhci_t * sdhci = (struct sdhci_t *)dev->priv;

	if(sdhci)
	{
		unregister_sdhci(sdhci);
		free_device_name(sdhci->name);
		free(sdhci->priv);
		free(sdhci);
	}
}

static void sdhci_pl180_suspend(struct device_t * dev)
{
}

static void sdhci_pl180_resume(struct device_t * dev)
{
}

static struct driver_t sdhci_pl180 = {
	.name		= "sdhci-pl180",
	.probe		= sdhci_pl180_probe,
	.remove		= sdhci_pl180_remove,
	.suspend	= sdhci_pl180_suspend,
	.resume		= sdhci_pl180_resume,
};

static __init void sdhci_pl180_driver_init(void)
{
	register_driver(&sdhci_pl180);
}

static __exit void sdhci_pl180_driver_exit(void)
{
	unregister_driver(&sdhci_pl180);
}

driver_initcall(sdhci_pl180_driver_init);
driver_exitcall(sdhci_pl180_driver_exit);
