/*
 * kernel/partition/dos.c
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

extern x_bool disk_read(struct disk * disk, x_u8 * buf, x_u32 offset, x_u32 size);

/*
 * the partition entry
 */
struct dos_partition_entry
{
	/* if active is 0x80, otherwise is 0x00 */
	x_u8 flag;

	/* the head of the start */
	x_u8  start_head;

	/* the sector of the start */
	x_u8 start_sector;

	/* the cylinder of the end */
	x_u8 start_cylinder;

	/* the partition type */
	x_u8 type;

	/* the head of the end */
	x_u8  end_head;

	/* the sector of the end */
	x_u8 end_sector;

	/* the cylinder of the end */
	x_u8 end_cylinder;

	/* the start sector*/
	x_u8 start[4];

	/* the length in sector units */
	x_u8 length[4];

} __attribute__ ((packed));

/*
 * the structure of mbr
 */
struct dos_partition_mbr
{
	/*
	 * the code area (actually, including BPB)
	 */
	x_u8 code[446];

	/*
	 * four partition entries
	 */
	struct dos_partition_entry entry[4];

	/*
	 * the signature 0x55, 0xaa
	 */
	x_u8 signature[2];

} __attribute__ ((packed));


static x_bool dos_partition(struct disk * disk, x_u32 sector)
{
	return FALSE;
}

static x_bool parser_probe_dos(struct disk * disk)
{
	struct dos_partition_mbr * mbr;
//	x_u8 buffer[512];

	if(!disk || !disk->name)
		return FALSE;

	if((disk->sector_size <= 0) || (disk->sector_count <=0))
		return FALSE;

	if((!disk->read_sector) || (!disk->write_sector))
		return FALSE;

	mbr = malloc(sizeof(struct dos_partition_mbr));
	if(!mbr)
		return FALSE;

	if(!disk_read(disk, (x_u8 *)mbr, 0, sizeof(struct dos_partition_mbr)))
	{
		free(mbr);
		return FALSE;
	}

	/*
	 * check dos partition's signature
	 */
	if((mbr->signature[0] != 0x55) || mbr->signature[1] != 0xaa)
	{
		free(mbr);
		return FALSE;
	}
	LOG_E("flag = 0x%lx", mbr->entry[0].flag);

	LOG_E("flag = 0x%lx", mbr->entry[0].flag);
	LOG_E("start_head = 0x%lx", mbr->entry[0].start_head);
	LOG_E("start_sector = 0x%lx", mbr->entry[0].start_sector);
	LOG_E("start_cylinder = 0x%lx", mbr->entry[0].start_cylinder);
	LOG_E("type = 0x%lx", mbr->entry[0].type);
	LOG_E("end_head = 0x%lx", mbr->entry[0].end_head);
	LOG_E("end_sector = 0x%lx", mbr->entry[0].end_sector);
	LOG_E("end_cylinder = 0x%lx", mbr->entry[0].end_cylinder);
	LOG_E("0x%lx,0x%lx,0x%lx,0x%lx", mbr->entry[0].start[0],mbr->entry[0].start[1],mbr->entry[0].start[2],mbr->entry[0].start[3]);
	LOG_E("0x%lx,0x%lx,0x%lx,0x%lx", mbr->entry[0].length[0],mbr->entry[0].length[1],mbr->entry[0].length[2],mbr->entry[0].length[3]);

	LOG_E("----------------");

	LOG_E("flag = 0x%lx", mbr->entry[1].flag);
	LOG_E("start_head = 0x%lx", mbr->entry[1].start_head);
	LOG_E("start_sector = 0x%lx", mbr->entry[1].start_sector);
	LOG_E("start_cylinder = 0x%lx", mbr->entry[1].start_cylinder);
	LOG_E("type = 0x%lx", mbr->entry[1].type);
	LOG_E("end_head = 0x%lx", mbr->entry[1].end_head);
	LOG_E("end_sector = 0x%lx", mbr->entry[1].end_sector);
	LOG_E("end_cylinder = 0x%lx", mbr->entry[1].end_cylinder);
	LOG_E("0x%lx,0x%lx,0x%lx,0x%lx", mbr->entry[1].start[0],mbr->entry[1].start[1],mbr->entry[1].start[2],mbr->entry[1].start[3]);
	LOG_E("0x%lx,0x%lx,0x%lx,0x%lx", mbr->entry[1].length[0],mbr->entry[1].length[1],mbr->entry[1].length[2],mbr->entry[1].length[3]);

	free(mbr);

	//FIXME
	/* for test */
	struct partition * part;
	part = malloc(sizeof(struct partition));
	strlcpy((x_s8 *)part->name, (const x_s8 *)"part1", sizeof(part->name));
	part->sector_from = 0;
	//part->sector_to = 20480 - 1;
	part->sector_to = 2 - 1;
	part->sector_size = disk->sector_size;
	list_add_tail(&part->entry, &(disk->info.entry));

	part = malloc(sizeof(struct partition));
	strlcpy((x_s8 *)part->name, (const x_s8 *)"part2", sizeof(part->name));
	//part->sector_from = 20480;
	part->sector_from = 2;
	part->sector_to = disk->sector_count - 1;
	part->sector_size = disk->sector_size;
	list_add_tail(&part->entry, &(disk->info.entry));
	return TRUE;

	return FALSE;
}

/*
 * dos partition parser
 */
static struct partition_parser dos_partition_parser = {
	.name		= "dos",
	.probe		= parser_probe_dos,
};

static __init void partition_parser_dos_init(void)
{
	if(!register_partition_parser(&dos_partition_parser))
		LOG_E("register 'dos' partition parser fail");
}

static __exit void partition_parser_dos_exit(void)
{
	if(!unregister_partition_parser(&dos_partition_parser))
		LOG_E("unregister 'dos' partition parser fail");
}

module_init(partition_parser_dos_init, LEVEL_POSTCORE);
module_exit(partition_parser_dos_exit, LEVEL_POSTCORE);
