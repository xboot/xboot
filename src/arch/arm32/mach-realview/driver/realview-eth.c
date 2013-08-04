/*
 * driver/realview_eth.c
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
#include <realview/reg-eth.h>

static u32_t lan9118_mac_read(u8_t reg)
{
	while(readl(REALVIEW_ETH_MAC_CSR_CMD) & 0x80000000);
	writel(REALVIEW_ETH_MAC_CSR_CMD, (reg & 0xff) | (0x80000000 | 0x40000000));

	while(readl(REALVIEW_ETH_MAC_CSR_CMD) & 0x80000000);
	return readl(REALVIEW_ETH_MAC_CSR_DATA);
}

static void lan9118_mac_write(u8_t reg, u32_t val)
{
	while(readl(REALVIEW_ETH_MAC_CSR_CMD) & 0x80000000);
	writel(REALVIEW_ETH_MAC_CSR_DATA, val);

	writel(REALVIEW_ETH_MAC_CSR_CMD, (reg & 0xff) | (0x80000000));
	while(readl(REALVIEW_ETH_MAC_CSR_CMD) & 0x80000000);
}

static u32_t lan9118_phy_read(u8_t reg)
{
	while(lan9118_mac_read(0x06) & (1<<0));
	lan9118_mac_write(0x06, ((1 & 0x1f) << 11) | ((reg & 0x1f) << 6));

	while(lan9118_mac_read(0x06) & (1<<0));
	return lan9118_mac_read(0x7);
}

static void lan9118_phy_write(u8_t reg, u32_t val)
{
	while(lan9118_mac_read(0x06) & (1<<0));
	lan9118_mac_write(0x7, val);

	lan9118_mac_write(0x6, ((1 & 0x1f) << 11) | ((reg & 0x1f) << 6) | (1 << 1));
	while(lan9118_mac_read(0x06) & (1<<0));
}

static void lan9118_reset(void)
{
	writel(REALVIEW_ETH_HW_CFG, readl(REALVIEW_ETH_HW_CFG) | (1 << 0));
    while(readl(REALVIEW_ETH_HW_CFG) & (1 << 0));

    writel(REALVIEW_ETH_PMT_CTRL, readl(REALVIEW_ETH_PMT_CTRL) | (1 << 10));
    while(readl(REALVIEW_ETH_PMT_CTRL) & (1 << 10));
}

static bool_t lan9118_verify_chip_id(void)
{
	if(readl(REALVIEW_ETH_BYTE_TEST) != 0x87654321)
		return FALSE;

	if((readl(REALVIEW_ETH_ID_REV) >> 16) != 0x0118)
		return FALSE;

	return TRUE;
}

static void lan9118_set_mac(const u8_t * mac)
{
	u32_t val;

	val = (mac[5] << 8) | (mac[4] << 0);
	lan9118_mac_write(0x2, val);

	val = (mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | (mac[0] << 0);
	lan9118_mac_write(0x3, val);
}

static void lan9118_enable_multicast_reception(void)
{
	u32_t val;

	val = lan9118_mac_read(0x1);
	val |= (1 << 18);
	lan9118_mac_write(0x1, val);
}

static void lan9118_disable_multicast_reception(void)
{
	u32_t val;

	val = lan9118_mac_read(0x1);
	val &= ~(1 << 18);
	lan9118_mac_write(0x1, val);
}

static void lan9118_enable_promiscuous_reception(void)
{
	u32_t val;

	val = lan9118_mac_read(0x1);
	val |= (1 << 18);
	lan9118_mac_write(0x1, val);
}

static void lan9118_disable_promiscuous_reception(void)
{
	u32_t val;

	val = lan9118_mac_read(0x1);
	val &= ~(1 << 18);
	lan9118_mac_write(0x1, val);
}

static void lan9118_enable_broadcast_reception(void)
{
	u32_t val;

	val = lan9118_mac_read(0x1);
	val |= (1 << 11);
	lan9118_mac_write(0x1, val);
}

static void lan9118_disable_broadcast_reception(void)
{
	u32_t val;

	val = lan9118_mac_read(0x1);
	val &= ~(1 << 11);
	lan9118_mac_write(0x1, val);
}

static bool_t lan9118_init(void)
{
	const u8_t mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	u32_t val;

	/*
     * Set the PHY clock config
     */
	writel(REALVIEW_ETH_HW_CFG, (readl(REALVIEW_ETH_HW_CFG) & ~(0x3 << 5)) | (0 << 5));

	/*
	 * Reset the controller
	 */
	lan9118_reset();

	/*
	 * Verify Chip ID
	 */
	if(!lan9118_verify_chip_id())
		return FALSE;

	/*
	 * Set mac address
	 */
	lan9118_set_mac(mac);

	/*
	 * These came from SMSC's example driver
	 */
	writel(REALVIEW_ETH_HW_CFG, (readl(REALVIEW_ETH_HW_CFG) & ~(0xf << 16)) | ((5 & 0xf) << 16));
	writel(REALVIEW_ETH_AFC_CFG, ((0x6e & 0xff) << 16) | ((0x37 & 0xff) << 8) | ((0x4 & 0xf) << 4));

	/*
	 * Configure PHY to advertise all speeds
	 */
	lan9118_phy_write(0x4, ((1 << 8) | (1 << 7) | (1 << 6) | (1 << 5) | 0x01));

	/*
	 * Enable auto negotiation
	 */
	lan9118_phy_write(0x0, ((1 << 12) | (1 << 9)));

	/*
	 * Set the controller to buffer entire packets
	 */
	writel(REALVIEW_ETH_HW_CFG, readl(REALVIEW_ETH_HW_CFG) | (1 << 20));

	/*
	 * Configure FIFO thresholds
	 */
	writel(REALVIEW_ETH_FIFO_INT, 0xff << 24);

	/*
	 * Enable the MAC
	 */
	val = lan9118_mac_read(0x1);
	val |= (1<<3) | (1<<2);
	lan9118_mac_write(0x1, val);

	writel(REALVIEW_ETH_TX_CFG, readl(REALVIEW_ETH_TX_CFG) | (1 << 1));
	writel(REALVIEW_ETH_RX_CFG, 0);

	return TRUE;
}

static void lan9118_tx(u8_t * txbuf, size_t len)
{
	u32_t * p = (u32_t *)txbuf;
	u32_t cmda, cmdb;
	u32_t val;
	int i;

	val = readl(REALVIEW_ETH_TX_STATUS_FIFO);
	do {
		val = readl(REALVIEW_ETH_TX_FIFO_INF) & (0xffff <<  0);
	} while(val < len);

	cmda = ((1 << 13) | (1 << 12) | ((len & 0x7ff) << 0));
	cmdb = ((len & 0x7ff) << 0);

	writel(REALVIEW_ETH_TX_DATA_FIFO, cmda);
	writel(REALVIEW_ETH_TX_DATA_FIFO, cmdb);

	for(i = 0; i < (len / 4); i++)
	{
		writel(REALVIEW_ETH_TX_DATA_FIFO, p[i]);
	}

	if((len & 0x3) != 0)
	{
		int index = len & ~3;
		int num = len & 3;

		for(i = 0; i < num; i++)
		{
			val |= (txbuf[index + i] << (i * 8));
		}

		writel(REALVIEW_ETH_TX_DATA_FIFO, val);
	}
}

static size_t lan9118_rx(u8_t * rxbuf)
{
	u32_t *p = (u32_t *)rxbuf;
	size_t size;
	u32_t status;
	int i;

	if( ((readl(REALVIEW_ETH_RX_FIFO_INF) >> 16) & 0xffff) == 0 )
		return 0;

	status = readl(REALVIEW_ETH_RX_STATUS_FIFO);
	size = (status & (0x3fff << 16)) >> 16;
	if(size == 0)
		return 0;

	if((status & (1 << 15)) == 0)
	{
		for (i = 0; i < ((size + 3) / 4); i++)
		{
			p[i] = readl(REALVIEW_ETH_RX_DATA_FIFO);
		}

		return size - 4;
	}
	else
	{
		if (size >= 16)
		{
			writel(REALVIEW_ETH_RX_DP_CTRL, (1 << 31));
			while(readl(REALVIEW_ETH_RX_DP_CTRL) & (1 << 31));
		}
		else
		{
			for (i = 0; i < ((size + 3) / 4); i++)
			{
				status = readl(REALVIEW_ETH_RX_DATA_FIFO);
			}
		}
		return 0;
	}
}

static __init void realview_eth_init(void)
{
	lan9118_init();
}

static __exit void realview_eth_exit(void)
{
}

device_initcall(realview_eth_init);
device_exitcall(realview_eth_exit);
