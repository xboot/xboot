/*
 * driver/sdhci-pl180.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#define PL180_RSP_NONE			(0 << 0)
#define PL180_RSP_PRESENT		(1 << 0)
#define PL180_RSP_136BIT		(1 << 1)
#define PL180_RSP_CRC			(1 << 2)

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

static bool_t mmci_command(struct sdhci_pl180_pdata_t * pdat, struct sdhci_cmd_t * cmd)
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
		if(cmd->resptype & PL180_RSP_136BIT)
			cmdidx |= PL180_CMD_LONGRSP;
	}

	write32(pdat->virt + PL180_ARGUMENT, cmd->cmdarg);
	write32(pdat->virt + PL180_COMMAND, cmdidx);

	do {
		status = read32(pdat->virt + PL180_STATUS);
	} while(!(status & (PL180_STAT_CMD_SENT | PL180_STAT_CMD_RESP_END | PL180_STAT_CMD_TIME_OUT | PL180_STAT_CMD_CRC_FAIL)));

	if(cmd->resptype & PL180_RSP_PRESENT)
	{
		cmd->response[0] = read32(pdat->virt + PL180_RESP0);
		if(cmd->resptype & PL180_RSP_136BIT)
		{
			cmd->response[1] = read32(pdat->virt + PL180_RESP1);
			cmd->response[2] = read32(pdat->virt + PL180_RESP2);
			cmd->response[3] = read32(pdat->virt + PL180_RESP3);
		}
	}

	if(status & PL180_STAT_CMD_TIME_OUT)
		ret = FALSE;
	else if ((status & PL180_STAT_CMD_CRC_FAIL) && (cmd->resptype & PL180_RSP_CRC))
		ret = FALSE;

	write32(pdat->virt + PL180_CLEAR, (PL180_CLR_CMD_SENT | PL180_CLR_CMD_RESP_END | PL180_CLR_CMD_TIMEOUT | PL180_CLR_CMD_CRC_FAIL));
	return ret;
}

static bool_t mmci_transfer(struct sdhci_pl180_pdata_t * pdat, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	bool_t ret = TRUE;
	return ret;
}

static void sdhci_pl180_reset(struct sdhci_t * sdhci)
{
	struct sdhci_pl180_pdata_t * pdat = (struct sdhci_pl180_pdata_t *)sdhci->priv;
	write32(pdat->virt + PL180_POWER, 0xbf);
}

static bool_t sdhci_pl180_getcd(struct sdhci_t * sdhci)
{
	return TRUE;
}

static bool_t sdhci_pl180_getwp(struct sdhci_t * sdhci)
{
	return FALSE;
}

static bool_t sdhci_pl180_setios(struct sdhci_t * sdhci, struct sdhci_ios_t * ios)
{
	return TRUE;
}

static bool_t sdhci_pl180_request(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	struct sdhci_pl180_pdata_t * pdat = (struct sdhci_pl180_pdata_t *)sdhci->priv;
	bool_t ret;

	if(dat)
		ret = mmci_transfer(pdat, cmd, dat);
	else
		ret = mmci_command(pdat, cmd);
	return ret;
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
		return FALSE;

	sdhci = malloc(sizeof(struct sdhci_t));
	if(!sdhci)
	{
		free(pdat);
		return FALSE;
	}

	pdat->virt = virt;

	sdhci->name = alloc_device_name(dt_read_name(n), -1);
	sdhci->reset = sdhci_pl180_reset;
	sdhci->getcd = sdhci_pl180_getcd;
	sdhci->getwp = sdhci_pl180_getwp;
	sdhci->setios = sdhci_pl180_setios;
	sdhci->request = sdhci_pl180_request;
	sdhci->priv = pdat;

	if(!register_sdhci(&dev, sdhci))
	{
		free_device_name(sdhci->name);
		free(sdhci->priv);
		free(sdhci);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void sdhci_pl180_remove(struct device_t * dev)
{
	struct sdhci_t * sdhci = (struct sdhci_t *)dev->priv;

	if(sdhci && unregister_sdhci(sdhci))
	{
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
