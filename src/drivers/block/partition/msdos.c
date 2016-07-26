/*
 * drivers/block/partition/msdos.c
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

#include <block/partition.h>

struct msdos_partition_entry_t
{
	/* If active is 0x80, otherwise is 0x00 */
	u8_t flag;

	/* The head of the start */
	u8_t  start_head;

	/* The sector of the start */
	u8_t start_sector;

	/* The cylinder of the end */
	u8_t start_cylinder;

	/* The partition type */
	u8_t type;

	/* The head of the end */
	u8_t  end_head;

	/* The sector of the end */
	u8_t end_sector;

	/* The cylinder of the end */
	u8_t end_cylinder;

	/* The start sector*/
	u8_t start[4];

	/* The length in sector units */
	u8_t length[4];

} __attribute__ ((packed));

struct msdos_partition_mbr_t
{
	/*
	 * The code area (actually, including BPB)
	 */
	u8_t code[446];

	/*
	 * Four partition entries
	 */
	struct msdos_partition_entry_t entry[4];

	/*
	 * The signature 0x55, 0xaa
	 */
	u8_t signature[2];

} __attribute__ ((packed));

static bool_t is_msdos_extended(u8_t type)
{
	if((type == 0x5) || (type == 0xf) || (type == 0x85))
		return TRUE;
	return FALSE;
}

static bool_t msdos_partition(struct disk_t * disk, size_t sector, size_t relative)
{
	struct msdos_partition_mbr_t mbr;
	struct partition_t * part;
	size_t start;
	int i;

	if(!disk || !disk->name)
		return FALSE;

	if(!disk->size || !disk->count)
		return FALSE;

	if(disk_read(disk, (u8_t *)(&mbr), sector * disk->size, sizeof(struct msdos_partition_mbr_t)) != sizeof(struct msdos_partition_mbr_t))
		return FALSE;

	if((mbr.signature[0] != 0x55) || mbr.signature[1] != 0xaa)
		return FALSE;

	for(i = 0; i < 4; i++)
	{
		if((mbr.entry[i].type != 0) && (is_msdos_extended(mbr.entry[i].type)==FALSE))
		{
			part = malloc(sizeof(struct partition_t));
			if(!part)
				return FALSE;

			strlcpy(part->name, "", sizeof(part->name));
			part->from = sector + ((mbr.entry[i].start[3] << 24) | (mbr.entry[i].start[2] << 16) | (mbr.entry[i].start[1] << 8) | (mbr.entry[i].start[0] << 0));
			part->to = part->from + ((mbr.entry[i].length[3] << 24) | (mbr.entry[i].length[2] << 16) | (mbr.entry[i].length[1] << 8) | (mbr.entry[i].length[0] << 0)) - 1;
			part->size = disk->size;
			list_add_tail(&part->entry, &(disk->part.entry));
		}
	}

	for(i = 0; i < 4; i++)
	{
		if(is_msdos_extended(mbr.entry[i].type) == TRUE)
		{
			start = ((mbr.entry[i].start[3] << 24) | (mbr.entry[i].start[2] << 16) | (mbr.entry[i].start[1] << 8) | (mbr.entry[i].start[0] << 0)) + relative;
			return msdos_partition(disk, start, (sector == 0) ? start : relative);
		}
	}

	return TRUE;
}

static bool_t msdos_map(struct disk_t * disk)
{
	return msdos_partition(disk, 0, 0);
}

static struct partition_map_t msdos = {
	.name	= "msdos",
	.map	= msdos_map,
};

static __init void partition_map_msdos_init(void)
{
	if(register_partition_map(&msdos))
		LOG("Register partition map 'msdos'");
	else
		LOG("Fail to register partition map 'msdos'");
}

static __exit void partition_map_msdos_exit(void)
{
	if(unregister_partition_map(&msdos))
		LOG("Unregister partition map 'msdos'");
	else
		LOG("Fail to unregister partition map 'msdos'");
}

core_initcall(partition_map_msdos_init);
core_exitcall(partition_map_msdos_exit);
