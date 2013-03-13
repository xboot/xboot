/*
 * drivers/disk/partition/dos.c
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
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <disk/disk.h>
#include <disk/partition.h>

extern loff_t disk_read(struct disk * disk, u8_t * buf, loff_t offset, loff_t count);

/*
 * the partition entry
 */
struct dos_partition_entry
{
	/* if active is 0x80, otherwise is 0x00 */
	u8_t flag;

	/* the head of the start */
	u8_t  start_head;

	/* the sector of the start */
	u8_t start_sector;

	/* the cylinder of the end */
	u8_t start_cylinder;

	/* the partition type */
	u8_t type;

	/* the head of the end */
	u8_t  end_head;

	/* the sector of the end */
	u8_t end_sector;

	/* the cylinder of the end */
	u8_t end_cylinder;

	/* the start sector*/
	u8_t start[4];

	/* the length in sector units */
	u8_t length[4];

} __attribute__ ((packed));

/*
 * the structure of mbr
 */
struct dos_partition_mbr
{
	/*
	 * the code area (actually, including BPB)
	 */
	u8_t code[446];

	/*
	 * four partition entries
	 */
	struct dos_partition_entry entry[4];

	/*
	 * the signature 0x55, 0xaa
	 */
	u8_t signature[2];

} __attribute__ ((packed));


static bool_t is_dos_extended(u8_t type)
{
	if((type == 0x5) || (type == 0xf) || (type == 0x85))
		return TRUE;
	return FALSE;
}

static bool_t dos_partition(struct disk * disk, size_t sector, size_t relative)
{
	struct dos_partition_mbr mbr;
	struct partition * part;
	size_t start;
	int i;

	if(!disk || !disk->name)
		return FALSE;

	if((disk->sector_size <= 0) || (disk->sector_count <=0))
		return FALSE;

	if((!disk->read_sectors) || (!disk->write_sectors))
		return FALSE;

	if(disk_read(disk, (u8_t *)(&mbr), (loff_t)(sector * disk->sector_size) , sizeof(struct dos_partition_mbr)) != sizeof(struct dos_partition_mbr))
		return FALSE;

	/*
	 * check dos partition's signature
	 */
	if((mbr.signature[0] != 0x55) || mbr.signature[1] != 0xaa)
		return FALSE;

	for(i=0; i<4; i++)
	{
		if((mbr.entry[i].type != 0) && (is_dos_extended(mbr.entry[i].type)==FALSE))
		{
			part = malloc(sizeof(struct partition));
			if(!part)
				return FALSE;

			strlcpy((char *)part->name, (const char *)"", sizeof(part->name));
			part->sector_from = sector + ((mbr.entry[i].start[3] << 24) | (mbr.entry[i].start[2] << 16) | (mbr.entry[i].start[1] << 8) | (mbr.entry[i].start[0] << 0));
			part->sector_to = part->sector_from + ((mbr.entry[i].length[3] << 24) | (mbr.entry[i].length[2] << 16) | (mbr.entry[i].length[1] << 8) | (mbr.entry[i].length[0] << 0)) - 1;
			part->sector_size = disk->sector_size;
			list_add_tail(&part->entry, &(disk->info.entry));
		}
	}

	for(i=0; i<4; i++)
	{
		if(is_dos_extended(mbr.entry[i].type)==TRUE)
		{
			start = ((mbr.entry[i].start[3] << 24) | (mbr.entry[i].start[2] << 16) | (mbr.entry[i].start[1] << 8) | (mbr.entry[i].start[0] << 0)) + relative;
			return dos_partition(disk, start, (sector == 0) ? start : relative);
		}
	}

	return TRUE;
}

static bool_t parser_probe_dos(struct disk * disk)
{
	return dos_partition(disk, 0, 0);
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

core_initcall(partition_parser_dos_init);
core_exitcall(partition_parser_dos_exit);
