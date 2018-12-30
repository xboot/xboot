/*
 * driver/sdhci-v3s.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

struct sdhci_v3s_pdata_t {
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

static bool_t v3s_transfer_command(struct sdhci_v3s_pdata_t * pdat, struct sdhci_cmd_t * cmd)
{
	return TRUE;
}

static bool_t v3s_transfer_data(struct sdhci_v3s_pdata_t * pdat, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	return TRUE;
}

static bool_t sdhci_v3s_detect(struct sdhci_t * sdhci)
{
	return FALSE;
}

static bool_t sdhci_v3s_setvoltage(struct sdhci_t * sdhci, u32_t voltage)
{
	return TRUE;
}

static bool_t sdhci_v3s_setwidth(struct sdhci_t * sdhci, u32_t width)
{
	return TRUE;
}

static bool_t sdhci_v3s_setclock(struct sdhci_t * sdhci, u32_t clock)
{
	return TRUE;
}

static bool_t sdhci_v3s_transfer(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	struct sdhci_v3s_pdata_t * pdat = (struct sdhci_v3s_pdata_t *)sdhci->priv;

	if(!dat)
		return v3s_transfer_command(pdat, cmd);
	return v3s_transfer_data(pdat, cmd, dat);
}

static struct device_t * sdhci_v3s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct sdhci_v3s_pdata_t * pdat;
	struct sdhci_t * sdhci;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * pclk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(pclk))
		return NULL;

	pdat = malloc(sizeof(struct sdhci_v3s_pdata_t));
	if(!pdat)
		return FALSE;

	sdhci = malloc(sizeof(struct sdhci_t));
	if(!sdhci)
	{
		free(pdat);
		return FALSE;
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
	sdhci->clock = 52 * 1000 * 1000;
	sdhci->removable = TRUE;
	sdhci->detect = sdhci_v3s_detect;
	sdhci->setvoltage = sdhci_v3s_setvoltage;
	sdhci->setwidth = sdhci_v3s_setwidth;
	sdhci->setclock = sdhci_v3s_setclock;
	sdhci->transfer = sdhci_v3s_transfer;
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

	if(!register_sdhci(&dev, sdhci))
	{
		clk_disable(pdat->pclk);
		free(pdat->pclk);

		free_device_name(sdhci->name);
		free(sdhci->priv);
		free(sdhci);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void sdhci_v3s_remove(struct device_t * dev)
{
	struct sdhci_t * sdhci = (struct sdhci_t *)dev->priv;
	struct sdhci_v3s_pdata_t * pdat = (struct sdhci_v3s_pdata_t *)sdhci->priv;

	if(sdhci && unregister_sdhci(sdhci))
	{
		clk_disable(pdat->pclk);
		free(pdat->pclk);

		free_device_name(sdhci->name);
		free(sdhci->priv);
		free(sdhci);
	}
}

static void sdhci_v3s_suspend(struct device_t * dev)
{
}

static void sdhci_v3s_resume(struct device_t * dev)
{
}

static struct driver_t sdhci_v3s = {
	.name		= "sdhci-v3s",
	.probe		= sdhci_v3s_probe,
	.remove		= sdhci_v3s_remove,
	.suspend	= sdhci_v3s_suspend,
	.resume		= sdhci_v3s_resume,
};

static __init void sdhci_v3s_driver_init(void)
{
	register_driver(&sdhci_v3s);
}

static __exit void sdhci_v3s_driver_exit(void)
{
	unregister_driver(&sdhci_v3s);
}

driver_initcall(sdhci_v3s_driver_init);
driver_exitcall(sdhci_v3s_driver_exit);
