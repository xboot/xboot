/*
 * kernel/partition/msdos.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <malloc.h>
#include <vsprintf.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/disk.h>
#include <xboot/partition.h>


static x_bool parser_probe_msdos(struct disk * disk)
{
	x_u8 * buf;

	if(!disk || !disk->name)
		return FALSE;

	if((disk->sector_size <= 0) || (disk->sector_count <=0))
		return FALSE;

	if((!disk->read_sector) || (!disk->write_sector))
		return FALSE;

	buf = malloc(disk->sector_size);
	if(!buf)
		return FALSE;

	//TODO

	free(buf);
	return TRUE;
}

/*
 * msdos partition parser
 */
static struct partition_parser msdos_partition_parser = {
	.name		= "msdos",
	.probe		= parser_probe_msdos,
};

static __init void partition_parser_msdos_init(void)
{
	if(!register_partition_parser(&msdos_partition_parser))
		LOG_E("register 'msdos' partition parser fail");
}

static __exit void partition_parser_msdos_exit(void)
{
	if(!unregister_partition_parser(&msdos_partition_parser))
		LOG_E("unregister 'msdos' partition parser fail");
}

module_init(partition_parser_msdos_init, LEVEL_POSTCORE);
module_exit(partition_parser_msdos_exit, LEVEL_POSTCORE);
