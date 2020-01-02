/*
 * kernel/command/cmd-mkfat32.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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

struct fat32_boot_t {
	uint8_t boot_jmp[3];
	uint8_t oem_id[8];
	uint8_t bytes_per_sector[2];
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fat_copies;
	uint8_t unused[4];
	uint8_t media_code;
	uint16_t unused2;
	uint16_t sectors_per_track;
	uint16_t heads;
	uint32_t hidden_sectors;
	uint32_t num_sectors;
	uint32_t sectors_per_fat;
	uint16_t flags;
	uint16_t version;
	uint32_t rootdir_cluster;
	uint16_t fs_info;
	uint16_t backup_boot;
	uint8_t reserved[12];
	uint8_t drive_number;
	uint8_t chkdsk_needed;
	uint8_t ext_sig;
	uint8_t serial[4];
	uint8_t vol_name[11];
	uint8_t fs_type[8];
	uint8_t boot_code[420];
	uint8_t block_sig[2];
};

struct fat32_info_t {
	uint32_t magic1;
	uint8_t reserved[480];
	uint32_t magic2;
	uint32_t free_clusters;
	uint32_t used_cluster;
	uint8_t reserved2[12];
	uint8_t block_sig[4];
};

/*
 * Round up the size of a FAT to an even cluster boundary.
 */
static uint32_t roundup_fat(int bpc, uint32_t bpf)
{
	if(bpf & (bpc - 1))
	{
		bpf += bpc;
		bpf &= ~(bpc - 1);
	}
	return bpf;
}

static int format_fat32(struct block_t * blk)
{
	struct fat32_boot_t bs;
	struct fat32_info_t is;
	uint8_t block[512];
	time_t now = time(NULL);
	uint32_t blocks_per_cluster = 1;
	uint32_t reserved_blocks = 32;
	uint32_t data_blocks, clusters;
	uint32_t blocks_per_fat;
	uint64_t vol_size;
	uint32_t i;

	/*
	 * According to dosfstools, Microsoft suggests the following number of
	 * blocks per cluster, depending on volume size:
	 * <= 260MiB : 1 block per cluster
	 * <=   8GiB : 8 blocks per cluster
	 * <=  16GiB : 16 blocks per cluster
	 * <=  32GiB : 32 blocks per cluster
	 * >   32GiB : 64 blocks per cluster
	 */
	vol_size = block_capacity(blk);
	if(vol_size >= 128ULL * SZ_1G)
		blocks_per_cluster = 256;
	else if(vol_size >= 64ULL * SZ_1G)
		blocks_per_cluster = 128;
	else if(vol_size >= 32ULL * SZ_1G)
		blocks_per_cluster = 64;
	else if(vol_size >= 16ULL * SZ_1G)
		blocks_per_cluster = 32;
	else if(vol_size >= 8ULL * SZ_1G)
		blocks_per_cluster = 16;
	else if(vol_size >= 260 * SZ_1M)
		blocks_per_cluster = 8;
	else
		blocks_per_cluster = 1;

	/*
	 * Now that we know the number of blocks per cluster, we can calculate the
	 * size of the FAT Once again, this might overestimate a bit, but that's fine
	 */
	data_blocks = block_capacity(blk) / 512 - 32;
	clusters = data_blocks / blocks_per_cluster;

	/*
	 * If the number of clusters is evenly divisible by the amount of
	 * clusters per FAT block (128), then we can just do integer division to
	 * figure out how many blocks we need. Otherwise, we need to add one to
	 * the result of the (rounded down) integer division to ensure we don't
	 * run out of space in the FAT. The easiest way to do this is to simply
	 * add 127 to the number of clusters and then do the integer division,
	 * saving us a branch (if it is evenly divisible, this won't add 1 to
	 * the final result, otherwise it will).
	 */
	blocks_per_fat = roundup_fat(blocks_per_cluster, (clusters + 127) / 128);

	data_blocks -= (2 * blocks_per_fat);
	clusters = data_blocks / blocks_per_cluster;

	/*
	 * Bail out early if we know there's no chance this will work right.
	 */
	if(clusters < 65525)
	{
		printf("Block too small to format as FAT32\r\n");
		return -1;
	}

	if(clusters > 0x0FFFFFF0)
	{
		printf("Block too large to format as FAT32\r\n");
		return -1;
	}

	printf("FAT32 filesystem parameters:\r\n");
	printf(" Cluster size (in blocks): %ld (%ld bytes)\r\n", blocks_per_cluster, blocks_per_cluster * 512);
	printf(" Number of raw blocks: %ld\r\n", block_capacity(blk) / 512);
	printf(" Data blocks on volume: %ld\r\n", data_blocks);
	printf(" Number of clusters: %ld\r\n", clusters);
	printf(" Size of each FAT (in blocks): %ld\r\n", blocks_per_fat);

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
	bs.reserved_sectors = 32;
	bs.fat_copies = 2;
	bs.unused[0] = 0;
	bs.unused[1] = 0;
	bs.unused[2] = 0;
	bs.unused[3] = 0;
	bs.media_code = 0xF8;               /* 0xF8 = Not a floppy disk */
	bs.unused2 = 0;
	bs.sectors_per_track = 1;           /* We don't care about CHS addressing */
	bs.heads = 1;                       /* Ditto. */
	bs.hidden_sectors = 0;
	bs.num_sectors = block_capacity(blk) / 512;
	bs.sectors_per_fat = blocks_per_fat;
	bs.flags = 0;                       /* Mirror the FAT, as FAT16 does */
	bs.version = 0;                     /* Version 0.0 */
	bs.rootdir_cluster = 2;
	bs.fs_info = 1;
	bs.backup_boot = 6;                 /* The standard place to put it... */
	memset(bs.reserved, 0, 12);
	bs.drive_number = 0x80;             /* Fixed disk */
	bs.chkdsk_needed = 0;
	bs.ext_sig = 0x29;                  /* Magic value, required by FAT */
	bs.serial[0] = now & 0xFF;          /* Set the serial number to the time */
	bs.serial[1] = (now >> 8) & 0xFF;
	bs.serial[2] = (now >> 16) & 0xFF;
	bs.serial[3] = (now >> 24) & 0xFF;
	memcpy(bs.vol_name, "XBOOT      ", 11);
	memcpy(bs.fs_type, "FAT32   ", 8);
	bs.boot_code[0] = 0xCD;             /* INT 18h */
	bs.boot_code[1] = 0x18;
	bs.boot_code[2] = 0xEB;             /* JMP -2 (an infinite loop) */
	bs.boot_code[3] = 0xFE;
	memset(&bs.boot_code[4], 0, 416);
	bs.block_sig[0] = 0x55;             /* Magic value, required by FAT */
	bs.block_sig[1] = 0xAA;

	/*
	 * Fill in the filesystem information sector
	 */
	is.magic1 = 0x41615252;
	memset(is.reserved, 0, 480);
	is.magic2 = 0x61417272;
	is.free_clusters = clusters - 1;
	is.used_cluster = 2;
	memset(is.reserved2, 0, 12);
	is.block_sig[0] = 0;
	is.block_sig[1] = 0;
	is.block_sig[2] = 0x55;
	is.block_sig[3] = 0xAA;

	/*
	 * Now, all that is left is actually writing data out to the disk
	 */
	memcpy(block, &bs, 512);
	block_write(blk, block, 0 * 512, 1 * 512);	/* Block 0 = BPB. */
	block_write(blk, block, 6 * 512, 1 * 512);	/* Block 6 = Backup of BPB. */

	memcpy(block, &is, 512);
	block_write(blk, block, 1 * 512, 1 * 512);	/* Block 1 = FS Info block */
	block_write(blk, block, 7 * 512, 1 * 512);	/* Block 7 = (unused) FS Info backup */

	/*
	 * Now, fill in the first three entries of the FAT so we can write the two
	 * copies of the FAT to the disk.
	 */
	memset(block + 12, 0, 500);
	block[0] = 0xF8;
	block[1] = 0xFF;
	block[2] = 0xFF;
	block[3] = 0x0F;
	block[4] = 0xFF;
	block[5] = 0xFF;
	block[6] = 0xFF;
	block[7] = 0x0F;
	block[8] = 0xFF;
	block[9] = 0xFF;
	block[10] = 0xFF;
	block[11] = 0x0F;
	block_write(blk, block, reserved_blocks * 512, 1 * 512);
	block_write(blk, block, (reserved_blocks + blocks_per_fat) * 512, 1 * 512);

	/*
	 * Clear the three entries and fill in the rest of the FATs.
	 */
	memset(block, 0, 12);
	for(i = 1; i < blocks_per_fat; ++i)
	{
		block_write(blk, block, (reserved_blocks + i) * 512, 1 * 512);
		block_write(blk, block, (reserved_blocks + i + blocks_per_fat) * 512, 1 * 512);
	}

	/*
	 * Zero out the root directory's cluster.
	 */
	reserved_blocks += 2 * blocks_per_fat;
	for(i = 0; i < blocks_per_cluster; ++i)
		block_write(blk, block, (reserved_blocks + i) * 512, 1 * 512);

	block_sync(blk);
    return 0;
}

static void usage(void)
{
	struct device_t * pos, * n;

	printf("usage:\r\n");
	printf("    mkfat32 <block device>\r\n");

	printf("supported device list:\r\n");
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_BLOCK], head)
	{
		printf("    %s\r\n", pos->name);
	}
}

static int do_mkfat32(int argc, char ** argv)
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

	if(format_fat32(blk) < 0)
	{
		printf("Format failed.\r\n");
		return -1;
	}

	printf("Format complete.\r\n");
	return 0;
}

static struct command_t cmd_mkfat32 = {
	.name	= "mkfat32",
	.desc	= "format fat32 filesystem on block device",
	.usage	= usage,
	.exec	= do_mkfat32,
};

static __init void mkfat32_cmd_init(void)
{
	register_command(&cmd_mkfat32);
}

static __exit void mkfat32_cmd_exit(void)
{
	unregister_command(&cmd_mkfat32);
}

command_initcall(mkfat32_cmd_init);
command_exitcall(mkfat32_cmd_exit);
