/*
 * driver/bcm2836-wdog.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

#include <bcm2836-wdog.h>

struct bcm2836_wdog_pdata_t {
	virtual_addr_t virt;
	int start;
};

static void wdog_init(struct watchdog_t * watchdog)
{
}

static void wdog_exit(struct watchdog_t * watchdog)
{
}

static void wdog_set(struct watchdog_t * watchdog, int timeout)
{
	struct bcm2836_wdog_pdata_t * pdat = (struct bcm2836_wdog_pdata_t *)watchdog->priv;
	u32_t val;

	if(timeout < 0)
		timeout = 0;
	else if(timeout > 15)
		timeout = 15;

	if(timeout > 0)
	{
		write32(pdat->virt + PM_WDOG, PM_PASSWORD | ((timeout << 16) & PM_WDOG_TIME_SET));
		val = read32(pdat->virt + PM_RSTC);
		write32(pdat->virt + PM_RSTC, PM_PASSWORD | (val & PM_RSTC_WRCFG_CLR) | PM_RSTC_WRCFG_FULL_RESET);
		pdat->start = 1;
	}
	else
	{
		write32(pdat->virt + PM_RSTC, PM_PASSWORD | PM_RSTC_RESET);
		pdat->start = 0;
	}
}

static int wdog_get(struct watchdog_t * watchdog)
{
	struct bcm2836_wdog_pdata_t * pdat = (struct bcm2836_wdog_pdata_t *)watchdog->priv;
	return (pdat->start != 0) ? ((read32(pdat->virt + PM_WDOG) & PM_WDOG_TIME_SET) >> 16) : 0;
}

static void wdog_suspend(struct watchdog_t * watchdog)
{
}

static void wdog_resume(struct watchdog_t * watchdog)
{
}

static bool_t bcm2836_register_wdog(struct resource_t * res)
{
	struct bcm2836_wdog_data_t * rdat = (struct bcm2836_wdog_data_t *)res->data;
	struct bcm2836_wdog_pdata_t * pdat;
	struct watchdog_t * wdog;
	char name[64];

	pdat = malloc(sizeof(struct bcm2836_wdog_pdata_t));
	if(!pdat)
		return FALSE;

	wdog = malloc(sizeof(struct watchdog_t));
	if(!wdog)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->virt = phys_to_virt(rdat->phys);
	pdat->start = 0;
	wdog->name = strdup(name);
	wdog->init = wdog_init;
	wdog->exit = wdog_exit;
	wdog->set = wdog_set,
	wdog->get = wdog_get,
	wdog->suspend = wdog_suspend,
	wdog->resume = wdog_resume,
	wdog->priv = pdat;

	if(register_watchdog(wdog))
		return TRUE;

	free(wdog->priv);
	free(wdog->name);
	free(wdog);
	return FALSE;
}

static bool_t bcm2836_unregister_wdog(struct resource_t * res)
{
	struct watchdog_t * wdog;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	wdog = search_watchdog(name);
	if(!wdog)
		return FALSE;

	if(!unregister_watchdog(wdog))
		return FALSE;

	free(wdog->priv);
	free(wdog->name);
	free(wdog);
	return TRUE;
}

static __init void bcm2836_watchdog_init(void)
{
	resource_for_each("bcm2836-wdog", bcm2836_register_wdog);
}

static __exit void bcm2836_watchdog_exit(void)
{
	resource_for_each("bcm2836-wdog", bcm2836_unregister_wdog);
}

device_initcall(bcm2836_watchdog_init);
device_exitcall(bcm2836_watchdog_exit);
