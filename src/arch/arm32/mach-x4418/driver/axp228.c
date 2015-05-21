/*
 * driver/axp228.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

#include <axp228.h>

struct axp228_private_data_t {
	struct i2c_client_t * client;
	struct axp228_data_t * rdat;
};

static bool_t axp228_read_reg(struct i2c_client_t * client, u8_t reg, u8_t * val)
{
	struct i2c_msg_t msgs[2];
    u8_t buf;

    buf = reg;
    msgs[0].addr = client->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &buf;

    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = &buf;

    if(i2c_transfer(client->i2c, msgs, 2) != 2)
    	return FALSE;

    if(val)
    	*val = buf;
    return TRUE;
}

static bool_t axp228_read(struct i2c_client_t * client, u8_t * buf, u8_t len)
{
	struct i2c_msg_t msgs[2];
	u8_t mbuf = 0;

	msgs[0].flags = 0;
	msgs[0].addr = client->addr;
	msgs[0].len = 1;
	msgs[0].buf = &mbuf;

	msgs[1].flags = I2C_M_RD;
	msgs[1].addr = client->addr;
	msgs[1].len = len;
	msgs[1].buf = buf;

    if(i2c_transfer(client->i2c, msgs, 2) != 2)
    	return FALSE;
    return TRUE;
}

static bool_t axp228_update(struct battery_t * bat, struct battery_info_t * info)
{
	struct axp228_private_data_t * dat = (struct axp228_private_data_t *)bat->priv;
	struct axp228_data_t * rdat = (struct axp228_data_t *)dat->rdat;
	u8_t reg = 0x0, val = 0x0;

	LOG("axp228_update ...");
	axp228_read_reg(dat->client, reg, &val);
	LOG("[%d] = 0x%02x",reg, val);

	return TRUE;
}

static void axp228_suspend(struct battery_t * bat)
{
}

static void axp228_resume(struct battery_t * bat)
{
}

static bool_t register_pmic_axp228(struct resource_t * res)
{
	struct axp228_data_t * rdat = (struct axp228_data_t *)res->data;
	struct axp228_private_data_t * dat;
	struct battery_t * bat;
	struct i2c_client_t * client;
	char name[64];

	client = i2c_client_alloc(rdat->i2cbus, rdat->addr, 0);
	if(!client)
		return FALSE;

	dat = malloc(sizeof(struct axp228_private_data_t));
	if(!dat)
	{
		i2c_client_free(client);
		return FALSE;
	}

	bat = malloc(sizeof(struct battery_t));
	if(!bat)
	{
		i2c_client_free(client);
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	memset(dat, 0xff, sizeof(struct axp228_private_data_t));
	dat->client = client;
	dat->rdat = rdat;

	bat->name = strdup(name);
	bat->update = axp228_update;
	bat->suspend = axp228_suspend;
	bat->resume = axp228_resume;
	bat->priv = dat;

	if(register_battery(bat))
		return TRUE;

	i2c_client_free(client);
	free(bat->priv);
	free(bat->name);
	free(bat);
	return FALSE;
}

static bool_t unregister_pmic_axp228(struct resource_t * res)
{
	struct battery_t * bat;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	bat = search_battery(name);
	if(!bat)
		return FALSE;

	if(!unregister_battery(bat))
		return FALSE;

	i2c_client_free(((struct axp228_private_data_t *)(bat->priv))->client);
	free(bat->priv);
	free(bat->name);
	free(bat);
	return TRUE;
}

static __init void pmic_axp228_init(void)
{
	resource_for_each_with_name("axp228", register_pmic_axp228);
}

static __exit void pmic_axp228_exit(void)
{
	resource_for_each_with_name("axp228", unregister_pmic_axp228);
}

device_initcall(pmic_axp228_init);
device_exitcall(pmic_axp228_exit);
