/*
 * kernel/command/cmd_probe.c
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <mtd/nand/nfc.h>
#include <mtd/nand/nand.h>
#include <mmc/mmc_host.h>
#include <mmc/mmc_card.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_PROBE) && (CONFIG_COMMAND_PROBE > 0)

extern struct nand_list * nand_list;
extern struct mmc_card_list * mmc_card_list;

static void usage(void)
{
	printk("usage:\r\n    probe <TYPE>\r\n");
}

static void list_nand_flash(void)
{
	struct nand_list * list;
	struct list_head * pos;

	for(pos = (&nand_list->entry)->next; pos != (&nand_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct nand_list, entry);

		printk(" \"%s\" - %s (%s)\r\n", list->nand->name, list->nand->info->name, list->nand->manufacturer->name);
	}
}

static void list_mmc_card(void)
{
	struct mmc_card_list * list;
	struct list_head * pos;
	char buff[32];

	for(pos = (&mmc_card_list->entry)->next; pos != (&mmc_card_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct mmc_card_list, entry);

		ssize(buff, (u64_t)(list->card->info->capacity));
		printk(" \"%s\" - %s (%s)\r\n", list->card->name, list->card->info->cid.pnm, buff);
	}
}

static int probe(int argc, char ** argv)
{
	s32_t i;

	if(argc < 2)
	{
		usage();
		return -1;
	}

	for(i=1; i<argc; i++)
	{
		if( !strcmp((const char *)argv[i], "nor"))
		{

		}
		else if( !strcmp((const char *)argv[i], "nand"))
		{
			nand_flash_probe();
			list_nand_flash();
		}
		else if( !strcmp((const char *)argv[i], "mmc"))
		{
			mmc_card_probe();
			list_mmc_card();
		}
		else
		{
			printk("do not probe device with the type of '%s'\r\n", argv[i]);
		}
	}

	return 0;
}

static struct command probe_cmd = {
	.name		= "probe",
	.func		= probe,
	.desc		= "probe the special device\r\n",
	.usage		= "probe <TYPE>\r\n",
	.help		= "    probe the special device, for the removable device\r\n"
				  "    the TYPE as below for details\r\n"
				  "    nor     probe nor flash\r\n"
				  "    nand    probe nand flash\r\n"
				  "    mmc     probe mmc card\r\n"
};

static __init void probe_cmd_init(void)
{
	if(!command_register(&probe_cmd))
		LOG_E("register 'probe' command fail");
}

static __exit void probe_cmd_exit(void)
{
	if(!command_unregister(&probe_cmd))
		LOG_E("unregister 'probe' command fail");
}

command_initcall(probe_cmd_init);
command_exitcall(probe_cmd_exit);

#endif
