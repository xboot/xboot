/*
 * driver/sdhci-spi.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <spi/spi.h>
#include <gpio/gpio.h>
#include <sd/sdhci.h>

struct sdhci_spi_pdata_t {
	struct spi_device_t * dev;
	int cd;
	int cdcfg;
};

static const u8_t crc7_table[256] = {
	0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
	0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
	0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
	0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
	0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
	0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
	0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
	0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
	0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
	0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
	0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
	0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
	0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
	0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
	0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
	0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
	0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
	0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
	0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
	0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
	0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
	0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
	0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
	0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
	0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
	0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
	0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
	0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
	0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
	0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
	0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
	0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79,
};

static u8_t crc7(const u8_t * buf, int len)
{
	u8_t crc = 0;
	int i;

	for(i = 0; i < len; i++)
		crc = crc7_table[(crc << 1) ^ *buf++];
	return (crc << 1) | 0x1;
}

static bool_t spi_transfer_wait_response(struct sdhci_spi_pdata_t * pdat, u8_t * rx, u8_t mask, u8_t value)
{
	int n = 1000;

	while(--n > 0)
	{
		if((spi_device_write_then_read(pdat->dev, 0, 0, rx, 1) >= 0) && ((*rx & mask) == value))
			return TRUE;
	}
	return FALSE;
}

static bool_t sdhci_spi_detect(struct sdhci_t * sdhci)
{
	struct sdhci_spi_pdata_t * pdat = (struct sdhci_spi_pdata_t *)sdhci->priv;

	if((pdat->cd >= 0) && gpio_get_value(pdat->cd))
		return FALSE;
	return TRUE;
}

static bool_t sdhci_spi_setvoltage(struct sdhci_t * sdhci, u32_t voltage)
{
	return TRUE;
}

static bool_t sdhci_spi_setwidth(struct sdhci_t * sdhci, u32_t width)
{
	return TRUE;
}

static bool_t sdhci_spi_setclock(struct sdhci_t * sdhci, u32_t clock)
{
	struct sdhci_spi_pdata_t * pdat = (struct sdhci_spi_pdata_t *)sdhci->priv;

	pdat->dev->speed = clock;
	return TRUE;
}

static bool_t sdhci_spi_transfer(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	struct sdhci_spi_pdata_t * pdat = (struct sdhci_spi_pdata_t *)sdhci->priv;
	bool_t ret = FALSE;
	u8_t tx[6], rx[16], crc[2];
	u8_t r;
	int i;

	if(cmd && cmd->cmdidx == MMC_GO_IDLE_STATE)
	{
		r = 0xff;
		for(i = 0; i < 10; i++)
			spi_device_write_then_read(pdat->dev, &r, 1, 0, 0);
		udelay(200);
	}

	spi_device_select(pdat->dev);
	if(cmd)
	{
		tx[0] = (0x1 << 6) | (cmd->cmdidx & 0x3f);
		tx[1] = (cmd->cmdarg >> 24) & 0xff;
		tx[2] = (cmd->cmdarg >> 16) & 0xff;
		tx[3] = (cmd->cmdarg >> 8) & 0xff;
		tx[4] = (cmd->cmdarg >> 0) & 0xff;
		tx[5] = crc7(tx, 5);
		spi_device_write_then_read(pdat->dev, &tx, 6, 0, 0);
		switch(cmd->resptype)
		{
		case MMC_RSP_NONE:
			break;
		case MMC_RSP_R1:
			if(spi_transfer_wait_response(pdat, &r, 0x80, 0x00))
			{
				if((cmd->cmdidx == MMC_SEND_CID) || (cmd->cmdidx == MMC_SEND_CSD))
				{
					if(spi_transfer_wait_response(pdat, &r, 0xff, 0xfe))
					{
						if(spi_device_write_then_read(pdat->dev, 0, 0, &rx[0], 16) >= 0)
						{
							spi_device_write_then_read(pdat->dev, 0, 0, &crc[0], 2);
							cmd->response[0] = (rx[0] << 24) | (rx[1] << 16) | (rx[2] << 8) | (rx[3] << 0);
							cmd->response[1] = (rx[4] << 24) | (rx[5] << 16) | (rx[6] << 8) | (rx[7] << 0);
							cmd->response[2] = (rx[8] << 24) | (rx[9] << 16) | (rx[10] << 8) | (rx[11] << 0);
							cmd->response[3] = (rx[12] << 24) | (rx[13] << 16) | (rx[14] << 8) | (rx[15] << 0);
							ret = TRUE;
						}
					}
				}
				else
				{
					ret = TRUE;
				}
			}
			break;
		case MMC_RSP_R1B:
			if(spi_transfer_wait_response(pdat, &r, 0xff, 0x00))
				ret = TRUE;
			break;
		case MMC_RSP_R2:
			if(spi_transfer_wait_response(pdat, &r, 0x80, 0x00))
				ret = TRUE;
			break;
		case MMC_RSP_R3:
			if(spi_transfer_wait_response(pdat, &r, 0x80, 0x00))
			{
				if(spi_device_write_then_read(pdat->dev, 0, 0, &rx[0], 4) >= 0)
				{
					cmd->response[0] = (rx[0] << 24) | (rx[1] << 16) | (rx[2] << 8) | (rx[3] << 0);
					ret = TRUE;
				}
			}
			break;
		case MMC_RSP_R4:
			break;
		case MMC_RSP_R5:
			break;
		case MMC_RSP_R6:
			break;
		case MMC_RSP_R7:
			if(spi_transfer_wait_response(pdat, &r, 0x80, 0x00))
			{
				if(spi_device_write_then_read(pdat->dev, 0, 0, &rx[0], 4) >= 0)
				{
					cmd->response[0] = (rx[0] << 24) | (rx[1] << 16) | (rx[2] << 8) | (rx[3] << 0);
					ret = TRUE;
				}
			}
			break;
		default:
			break;
		}
	}
	if(dat)
	{
		if(dat->flag & MMC_DATA_READ)
		{
			if(spi_transfer_wait_response(pdat, &r, 0xff, 0xfe))
			{
				if(spi_device_write_then_read(pdat->dev, 0, 0, dat->buf, dat->blkcnt * dat->blksz) >= 0)
				{
					spi_device_write_then_read(pdat->dev, 0, 0, &crc[0], 2);
					ret = TRUE;
				}
			}
		}
		else if(dat->flag & MMC_DATA_WRITE)
		{
		}
	}
	spi_device_deselect(pdat->dev);

	r = 0xff;
	spi_device_write_then_read(pdat->dev, &r, 1, 0, 0);
	return ret;
}

static struct device_t * sdhci_spi_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct sdhci_spi_pdata_t * pdat;
	struct sdhci_t * sdhci;
	struct device_t * dev;
	struct spi_device_t * spidev;

	spidev = spi_device_alloc(dt_read_string(n, "spi-bus", NULL), dt_read_int(n, "chip-select", 0), dt_read_int(n, "type", 0), dt_read_int(n, "mode", 3), 8, dt_read_int(n, "speed", 0));
	if(!spidev)
		return NULL;

	pdat = malloc(sizeof(struct sdhci_spi_pdata_t));
	if(!pdat)
	{
		spi_device_free(spidev);
		return FALSE;
	}

	sdhci = malloc(sizeof(struct sdhci_t));
	if(!sdhci)
	{
		spi_device_free(spidev);
		free(pdat);
		return FALSE;
	}

	pdat->dev = spidev;
	pdat->cd = dt_read_int(n, "cd-gpio", -1);
	pdat->cdcfg = dt_read_int(n, "cd-gpio-config", -1);

	sdhci->name = alloc_device_name(dt_read_name(n), -1);
	sdhci->voltage = MMC_VDD_27_36;
	sdhci->width = MMC_BUS_WIDTH_1;
	sdhci->clock = (u32_t)dt_read_long(n, "clock-frequency", 1 * 1000 * 1000);
	sdhci->removeable = dt_read_bool(n, "removeable", 0) ? TRUE : FALSE;
	sdhci->isspi = TRUE;
	sdhci->detect = sdhci_spi_detect;
	sdhci->setvoltage = sdhci_spi_setvoltage;
	sdhci->setwidth = sdhci_spi_setwidth;
	sdhci->setclock = sdhci_spi_setclock;
	sdhci->transfer = sdhci_spi_transfer;
	sdhci->priv = pdat;

	if(pdat->cd >= 0)
	{
		if(pdat->cdcfg >= 0)
			gpio_set_cfg(pdat->cd, pdat->cdcfg);
		gpio_set_pull(pdat->cd, GPIO_PULL_UP);
		gpio_set_direction(pdat->cd, GPIO_DIRECTION_INPUT);
	}

	if(!register_sdhci(&dev, sdhci))
	{
		spi_device_free(pdat->dev);

		free_device_name(sdhci->name);
		free(sdhci->priv);
		free(sdhci);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void sdhci_spi_remove(struct device_t * dev)
{
	struct sdhci_t * sdhci = (struct sdhci_t *)dev->priv;
	struct sdhci_spi_pdata_t * pdat = (struct sdhci_spi_pdata_t *)sdhci->priv;

	if(sdhci && unregister_sdhci(sdhci))
	{
		spi_device_free(pdat->dev);

		free_device_name(sdhci->name);
		free(sdhci->priv);
		free(sdhci);
	}
}

static void sdhci_spi_suspend(struct device_t * dev)
{
}

static void sdhci_spi_resume(struct device_t * dev)
{
}

static struct driver_t sdhci_spi = {
	.name		= "sdhci-spi",
	.probe		= sdhci_spi_probe,
	.remove		= sdhci_spi_remove,
	.suspend	= sdhci_spi_suspend,
	.resume		= sdhci_spi_resume,
};

static __init void sdhci_spi_driver_init(void)
{
	register_driver(&sdhci_spi);
}

static __exit void sdhci_spi_driver_exit(void)
{
	unregister_driver(&sdhci_spi);
}

driver_initcall(sdhci_spi_driver_init);
driver_exitcall(sdhci_spi_driver_exit);
