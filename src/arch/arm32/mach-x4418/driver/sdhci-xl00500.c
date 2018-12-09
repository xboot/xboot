/*
 * driver/sdhci-xl00500.c
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
#include <sd/sdhci.h>
#include <gpio/gpio.h>
#include <clk/clk.h>
#include <s5p4418-gpio.h>
#include "sdhci-xl00500.h"

struct gpio_desc
{
	int no;
	int alt;
};

struct sd_gpios
{
	struct gpio_desc D0;
	struct gpio_desc D1;
	struct gpio_desc D2;
	struct gpio_desc D3;
	struct gpio_desc CLK;
	struct gpio_desc CMD;
	struct gpio_desc CD;
};

struct sdhci_xl00500_pdata_t
{
	virtual_addr_t virt;
	char *clk_name;
	int port;
};

static u32_t const sd_reset_num[3] = {
	RESETINDEX_OF_SDMMC0_MODULE_i_nRST,
	RESETINDEX_OF_SDMMC1_MODULE_i_nRST,
	RESETINDEX_OF_SDMMC2_MODULE_i_nRST
};

static struct sd_gpios gpios[3] = {
	[0] =
	{
		.D0 = { S5P4418_GPIOB1, 1 },
		.D1 = { S5P4418_GPIOB3, 1 },
		.D2 = { S5P4418_GPIOB5, 1 },
		.D3 = { S5P4418_GPIOB7, 1 },
		.CMD = { S5P4418_GPIOA31, 1 },
		.CLK = { S5P4418_GPIOA29, 1 },
		.CD = { S5P4418_GPIOALV1, -1 },
	},
	[1] =
	{
		.D0 = { S5P4418_GPIOD24, 1 },
		.D1 = { S5P4418_GPIOD25, 1 },
		.D2 = { S5P4418_GPIOD26, 1 },
		.D3 = { S5P4418_GPIOD27, 1 },
		.CMD = { S5P4418_GPIOD23, 1 },
		.CLK = { S5P4418_GPIOD22, 1 },
		.CD = { S5P4418_GPIOB25, 1 },
	},
	[2] =
	{
		.D0 = { S5P4418_GPIOC20, 2 },
		.D1 = { S5P4418_GPIOC21, 2 },
		.D2 = { S5P4418_GPIOC22, 2 },
		.D3 = { S5P4418_GPIOC23, 2 },
		.CMD = { S5P4418_GPIOC19, 2 },
		.CLK = { S5P4418_GPIOC18, 2 },
		.CD = { -1, -1 },
	},
};

static struct clkgen_register_set * get_clkgen_register_set(int port)
{
	if(port == 0)
		return phys_to_virt(PHY_BASEADDR_CLKGEN18_MODULE);
	else if(port == 1)
		return phys_to_virt(PHY_BASEADDR_CLKGEN19_MODULE);
	else if(port == 2)
		return phys_to_virt(PHY_BASEADDR_CLKGEN20_MODULE);
	else
		return NULL;
}
;

static void reset_con(u32_t devicenum, bool_t en)
{
	struct rstcon_register_set *preg_rstcon = (struct rstcon_register_set *)phys_to_virt(PHY_BASEADDR_RSTCON_MODULE);
	if(en)
		ClearIO32(&preg_rstcon->REGRST[(devicenum >> 5) & 0x3], (0x1 << (devicenum & 0x1F))); // reset
	else
		SetIO32(&preg_rstcon->REGRST[(devicenum >> 5) & 0x3], (0x1 << (devicenum & 0x1F))); // reset negate
}

static void sdhci_xl00500_gpio_init(struct sdhci_xl00500_pdata_t *pdat)
{
	int port = pdat->port;

	gpio_set_cfg(gpios[port].D0.no, gpios[port].D0.alt);
	gpio_set_rate(gpios[port].D0.no, GPIO_RATE_SLOW);
	gpio_set_pull(gpios[port].D0.no, GPIO_PULL_UP);
	gpio_set_drv(gpios[port].D0.no, GPIO_DRV_STRONGER);

	gpio_set_cfg(gpios[port].D1.no, gpios[port].D1.alt);
	gpio_set_rate(gpios[port].D1.no, GPIO_RATE_SLOW);
	gpio_set_pull(gpios[port].D1.no, GPIO_PULL_UP);
	gpio_set_drv(gpios[port].D1.no, GPIO_DRV_STRONGER);

	gpio_set_cfg(gpios[port].D2.no, gpios[port].D2.alt);
	gpio_set_rate(gpios[port].D2.no, GPIO_RATE_SLOW);
	gpio_set_pull(gpios[port].D2.no, GPIO_PULL_UP);
	gpio_set_drv(gpios[port].D2.no, GPIO_DRV_STRONGER);

	gpio_set_cfg(gpios[port].D3.no, gpios[port].D3.alt);
	gpio_set_rate(gpios[port].D3.no, GPIO_RATE_SLOW);
	gpio_set_pull(gpios[port].D3.no, GPIO_PULL_UP);
	gpio_set_drv(gpios[port].D3.no, GPIO_DRV_STRONGER);

	gpio_set_cfg(gpios[port].CMD.no, gpios[port].CMD.alt);
	gpio_set_rate(gpios[port].CMD.no, GPIO_RATE_SLOW);
	gpio_set_pull(gpios[port].CMD.no, GPIO_PULL_UP);
	gpio_set_drv(gpios[port].CMD.no, GPIO_DRV_STRONGER);

	gpio_set_cfg(gpios[port].CLK.no, gpios[port].CLK.alt);
	gpio_set_rate(gpios[port].CLK.no, GPIO_RATE_SLOW);
	gpio_set_drv(gpios[port].CLK.no, GPIO_DRV_STRONGER);

	if(gpios[port].CD.no != -1)
	{
		gpio_set_cfg(gpios[port].CD.no, gpios[port].CD.alt);
		gpio_set_direction(gpios[port].CD.no, GPIO_DIRECTION_INPUT);
	}

}

static u32_t get_pllnum(char *clk_name)
{
	if(strcmp(clk_name, "pll0") == 0)
		return 0;
	if(strcmp(clk_name, "pll1") == 0)
		return 1;
	if(strcmp(clk_name, "pll2") == 0)
		return 2;
	if(strcmp(clk_name, "pll3") == 0)
		return 3;
	return -1;
}

static bool_t sdmmc_getclkparam(char *clk_name, clkinfo *info)
{
	u32_t src_freq = clk_get_rate(clk_name);
	if((src_freq == 0) || (src_freq / (256 * 255 * 2) > info->freq) || (src_freq < info->freq))
		return FALSE;

	u64_t min = ~0ull;

	for(u32_t i = 0; i < 256; i++)
	{
		u64_t temp;
		u32_t div = i * 2;
		if(i == 0)
			div = 1;
		temp = (u64_t)div * (u64_t)info->freq;
		if(temp > src_freq)
			continue;
		assert((temp & 0xffffffff00000000ull) == 0);
		u32_t gendiv = src_freq / (div * info->freq);
		if(gendiv > 256)
			continue;
		temp = gendiv * div * info->freq;
		if(temp < src_freq)
			gendiv++;
		temp = gendiv * div * info->freq;
		assert(temp >= src_freq);
		temp = temp - src_freq;

		if(temp < min)
		{
			min = temp;
			info->clkdiv = div;
			info->clkgendiv = gendiv;
			//LOG("f = %d,gendiv =%d, div =%d,rst =%d d = %llu", info->freq, gendiv, div, src_freq / (gendiv * div), temp);
		}
	}
	return min != ~0ull;
}

static bool_t sdhci_xl00500_clk_init(struct sdhci_xl00500_pdata_t *pdat)
{
	struct sdmmc_register_set * const psdmmc_reg = (struct sdmmc_register_set *)pdat->virt;
	struct clkgen_register_set * const psd_clkgen_reg = get_clkgen_register_set(pdat->port);
	clkinfo clk_info;

	clk_info.pllnum = get_pllnum(pdat->clk_name);
	clk_info.freq = 25000000;

	if(sdmmc_getclkparam(pdat->clk_name, &clk_info) == FALSE)
	{
		LOG("failed to get clock param.");
		return FALSE;
	}

	psd_clkgen_reg->CLKENB = PCLKMODE_ALWAYS << 3 | BCLKMODE_DYNAMIC << 0;

	psd_clkgen_reg->CLKGEN = (psd_clkgen_reg->CLKGEN & ~(0x7 << 2 | 0xFF << 5)) | (clk_info.pllnum << 2)				// set clock source
	        | ((clk_info.clkgendiv - 1) << 5)		// set clock divisor
	        | (0UL << 1);							// set clock invert
	psd_clkgen_reg->CLKENB |= 0x1UL << 2;		// clock generation enable

	reset_con(sd_reset_num[pdat->port], TRUE);	// reset on
	reset_con(sd_reset_num[pdat->port], FALSE);	// reset negate

	psdmmc_reg->PWREN = 0 << 0;					// Set Power Disable
	//psdmmc_reg->UHS_REG |= 1<<0;				// for DDR mode
	psdmmc_reg->CLKENA = SDXC_CLKENA_LOWPWR;	// low power mode & clock disable
	psdmmc_reg->CLKCTRL = 0 << 24 |				// sample clock phase shift 0:0 1:90 2:180 3:270
	        2 << 16 |				// drive clock phase shift 0:0 1:90 2:180 3:270
	        0 << 8 |				// sample clock delay
	        0 << 0;				// drive clock delay

	psdmmc_reg->CLKSRC = 0;	// prescaler 0
	psdmmc_reg->CLKDIV = (clk_info.clkdiv >> 1);
	psdmmc_reg->CTRL &= ~(SDXC_CTRL_DMAMODE_EN | SDXC_CTRL_READWAIT);	// fifo mode, not read wait(only use sdio mode)
	// Reset the controller & DMA interface & FIFO
	psdmmc_reg->CTRL = SDXC_CTRL_DMARST | SDXC_CTRL_FIFORST | SDXC_CTRL_CTRLRST;
	while(psdmmc_reg->CTRL & (SDXC_CTRL_DMARST | SDXC_CTRL_FIFORST | SDXC_CTRL_CTRLRST))
		;

	// Set Power Enable
	psdmmc_reg->PWREN = 0x1 << 0;
	// Data Timeout = 0xFFFFFF, Response Timeout = 0x64
	psdmmc_reg->TMOUT = (0xFFFFFFU << 8) | (0x64 << 0);
	// Data Bus Width : 0(1-bit), 1(4-bit)
	psdmmc_reg->CTYPE = 0;
	// Block size
	psdmmc_reg->BLKSIZ = BLOCK_LENGTH;
	// Issue when RX FIFO Count >= 8 x 4 bytes & TX FIFO Count <= 8 x 4 bytes
	psdmmc_reg->FIFOTH = ((8 - 1) << 16) |		// Rx threshold
	        (8 << 0);				// Tx threshold
	// Mask & Clear All interrupts
	psdmmc_reg->INTMASK = 0;
	psdmmc_reg->RINTSTS = 0xFFFFFFFF;
	return TRUE;
}

static bool_t update_clock(struct sdmmc_register_set * psdmmc_reg)
{
	// Start a command with SDXC_CMDFLAG_UPDATECLKONLY flag.
	while(1)
	{
		psdmmc_reg->CMD = 0 | SDXC_CMDFLAG_STARTCMD | SDXC_CMDFLAG_UPDATECLKONLY | SDXC_CMDFLAG_STOPABORT;

		// Wait until a update clock command is taken by the SDXC module.
		//	If a HLE is occurred, repeat.
		volatile u32_t timeout = 0;
		while(psdmmc_reg->CMD & SDXC_CMDFLAG_STARTCMD)
		{
			if(++timeout > SDMMC_TIMEOUT)
			{
				LOG("sdmmc_setclock : ERROR - Time-out to update clock."); INFINTE_LOOP();
				return FALSE;
			}
		}

		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_HLE)
		{
			INFINTE_LOOP();
			psdmmc_reg->RINTSTS = SDXC_RINTSTS_HLE;
		}
		else
			break;
	}
	return TRUE;
}

static bool_t sdmmc_setclock(struct sdhci_xl00500_pdata_t *pdat, bool_t enb, u32_t nFreq)
{
	volatile u32_t timeout;
	struct sdmmc_register_set * const psdmmc_reg = (struct sdmmc_register_set *)pdat->virt;
	struct clkgen_register_set * const psd_clkgen_reg = get_clkgen_register_set(pdat->port);
	clkinfo clk_info;

	// 1. Confirm that no card is engaged in any transaction.
	//	If there's a transaction, wait until it has been finished.

#if defined(VERBOSE)
	if(psdmmc_reg->STATUS & (SDXC_STATUS_DATABUSY | SDXC_STATUS_FSMBUSY))
	{
		if(psdmmc_reg->STATUS & SDXC_STATUS_DATABUSY)
			LOG("sdmmc_setclock : ERROR - Data is busy");

		if(psdmmc_reg->STATUS & SDXC_STATUS_FSMBUSY)
			LOG("sdmmc_setclock : ERROR - Data Transfer is busy");

		timeout = SDMMC_TIMEOUT;
		while(timeout--)
		{
			if(!(psdmmc_reg->STATUS & (SDXC_STATUS_DATABUSY | SDXC_STATUS_FSMBUSY)))
				break;
		}
		if(timeout == 0)
		{
			INFINTE_LOOP();
		}
	}
#endif

	// 2. Disable the output clock.
	psdmmc_reg->CLKENA &= ~SDXC_CLKENA_CLKENB;
	psdmmc_reg->CLKENA |= SDXC_CLKENA_LOWPWR;		// low power mode & clock disable

	clk_info.pllnum = get_pllnum(pdat->clk_name);
	clk_info.freq = nFreq;

	if(sdmmc_getclkparam(pdat->clk_name, &clk_info) == FALSE)
	{
		LOG("failed to get clock param.");
		return FALSE;
	}

	psd_clkgen_reg->CLKGEN = (psd_clkgen_reg->CLKGEN & ~(0x7 << 2 | 0xFF << 5)) | (clk_info.pllnum << 2)			// set clock source
	        | ((clk_info.clkgendiv - 1) << 5)	// set clock divisor
	        | (0UL << 1);						// set clock invert
	psdmmc_reg->CLKDIV = (clk_info.clkdiv >> 1);	//  2*n divider (0 : bypass)
	psd_clkgen_reg->CLKENB |= 0x1UL << 2;			// clock generation enable
	psdmmc_reg->CLKENA &= ~SDXC_CLKENA_LOWPWR;		// normal power mode

	if(FALSE == update_clock(psdmmc_reg))
		return FALSE;
	if(FALSE == enb)
		return TRUE;

	psdmmc_reg->CLKENA |= SDXC_CLKENA_CLKENB;		// Enable the output clock.

	if(FALSE == update_clock(psdmmc_reg))
		return FALSE;

	return TRUE;
}

static bool_t __transfer_command(struct sdhci_xl00500_pdata_t *pdat, struct sdhci_cmd_t *cmd)
{
	u32_t cmdidx;
	u32_t status = 0;
	volatile u32_t timeout;
	struct sdmmc_register_set * const psdmmc_reg = (struct sdmmc_register_set *)pdat->virt;

	psdmmc_reg->RINTSTS = 0xFFFFFFFF;

	cmd->cmdidx &= 0xff;
	cmdidx = (cmd->cmdidx) | SDXC_CMDFLAG_STARTCMD;

	if(cmd->resptype)
	{
		if(cmd->resptype & MMC_RSP_136)
			cmdidx |= SDXC_CMDFLAG_LONGRSP;
		else
			cmdidx |= SDXC_CMDFLAG_SHORTRSP;
		if(cmd->resptype & MMC_RSP_CRC)
			cmdidx |= SDXC_CMDFLAG_CHKRSPCRC;
	}

	//TODO:To Abort boot mode, this bit of SDXC_CMDFLAG_STOPABORT should be set along with CMD[26] = disable_boot.
	if(cmd->cmdidx == MMC_GO_IDLE_STATE)
	{
		cmdidx |= SDXC_CMDFLAG_STOPABORT | SDXC_CMDFLAG_SENDINIT;
	}
	else if(cmd->cmdidx == MMC_SEND_STATUS)
	{
		cmdidx |= SDXC_CMDFLAG_STOPABORT;
	}
	else if(cmd->cmdidx == MMC_STOP_TRANSMISSION)
	{
		cmdidx |= SDXC_CMDFLAG_STOPABORT;
	}
	else
	{
		if(cmd->cmdidx == MMC_READ_SINGLE_BLOCK || cmd->cmdidx == MMC_READ_MULTIPLE_BLOCK)
		{
			cmdidx |= SDXC_CMDFLAG_BLOCK | SDXC_CMDFLAG_RXDATA;
		}
		else if(cmd->cmdidx == MMC_WRITE_SINGLE_BLOCK || cmd->cmdidx == MMC_WRITE_MULTIPLE_BLOCK)
		{
			cmdidx |= SDXC_CMDFLAG_BLOCK | SDXC_CMDFLAG_TXDATA;
		}

		// for emmc
		if(cmd->cmdidx == MMC_SEND_EXT_CSD && cmd->resptype == MMC_RSP_R1)
			cmdidx |= SDXC_CMDFLAG_BLOCK | SDXC_CMDFLAG_RXDATA;

		cmdidx |= SDXC_CMDFLAG_WAITPRVDAT;
	}
	// Send Command
	timeout = 0;
	do
	{
		psdmmc_reg->RINTSTS = SDXC_RINTSTS_HLE;
		psdmmc_reg->CMDARG = cmd->cmdarg;
		psdmmc_reg->CMD = cmdidx | SDXC_CMDFLAG_USE_HOLD_REG;

		while(psdmmc_reg->CMD & SDXC_CMDFLAG_STARTCMD)
		{
			if(++timeout > SDMMC_TIMEOUT)
			{
				LOG("__transfer_command : ERROR - Time-Out to send command.");
				status |= SDMMC_STATUS_CMDBUSY;
				INFINTE_LOOP();
				goto end;
			}
		}
	}while(psdmmc_reg->RINTSTS & SDXC_RINTSTS_HLE);

//--------------------------------------------------------------------------
	// Wait until Command sent to card and got response from card.
	timeout = 0;
	while(1)
	{
		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_CD)
			break;

		if(++timeout > SDMMC_TIMEOUT)
		{
			LOG("__transfer_command : ERROR - Time-Out to wait command done.");
			status |= SDMMC_STATUS_CMDTOUT;
			INFINTE_LOOP();
			goto end;
		}

		if((cmdidx & SDXC_CMDFLAG_STOPABORT) && (psdmmc_reg->RINTSTS & SDXC_RINTSTS_HTO))
		{
			// You have to clear FIFO when HTO is occurred.
			// After that, SDXC module leaves in stopped state and takes next command.
			while(0 == (psdmmc_reg->STATUS & SDXC_STATUS_FIFOEMPTY))
			{
				psdmmc_reg->DATA;
			}
		}
	}

	// Check Response Error
	if(psdmmc_reg->RINTSTS & (SDXC_RINTSTS_RCRC | SDXC_RINTSTS_RE | SDXC_RINTSTS_RTO))
	{
		if((psdmmc_reg->RINTSTS & SDXC_RINTSTS_RCRC) && (cmd->resptype & MMC_RSP_CRC))
			status |= SDMMC_STATUS_RESCRCFAIL;
		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_RE)
			status |= SDMMC_STATUS_RESERROR;
		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_RTO)
			status |= SDMMC_STATUS_RESTOUT;
	}

	if((status == SDMMC_STATUS_NOERROR) && (cmd->resptype & MMC_RSP_PRESENT))
	{
		cmd->response[0] = psdmmc_reg->RESP0;
		if(cmd->resptype & MMC_RSP_136)
		{
			cmd->response[1] = psdmmc_reg->RESP1;
			cmd->response[2] = psdmmc_reg->RESP2;
			cmd->response[3] = psdmmc_reg->RESP3;
		}

		if(cmd->resptype == MMC_RSP_R1B)
		{
			timeout = 0;
			do
			{
				if(++timeout > SDMMC_TIMEOUT)
				{
					LOG("__transfer_command : ERROR - Time-Out to wait card data is ready.");
					status |= SDMMC_STATUS_DATABUSY;
					INFINTE_LOOP();
					goto end;
				}
			}while(psdmmc_reg->STATUS & SDXC_STATUS_DATABUSY);
		}
	}

	end:

#if defined(VERBOSE)
	if(SDMMC_STATUS_NOERROR != status)
		LOG("__transfer_command Failed : command(%d), argument(0x%08X) => status(0x%08X)", cmd->cmdidx, cmd->cmdarg, status);
#endif

	return status;
}

static bool_t xl00500_transfer_command(struct sdhci_xl00500_pdata_t *pdat, struct sdhci_cmd_t *cmd)
{
	u32_t status = __transfer_command(pdat, cmd);
	return SDMMC_STATUS_NOERROR == status;
}

static bool_t read_bytes(struct sdhci_xl00500_pdata_t *pdat, u32_t *buf, u32_t blkcount, u32_t blksize)
{
	u32_t *tmp = buf;
	s64_t count = blkcount * blksize;

	struct sdmmc_register_set * const psdmmc_reg = (struct sdmmc_register_set *)pdat->virt;
	assert(0 == ((u32_t )buf & 3));
	assert(0 == (count % 32));

	while(0 < count)
	{
		if(psdmmc_reg->RINTSTS & (SDXC_RINTSTS_RXDR | SDXC_RINTSTS_DTO))
		{
			u32_t FSize, Timeout = SDMMC_TIMEOUT;
			while((psdmmc_reg->STATUS & SDXC_STATUS_FIFOEMPTY) && Timeout--)
				;
			if(0 == Timeout)
				break;
			FSize = (psdmmc_reg->STATUS & SDXC_STATUS_FIFOCOUNT) >> 17;
			while(FSize > 0 && count > 0)
			{
				*tmp++ = psdmmc_reg->DATA;
				FSize--;
				count -= 4;
			}

			psdmmc_reg->RINTSTS = SDXC_RINTSTS_RXDR;
		}

		// Check Errors
		if(psdmmc_reg->RINTSTS & (SDXC_RINTSTS_DRTO | SDXC_RINTSTS_EBE | SDXC_RINTSTS_SBE | SDXC_RINTSTS_DCRC))
		{
#if defined(VERBOSE)
			LOG("Read left = %d", count);

			if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_DRTO)
				LOG("ERROR : read_bytes - SDXC_RINTSTS_DRTO");
			if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_EBE)
				LOG("ERROR : read_bytes - SDXC_RINTSTS_EBE");
			if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_SBE)
				LOG("ERROR : read_bytes - SDXC_RINTSTS_SBE");
			if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_DCRC)
				LOG("ERROR : read_bytes - SDXC_RINTSTS_DCRC");
#endif

			return FALSE;
		}

		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_DTO)
		{
			if(count == 0)
			{
				psdmmc_reg->RINTSTS = SDXC_RINTSTS_DTO;
				break;
			}
		}

		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_HTO)
		{
#if defined(VERBOSE)
			LOG("ERROR : read_bytes - SDXC_RINTSTS_HTO");
#endif
			psdmmc_reg->RINTSTS = SDXC_RINTSTS_HTO;
		}

		assert(!(psdmmc_reg->RINTSTS & SDXC_RINTSTS_FRUN));
	}

	psdmmc_reg->RINTSTS = SDXC_RINTSTS_DTO;

	assert(count == 0);
	assert(psdmmc_reg->STATUS & SDXC_STATUS_FIFOEMPTY);
	assert(!(psdmmc_reg->STATUS & SDXC_STATUS_FIFOFULL));
	assert(!(psdmmc_reg->STATUS & SDXC_STATUS_FIFOCOUNT));

	if(blkcount > 1)
		while(psdmmc_reg->STATUS & SDXC_STATUS_FSMBUSY)
			;
	return TRUE;
}

static bool_t write_bytes(struct sdhci_xl00500_pdata_t *pdat, u32_t *buf, u32_t blkcount, u32_t blksize)
{
	u32_t *tmp = buf;
	s64_t count = blkcount * blksize;

	struct sdmmc_register_set * const psdmmc_reg = (struct sdmmc_register_set *)pdat->virt;
	assert(0 == ((u32_t )buf & 3));
	assert(0 == (count % 32));

	psdmmc_reg->RINTSTS = SDXC_RINTSTS_CD;

	while(0 < count)
	{
		if(psdmmc_reg->RINTSTS & (SDXC_RINTSTS_TXDR))
		{
			u32_t FSize;
			FSize = 32 / 4;
			while(FSize > 0 && count > 0)
			{
				psdmmc_reg->DATA = *tmp++;
				FSize--;
				count -= 4;
			}
			psdmmc_reg->RINTSTS = SDXC_RINTSTS_TXDR;
		}

		// Check Errors
		if(psdmmc_reg->RINTSTS & (SDXC_RINTSTS_DRTO | SDXC_RINTSTS_EBE | SDXC_RINTSTS_SBE | SDXC_RINTSTS_DCRC))
		{
#if defined(VERBOSE)
			LOG("Write left = %d", count);

			if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_DRTO)
				LOG("ERROR : write_bytes - SDXC_RINTSTS_DRTO");
			if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_EBE)
				LOG("ERROR : write_bytes - SDXC_RINTSTS_EBE");
			if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_SBE)
				LOG("ERROR : write_bytes - SDXC_RINTSTS_SBE");
			if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_DCRC)
				LOG("ERROR : write_bytes - SDXC_RINTSTS_DCRC");
#endif
			return FALSE;
		}

		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_HTO)
		{
#if defined(VERBOSE)
			LOG("ERROR : write_bytes - SDXC_RINTSTS_HTO");
#endif
			psdmmc_reg->RINTSTS = SDXC_RINTSTS_HTO;
		}

		assert(!(psdmmc_reg->RINTSTS & SDXC_RINTSTS_FRUN));
	}

	u32_t TimeOut = 0;
	while(!(psdmmc_reg->RINTSTS & SDXC_RINTSTS_DTO))
	{
		if(++TimeOut > SDMMC_TIMEOUT)
		{
			LOG("write_bytes : ERROR - TIme-out to wait DTO.\r\n"); INFINTE_LOOP();
			break;
		}
	}
	psdmmc_reg->RINTSTS = SDXC_RINTSTS_DTO;
	assert(count == 0);

	// Check Errors
	if(psdmmc_reg->RINTSTS & (SDXC_RINTSTS_DRTO | SDXC_RINTSTS_EBE | SDXC_RINTSTS_SBE | SDXC_RINTSTS_DCRC))
	{
#if defined(VERBOSE)
		LOG("Write left = %d", count);

		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_DRTO)
			LOG("ERROR : write_bytes - SDXC_RINTSTS_DRTO");
		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_EBE)
			LOG("ERROR : write_bytes - SDXC_RINTSTS_EBE");
		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_SBE)
			LOG("ERROR : write_bytes - SDXC_RINTSTS_SBE");
		if(psdmmc_reg->RINTSTS & SDXC_RINTSTS_DCRC)
			LOG("ERROR : write_bytes - SDXC_RINTSTS_DCRC");
#endif
		return FALSE;
	}
	//LOG("EMPTY=%d,FULL=%d,COUNT=%d",!!(psdmmc_reg->STATUS & SDXC_STATUS_FIFOEMPTY),
	//   !!(psdmmc_reg->STATUS & SDXC_STATUS_FIFOFULL),
	//  (psdmmc_reg->STATUS & SDXC_STATUS_FIFOCOUNT)>>17);
	assert(psdmmc_reg->STATUS & SDXC_STATUS_FIFOEMPTY);
	assert(!(psdmmc_reg->STATUS & SDXC_STATUS_FIFOFULL));
	assert(!(psdmmc_reg->STATUS & SDXC_STATUS_FIFOCOUNT));

	if(blkcount > 1)
		while(psdmmc_reg->STATUS & SDXC_STATUS_FSMBUSY)
			;
	return TRUE;
}

static bool_t xl00500_transfer_data(struct sdhci_xl00500_pdata_t *pdat, struct sdhci_cmd_t *cmd, struct sdhci_data_t *dat)
{
	bool_t ret = FALSE;
	struct sdmmc_register_set * const psdmmc_reg = (struct sdmmc_register_set *)pdat->virt;

	while(psdmmc_reg->STATUS & SDXC_STATUS_FSMBUSY || psdmmc_reg->STATUS & SDXC_STATUS_DATABUSY)
		;

	if(dat->flag & MMC_DATA_READ)
	{
		psdmmc_reg->BYTCNT = dat->blkcnt * dat->blksz;
		if(!xl00500_transfer_command(pdat, cmd))
			return FALSE;
		ret = read_bytes(pdat, (u32_t *)dat->buf, dat->blkcnt, dat->blksz);
	}
	else if(dat->flag & MMC_DATA_WRITE)
	{
		psdmmc_reg->BYTCNT = dat->blkcnt * dat->blksz;
		if(!xl00500_transfer_command(pdat, cmd))
			return FALSE;
		ret = write_bytes(pdat, (u32_t *)dat->buf, dat->blkcnt, dat->blksz);
	}
	return ret;
}

static bool_t sdhci_xl00500_detect(struct sdhci_t *sdhci)
{
	struct sdhci_xl00500_pdata_t *pdat = (struct sdhci_xl00500_pdata_t *)(sdhci->priv);
	if(!pdat)
		return FALSE;
	struct gpio_desc cd_gpio = gpios[pdat->port].CD;

	if(sdhci->removable == FALSE)
		return TRUE;

	if(gpio_get_value(cd_gpio.no) == 0)
		return TRUE;
	else
		return FALSE;
}

static bool_t sdhci_xl00500_setwidth(struct sdhci_t *sdhci, u32_t width)
{
	struct sdhci_xl00500_pdata_t *pdat = (struct sdhci_xl00500_pdata_t *)(sdhci->priv);

	if(!pdat)
		return FALSE;

	struct sdmmc_register_set * const psdmmc_reg = (struct sdmmc_register_set *)pdat->virt;

	// Data Bus Width : 0(1-bit), 1(4-bit)
	if(width == MMC_BUS_WIDTH_1)
		psdmmc_reg->CTYPE = 0;
	else if(width == MMC_BUS_WIDTH_4)
		psdmmc_reg->CTYPE = 4;
	else
		return FALSE;

	return TRUE;
}

static bool_t sdhci_xl00500_setclock(struct sdhci_t *sdhci, u32_t clock)
{
	struct sdhci_xl00500_pdata_t *pdat = (struct sdhci_xl00500_pdata_t *)(sdhci->priv);
	if(!pdat)
		return FALSE;
	return sdmmc_setclock(pdat, TRUE, clock);
}

static bool_t sdhci_xl00500_transfer(struct sdhci_t *sdhci, struct sdhci_cmd_t *cmd, struct sdhci_data_t *dat)
{
	struct sdhci_xl00500_pdata_t *pdat = (struct sdhci_xl00500_pdata_t *)sdhci->priv;

	if(!dat)
		return xl00500_transfer_command(pdat, cmd);
	return xl00500_transfer_data(pdat, cmd, dat);
}

static struct device_t *sdhci_xl00500_probe(struct driver_t *drv, struct dtnode_t *n)
{
	struct sdhci_xl00500_pdata_t *pdat;
	struct sdhci_t *sdhci;
	struct device_t *dev;
	virtual_addr_t virt;
	char *clk_name;
	int port;
	physical_addr_t phy = dt_read_address(n);

	if(phy == 0xc0062000)
		port = 0;
	else if(phy == 0xc0068000)
		port = 1;
	else if(phy == 0xc0069000)
		port = 2;
	else
		return FALSE;

	virt = phys_to_virt(phy);

	clk_name = dt_read_string(n, "clock-name", NULL);
	if(!clk_name)
		return FALSE;

	pdat = malloc(sizeof(struct sdhci_xl00500_pdata_t));
	if(!pdat)
		return FALSE;

	sdhci = malloc(sizeof(struct sdhci_t));
	if(!sdhci)
	{
		free(pdat);
		return FALSE;
	}

	pdat->virt = virt;
	pdat->port = port;
	pdat->clk_name = strdup(clk_name);

	sdhci->name = alloc_device_name(dt_read_name(n), -1);
	sdhci->voltage = MMC_VDD_27_36;
	sdhci->width = MMC_BUS_WIDTH_4;
	sdhci->clock = 25 * 1000 * 1000;
	sdhci->removable = dt_read_bool(n, "removable", TRUE);
	sdhci->detect = sdhci_xl00500_detect;
	sdhci->setwidth = sdhci_xl00500_setwidth;
	sdhci->setclock = sdhci_xl00500_setclock;
	sdhci->transfer = sdhci_xl00500_transfer;
	sdhci->priv = pdat;
	sdhci->isspi = FALSE;

	sdhci_xl00500_gpio_init(pdat);

	if(!sdhci_xl00500_clk_init(pdat))
		goto err;

	if(!register_sdhci(&dev, sdhci))
		goto err;

	dev->driver = drv;
	return dev;

	err: free_device_name(sdhci->name);
	free(sdhci->priv);
	free(sdhci);
	return NULL;
}

static void sdhci_xl00500_remove(struct device_t *dev)
{
	struct sdhci_t *sdhci = (struct sdhci_t *)dev->priv;

	if(sdhci && unregister_sdhci(sdhci))
	{
		free_device_name(sdhci->name);
		free(sdhci->priv);
		free(sdhci);
	}
}

static void sdhci_xl00500_suspend(struct device_t *dev)
{
}

static void sdhci_xl00500_resume(struct device_t *dev)
{
}

static struct driver_t sdhci_xl00500 = {
	.name = "sdhci-xl00500",
	.probe = sdhci_xl00500_probe,
	.remove = sdhci_xl00500_remove,
	.suspend = sdhci_xl00500_suspend,
	.resume = sdhci_xl00500_resume,
};

static __init void sdhci_xl00500_driver_init(void)
{
	register_driver(&sdhci_xl00500);
}

static __exit void sdhci_xl00500_driver_exit(void)
{
	unregister_driver(&sdhci_xl00500);
}

driver_initcall(sdhci_xl00500_driver_init);
driver_exitcall(sdhci_xl00500_driver_exit);
