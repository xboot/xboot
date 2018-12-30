/*
 * kernel/command/cmd-mkfat16.c
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
#include <command/command.h>

struct fat16_boot_t {
	uint8_t boot_jmp[3];
	uint8_t oem_id[8];
	uint8_t bytes_per_sector[2];
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fat_copies;
	uint8_t rootdir_entries[2];
	uint8_t num_sectors_sm[2];
	uint8_t media_code;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t heads;
	uint32_t hidden_sectors;
	uint32_t num_sectors_lg;
	uint8_t drive_number;
	uint8_t chkdsk_needed;
	uint8_t ext_sig;
	uint8_t serial[4];
	uint8_t vol_name[11];
	uint8_t fs_type[8];
	uint8_t boot_code[448];
	uint8_t block_sig[2];
};

static int format_fat16(struct block_t * blk)
{
	struct fat16_boot_t bs;
	uint8_t block[512];
	time_t now = time(NULL);
	uint32_t blocks_per_cluster = 1;
	uint32_t reserved_blocks;
	uint32_t data_blocks, clusters;
	uint32_t blocks_per_fat;
	uint32_t i;

	/*
	 * Figure out how many blocks of the volume will be reserved by the root
	 * directory and the BPB. This should be made adjustable, but the spec
	 * pretty much says that you should always use 512...
	 */
	reserved_blocks = ((512 * 32) / 512) + 1;

	/*
	 * Make an initial guess at the number of data blocks. This guess will
	 * obviously be wrong, as we're not including the FAT.
	 */
	data_blocks = blk->blkcnt - reserved_blocks;

	/*
	 * Calculate how many blocks the FAT would take up for one block per
	 * cluster (since that's what we've got set up at the moment).
	 */
	blocks_per_fat = (data_blocks + 255) / 256;

	/*
	 * Now, we know the real block count in the data area... Note that we may
	 * overestimate the real size of the FAT in this calculation. That's fine,
	 * as we really need to avoid underestimating it.
	 */
	clusters = data_blocks - (blocks_per_fat * 2);

	/*
	 * Bail out early if we know there's no chance this will work right.
	 */
	if(clusters < 4085)
	{
		printf("Block too small to format as FAT16\r\n");
		return -1;
	}

	/*
	 * Now, figure out the smallest cluster size we can use and still accomodate
	 * the number of blocks we have in our cluster count.
	 */
	while(clusters > 65525 && blocks_per_cluster <= 128)
	{
		/*
		 * If we've got too many clusters at this point, double the number of
		 * blocks per cluster. Not exactly the most efficient way to do this,
		 * but it works.
		 */
		blocks_per_cluster <<= 1;

		/*
		 * Recalculate the FAT size, the number of data blocks, and the number
		 * of clusters. This could be better, but this should be pretty clear as it is.
		 */
		clusters = data_blocks / blocks_per_cluster;

		/*
		 * If the number of clusters is evenly divisible by the amount of
		 * clusters per FAT block (256), then we can just do integer division to
		 * figure out how many blocks we need. Otherwise, we need to add one to
		 * the result of the (rounded down) integer division to ensure we don't
		 * run out of space in the FAT. The easiest way to do this is to simply
		 * add 255 to the number of clusters and then do the integer division,
		 * saving us a branch (if it is evenly divisible, this won't add 1 to
		 * the final result, otherwise it will).
		 */
		blocks_per_fat = (clusters + 255) / 256;

		clusters = (data_blocks - (blocks_per_fat * 2)) / blocks_per_cluster;
	}

	/*
	 * Calculate the real number of data blocks...
	 */
	data_blocks = blk->blkcnt - reserved_blocks - (2 * blocks_per_fat);

	/*
	 * Did we fail to get a match?
	 */
	if(blocks_per_cluster > 128)
	{
		printf("Cowardly refusing to make a large FAT16 volume\r\n");
		return -1;
	}

	printf("FAT16 filesystem parameters:\r\n");
	printf(" Cluster size (in blocks): %ld (%ld bytes)\r\n", blocks_per_cluster, blocks_per_cluster * 512);
	printf(" Number of raw blocks: %ld\r\n", blk->blkcnt);
	printf(" Data blocks on volume: %ld\r\n", data_blocks);
	printf(" Number of clusters: %ld\r\n", clusters);
	printf(" Size of each FAT (in blocks): %ld\r\n", blocks_per_fat);
	printf(" Number of root directory entries: %d\r\n", 512);

	/*
	 * Fill in the BPB
	 */
	bs.boot_jmp[0] = 0xEB;              /* Short JMP */
	bs.boot_jmp[1] = 0x3C;              /* To after the BPB. */
	bs.boot_jmp[2] = 0x90;              /* NOP */
	memcpy(bs.oem_id, "xboot   ", 8);
	bs.bytes_per_sector[0] = 0x00;      /* 512 byte sectors. */
	bs.bytes_per_sector[1] = 0x02;
	bs.sectors_per_cluster = blocks_per_cluster;
	bs.reserved_sectors = 1;
	bs.fat_copies = 2;
	bs.rootdir_entries[0] = 512 & 0xFF;
	bs.rootdir_entries[1] = (512 >> 8) & 0xFF;
	bs.num_sectors_sm[0] = 0;           /* We'll use the 4-byte version later */
	bs.num_sectors_sm[1] = 0;
	bs.media_code = 0xF8;               /* 0xF8 = Not a floppy disk */
	bs.sectors_per_fat = blocks_per_fat;
	bs.sectors_per_track = 1;           /* We don't care about CHS addressing */
	bs.heads = 1;                       /* Ditto. */
	bs.hidden_sectors = 0;
	bs.num_sectors_lg = blk->blkcnt;
	bs.drive_number = 0x80;             /* Fixed disk */
	bs.chkdsk_needed = 0;
	bs.ext_sig = 0x29;                  /* Magic value, required by FAT */
	bs.serial[0] = now & 0xFF;          /* Set the serial number to the time */
	bs.serial[1] = (now >> 8) & 0xFF;
	bs.serial[2] = (now >> 16) & 0xFF;
	bs.serial[3] = (now >> 24) & 0xFF;
	memcpy(bs.vol_name, "XBOOT      ", 11);
	memcpy(bs.fs_type, "FAT16   ", 8);
	bs.boot_code[0] = 0xCD;             /* INT 18h */
	bs.boot_code[1] = 0x18;
	bs.boot_code[2] = 0xEB;             /* JMP -2 (an infinite loop) */
	bs.boot_code[3] = 0xFE;
	memset(&bs.boot_code[4], 0, 444);
	bs.block_sig[0] = 0x55;             /* Magic value, required by FAT */
	bs.block_sig[1] = 0xAA;

	/*
	 * Now, all that is left is actually writing data out to the disk
	 */
	memcpy(block, &bs, 512);
	block_write(blk, block, 0 * 512, 1 * 512);	/* Block 0 = BPB */

	/*
	 * Zero out the block, so we can write it over and over again to fill in the
	 * root directory.
	 */
	memset(block, 0, 512);
	for(i = 1; i < reserved_blocks; ++i)
	{
		block_write(blk, block, i * 512, 1 * 512);
	}

	/*
	 * Now, fill in the first two entries of the FAT so we can write the two
	 * copies of the FAT to the disk.
	 */
	block[0] = 0xF8;
	block[1] = 0xFF;
	block[2] = 0xFF;
	block[3] = 0xFF;
	block_write(blk, block, reserved_blocks * 512, 1 * 512);
	block_write(blk, block, (reserved_blocks + blocks_per_fat) * 512, 1 * 512);

	/*
	 * Clear the two entries and fill in the rest of the FATs.
	 */
	block[0] = block[1] = block[2] = block[3] = 0;
	for(i = 1; i < blocks_per_fat; ++i)
	{
		block_write(blk, block, (reserved_blocks + i) * 512, 1 * 512);
		block_write(blk, block, (reserved_blocks + i + blocks_per_fat) * 512, 1 * 512);
	}

	block_sync(blk);
	return 0;
}

static void usage(void)
{
	struct device_t * pos, * n;

	printf("usage:\r\n");
	printf("    mkfat16 <block device>\r\n");

	printf("supported device list:\r\n");
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_BLOCK], head)
	{
		printf("    %s\r\n", pos->name);
	}
}

static int do_mkfat16(int argc, char ** argv)
{
	struct block_t * blk;

	if(argc != 2)
	{
		usage();
		return -1;
	}

	blk = search_block(argv[1]);
	if(!blk)
	{
		printf("Can't find block device '%s'\r\n", argv[1]);
		return -1;
	}

	if(format_fat16(blk) < 0)
	{
		printf("Format failed.\r\n");
		return -1;
	}

	printf("Format complete.\r\n");
	return 0;
}

static struct command_t cmd_mkfat16 = {
	.name	= "mkfat16",
	.desc	= "format fat16 filesystem on block device",
	.usage	= usage,
	.exec	= do_mkfat16,
};

static __init void mkfat16_cmd_init(void)
{
	register_command(&cmd_mkfat16);
}

static __exit void mkfat16_cmd_exit(void)
{
	unregister_command(&cmd_mkfat16);
}

command_initcall(mkfat16_cmd_init);
command_exitcall(mkfat16_cmd_exit);
