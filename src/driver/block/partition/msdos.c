/*
 * driver/block/partition/msdos.c
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
	register_partition_map(&msdos);
}

static __exit void partition_map_msdos_exit(void)
{
	unregister_partition_map(&msdos);
}

core_initcall(partition_map_msdos_init);
core_exitcall(partition_map_msdos_exit);
