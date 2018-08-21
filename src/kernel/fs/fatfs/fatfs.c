/*
 * kernel/fs/fatfs/fatfs.c
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

#include <xboot.h>
#include <types.h>
#include <ctype.h>
#include <stdarg.h>
#include <malloc.h>
#include <xboot/initcall.h>
#include <block/block.h>
#include <xboot/device.h>
#include <fs/vfs/vfs.h>
#include <fs/fs.h>

#if 0
/*
 * fat attribute
 */
#define FAT_ATTR_RDONLY			(0x01)
#define FAT_ATTR_HIDDEN			(0x02)
#define FAT_ATTR_SYSTEM			(0x04)
#define FAT_ATTR_VOLID			(0x08)
#define FAT_ATTR_SUBDIR			(0x10)
#define FAT_ATTR_ARCH			(0x20)
#define FFAT_ATTR_DEVICE		(0x40)

#define IS_DIR(de)				(((de)->attr) & FAT_ATTR_SUBDIR)
#define IS_VOL(de)				(((de)->attr) & FAT_ATTR_VOLID)
#define IS_FILE(de)				(!IS_DIR(de) && !IS_VOL(de))
#define IS_DELETED(de)  		((de)->name[0] == 0xe5)
#define IS_EMPTY(de)    		((de)->name[0] == 0)

/*
 * boot sector
 */
struct fat_boot_sector {
	/*
	 * jump instruction and oem name
	 */
	u8_t	jmp_instruction[3];
	u8_t	oem_name[8];

	/*
	 * the common part of the fat12, fat16 and fat32 bios parameter block
	 */
	u8_t	bytes_per_sector[2];
	u8_t	sectors_per_cluster;
	u8_t	reserved_sectors[2];
	u8_t	num_of_fats;
	u8_t	root_entries[2];
	u8_t	total_sectors[2];
	u8_t	media_descriptor;
	u8_t	sectors_per_fat[2];
	u8_t	sectors_per_track[2];
	u8_t	num_of_heads[2];
	u8_t	hidden_sectors[4];
	u8_t	big_total_sectors[4];

	/*
	 * the last part of fat12, fat16 and fat32
	 */
	union {
		u8_t code[474];

		struct {
			u8_t	drive_number;
			u8_t	reserved;
			u8_t	boot_signature;
			u8_t	volume_id[4];
			u8_t	volume_lab[11];
			u8_t	fs_type[8];

			u8_t 	fill[474 - 26];
		}fat12;

		struct {
			u8_t	drive_number;
			u8_t	reserved;
			u8_t	boot_signature;
			u8_t	volume_id[4];
			u8_t	volume_lab[11];
			u8_t	fs_type[8];

			u8_t 	fill[474 - 26];
		}fat16;

		struct {
			u8_t	sectors_per_fat_32[4];
			u8_t	ext_flags[2];
			u8_t	fs_version[2];
			u8_t	root_clus[4];
			u8_t	fs_info[2];
			u8_t	back_boot_sector[2];
			u8_t	reserved_before[12];

			u8_t	driver_number;
			u8_t	reserved;
			u8_t	boot_signature;
			u8_t	volume_id[4];
			u8_t	volume_lab[11];
			u8_t	fs_type[8];

			u8_t 	fill[474 - 54];
		}fat32;
	} x;

	/*
	 * the signature 0x55, 0xaa
	 */
	u8_t	signature[2];
} __attribute__ ((packed));

/*
 * fat directory entry
 */
struct fat_dirent {
	u8_t	name[11];
	u8_t	attr;
	u8_t	reserve[10];
	u8_t	time[2];
	u8_t	date[2];
	u8_t	cluster[2];
	u8_t	size[4];
} __attribute__ ((packed));

/*
 * file / directory node
 */
struct fat_node {
	struct fat_dirent	dirent;		/* copy of directory entry */
	u32_t				sector;		/* sector for directory entry */
	u32_t				offset;		/* offset of directory entry in sector */
};

/*
 * fat filesystem type
 */
enum fat_type {
	FAT_TYPE_FAT12,
	FAT_TYPE_FAT16,
	FAT_TYPE_FAT32
};

/*
 * fatfs mount data
 */
struct fatfs_mount_data {
	/* fat type */
	enum fat_type type;

	/* the size of sector */
	u32_t sector_size;

	/* sectors per cluster */
	u32_t sectors_per_cluster;

	/* cluster size */
	u32_t cluster_size;

	/* start sector for fat entries */
	u32_t fat_start;

	/* start sector for root directory */
	u32_t root_start;

	/* start sector for data */
	u32_t data_start;

	/* last cluser */
	u32_t last_cluster;

	/* start cluster to free search */
	u32_t free_scan;

	/* mask for cluster */
	u32_t fat_mask;

	/* id of end cluster */
	u32_t fat_eof;

	/* vnode for root */
	struct vnode_t * root_vnode;

	/* local data buffer */
	char * io_buf;

	/* buffer for fat entry */
	char * fat_buf;

	/* buffer for directory entry */
	char * dir_buf;

	/* mounted block device */
	struct blkdev_t * blk;
};

/*
 * filesystem operations
 */
static s32_t fatfs_mount(struct mount_t * m, char * dev, s32_t flag)
{
	struct fatfs_mount_data * md;
	struct blkdev_t * blk;
	struct fat_boot_sector fbs;
	u32_t sector_size;
	u32_t tmp;

	if(dev == NULL)
		return EINVAL;

	blk = (struct blkdev_t *)m->m_dev;
	if(!blk || !blk->info)
		return ENODEV;

	if(get_blkdev_total_size(blk) <= sizeof(struct fat_boot_sector))
		return EINTR;

	if(block_read(blk, (u8_t *)(&fbs), 0, sizeof(struct fat_boot_sector)) != sizeof(struct fat_boot_sector))
		return EIO;

	/*
	 * check both signature (0x55, 0xaa)
	 */
	if((fbs.signature[0] != 0x55) || fbs.signature[1] != 0xaa)
		return EINVAL;

	/* the logical sector size (bytes 11-12) is a power of two, at least 512 */
	sector_size = (fbs.bytes_per_sector[1] << 8) | fbs.bytes_per_sector[0];
	if( (sector_size < 512) || (!is_power_of_2(sector_size)) )
		return EINVAL;

	/* the cluster size (byte 13) is a power of two */
	if(! is_power_of_2(fbs.sectors_per_cluster))
		return EINVAL;

	/* the number of reserved sectors (bytes 14-15) is nonzero */
	tmp = (fbs.reserved_sectors[1] << 8) | fbs.reserved_sectors[0];
	if(tmp == 0)
		return EINVAL;

	/* the number of fats (byte 16) is nonzero */
	if(fbs.num_of_fats == 0x00)
		return EINVAL;

	/* the number of root directory entries (bytes 17-18) must be sector aligned */
	tmp = (fbs.root_entries[1] << 8) | fbs.root_entries[0];
	if(tmp % (sector_size / sizeof(struct fat_dirent)) != 0)
		return EINVAL;

	/* the sector per track (byte 24-25) is nonzero */
	tmp = (fbs.sectors_per_track[1] << 8) | fbs.sectors_per_track[0];
	if(tmp == 0)
		return EINVAL;

	/* the number of heads (byte 26-27) is nonzero */
	tmp = (fbs.num_of_heads[1] << 8) | fbs.num_of_heads[0];
	if(tmp == 0)
		return EINVAL;

	md = malloc(sizeof(struct fatfs_mount_data));
	if(!md)
		return ENOMEM;

	/* determine the type of fat */
	if( strncmp((const s8_t *)fbs.x.fat12.fs_type, (const s8_t *)"FAT12   ", 8) == 0 )
	{
		md->type = FAT_TYPE_FAT12;
	}
	else if( strncmp((const s8_t *)fbs.x.fat16.fs_type, (const s8_t *)"FAT16   ", 8) == 0 )
	{
		md->type = FAT_TYPE_FAT16;
	}
	else if( strncmp((const s8_t *)fbs.x.fat32.fs_type, (const s8_t *)"FAT32   ", 8) == 0 )
	{
		md->type = FAT_TYPE_FAT32;
	}
	else
	{
		free(md);
		return EINVAL;
	}

	/* build mount data */
	switch(md->type)
	{
	case FAT_TYPE_FAT12:
		md->sector_size = sector_size;
		md->sectors_per_cluster = fbs.sectors_per_cluster;
		md->cluster_size = md->sectors_per_cluster * md->sector_size;
		tmp = ((fbs.hidden_sectors[3] << 24) | (fbs.hidden_sectors[2] << 16) | (fbs.hidden_sectors[1] << 8) | (fbs.hidden_sectors[0] << 0));
		md->fat_start = tmp + ((fbs.reserved_sectors[1] << 8) | (fbs.reserved_sectors[0] << 0));
		md->root_start = md->fat_start + (fbs.num_of_fats * ((fbs.sectors_per_fat[1] << 8) | (fbs.sectors_per_fat[0] << 0)));
		md->data_start = md->root_start + ( ((fbs.root_entries[1] << 8) | (fbs.root_entries[0] << 0)) / (sector_size / sizeof(struct fat_dirent)) );

		tmp = ((fbs.total_sectors[1] << 8) | (fbs.total_sectors[0] << 0));
		if(tmp == 0)
			tmp = ((fbs.big_total_sectors[3] << 24) | (fbs.big_total_sectors[2] << 16) | (fbs.big_total_sectors[1] << 8) | (fbs.big_total_sectors[0] << 0));

		if(tmp == 0)
		{
			free(md);
			return EINVAL;
		}

		md->last_cluster = (tmp - md->data_start) / md->sectors_per_cluster + 2;
		md->free_scan = 2;
		md->fat_mask = 0x00000fff;
		md->fat_eof = 0xffffffff & 0x00000fff;
		break;

	case FAT_TYPE_FAT16:
		md->sector_size = sector_size;
		md->sectors_per_cluster = fbs.sectors_per_cluster;
		md->cluster_size = md->sectors_per_cluster * md->sector_size;
		tmp = ((fbs.hidden_sectors[3] << 24) | (fbs.hidden_sectors[2] << 16) | (fbs.hidden_sectors[1] << 8) | (fbs.hidden_sectors[0] << 0));
		md->fat_start = tmp + ((fbs.reserved_sectors[1] << 8) | (fbs.reserved_sectors[0] << 0));
		md->root_start = md->fat_start + (fbs.num_of_fats * ((fbs.sectors_per_fat[1] << 8) | (fbs.sectors_per_fat[0] << 0)));
		md->data_start = md->root_start + ( ((fbs.root_entries[1] << 8) | (fbs.root_entries[0] << 0)) / (sector_size / sizeof(struct fat_dirent)) );

		tmp = ((fbs.total_sectors[1] << 8) | (fbs.total_sectors[0] << 0));
		if(tmp == 0)
			tmp = ((fbs.big_total_sectors[3] << 24) | (fbs.big_total_sectors[2] << 16) | (fbs.big_total_sectors[1] << 8) | (fbs.big_total_sectors[0] << 0));

		if(tmp == 0)
		{
			free(md);
			return EINVAL;
		}

		md->last_cluster = (tmp - md->data_start) / md->sectors_per_cluster + 2;
		md->free_scan = 2;
		md->fat_mask = 0x0000ffff;
		md->fat_eof = 0xffffffff & 0x0000ffff;
		break;

	case FAT_TYPE_FAT32:
		md->sector_size = sector_size;
		md->sectors_per_cluster = fbs.sectors_per_cluster;
		md->cluster_size = md->sectors_per_cluster * md->sector_size;
		tmp = ((fbs.hidden_sectors[3] << 24) | (fbs.hidden_sectors[2] << 16) | (fbs.hidden_sectors[1] << 8) | (fbs.hidden_sectors[0] << 0));
		md->fat_start = tmp + ((fbs.reserved_sectors[1] << 8) | (fbs.reserved_sectors[0] << 0));
		md->root_start = md->fat_start + (fbs.num_of_fats * ((fbs.x.fat32.sectors_per_fat_32[3] << 24) | (fbs.x.fat32.sectors_per_fat_32[2] << 16) | (fbs.x.fat32.sectors_per_fat_32[1] << 8) | (fbs.x.fat32.sectors_per_fat_32[0] << 0)));

		md->data_start = md->root_start + 1;

		break;

	default:
		free(md);
		return EINVAL;
	}

	md->io_buf = malloc(md->cluster_size);
	if(!md->io_buf)
	{
		free(md);
		return ENOMEM;
	}

	md->fat_buf = malloc(md->sector_size * 2);
	if(!md->fat_buf)
	{
		free(md->io_buf);
		free(md);
		return ENOMEM;
	}

	md->dir_buf = malloc(md->sector_size);
	if(!md->dir_buf)
	{
		free(md->fat_buf);
		free(md->io_buf);
		free(md);
		return ENOMEM;
	}

	md->blk = blk;
	m->m_flags = flag & MOUNT_MASK;
	m->m_data = md;

	// for debug
	switch(md->type)
	{
	case FAT_TYPE_FAT12:
		break;
	case FAT_TYPE_FAT16:
		break;
	case FAT_TYPE_FAT32:
		break;
	default:
		break;
	}

	LOG("sector size: %ld\r\n", md->sector_size);
	LOG("sectors per cluster: %ld\r\n", md->sectors_per_cluster);
	LOG("cluster size: %ld\r\n", md->cluster_size);

	LOG("fat start: %ld\r\n", md->fat_start);
	LOG("root start: %ld\r\n", md->root_start);
	LOG("data start: %ld\r\n", md->data_start);

	LOG("last cluster: %ld\r\n", md->last_cluster);
	LOG("free scan: %ld\r\n", md->free_scan);

	LOG("fat mask: 0x%08x\r\n", md->fat_mask);
	LOG("fat eof: 0x%08x\r\n", md->fat_eof);

	return 0;
}

static s32_t fatfs_unmount(struct mount_t * m)
{
	struct fatfs_mount_data * md = m->m_data;

	free(m->m_root->v_data);

	free(md->dir_buf);
	free(md->fat_buf);
	free(md->io_buf);

	free(md);

	return 0;
}

static s32_t fatfs_sync(struct mount_t * m)
{
	return 0;
}

static s32_t fatfs_vget(struct mount_t * m, struct vnode_t * node)
{
	struct fat_node * n;

	n = malloc(sizeof(struct fat_node));
	if(!n)
		return ENOMEM;
	node->v_data = n;

	return 0;
}

static s32_t fatfs_statfs(struct mount_t * m, struct statfs * stat)
{
	return -1;
}

/*
 * convert file name to 8.3 format ("foo.bar" => "foo     bar")
 */
static void fat_convert_name(u8_t * org, u8_t * name)
{
	s32_t i;

	memset(name, ' ', 11);

	for(i = 0; i <= 11; i++)
	{
		if(!*org)
			break;
		if(*org == '/')
			break;
		if(*org == '.')
		{
			i = 7;
			org++;
			continue;
		}

		*(name + i) = *org;
		org++;
	}
}

/*
 * restore file name to normal format ("foo     bar" => "foo.bar")
 */
static void fat_restore_name(u8_t * org, u8_t * name)
{
	s32_t i;

	memset(name, 0, 13);

	for(i = 0; i < 8; i++)
	{
		if(*org != ' ')
			*name++ = *org;
		org++;
	}

	if(*org != ' ')
		*name++ = '.';
	for(i = 0; i < 3; i++)
	{
		if(*org != ' ')
			*name++ = *org;
		org++;
	}
}

/*
 * compare tow file names
 */
static bool_t fat_compare_name(u8_t * n1, u8_t * n2)
{
	s32_t i;

	for(i = 0; i < 11; i++, n1++, n2++)
	{
		if(toupper(*n1) != toupper(*n2))
			return FALSE;
	}

	return TRUE;
}

/*
 * check specified name is valid as fat file name.
 */
static bool_t fat_valid_name(u8_t * name)
{
	const u8_t invalid_char[] = "*?<>|\"+=,;[] \345";
	s32_t len = 0;

	/* . or .. */
	if(*name == '.')
	{
		name++;
		if(*name == '.')
			name++;
		return(*(name + 1) == '\0') ? TRUE : FALSE;
	}

	/* first char must be alphabet or numeric */
	if(!isalnum(*name))
		return FALSE;

	while(*name != '\0')
	{
		if(strchr((const s8_t *)invalid_char, *name))
			return FALSE;
		if(*name == '.')
			break;
		if(++len > 8)
			return FALSE;
		name++;
	}

	if(*name == '\0')
		return TRUE;
	name++;

	if(*name == '\0')
		return TRUE;
	len = 0;
	while(*name != '\0')
	{
		if(strchr((const s8_t *)invalid_char, *name))
			return FALSE;
		if(*name == '.')
			return FALSE;
		if(++len > 3)
			return FALSE;
		name++;
	}

	return TRUE;
}

/*
 * read the fat entry for specified cluster.
 */
static bool_t read_fat_entry(struct fatfs_mount_data * md, u32_t cl)
{
	return FALSE;
}

/*
 * get next cluster number of fat chain.
 */
static s32_t fat_next_cluster(struct fatfs_mount_data * md, u32_t cl, u32_t * next)
{
	u32_t offset;
	u32_t val;

	if(read_fat_entry(md, cl) != TRUE)
		return EIO;

	switch(md->type)
	{
	case FAT_TYPE_FAT12:
		offset = (cl * 3 / 2) % md->sector_size;
		val = (md->fat_buf[offset + 1] << 8) | (md->fat_buf[offset + 0] << 0);
		if(cl & 0x1)
			val >>= 4;
		else
			val &= 0xfff;
		break;

	case FAT_TYPE_FAT16:
		offset = (cl * 2) % md->sector_size;
		val = (md->fat_buf[offset + 1] << 8) | (md->fat_buf[offset + 0] << 0);
		break;

	case FAT_TYPE_FAT32:
		offset = (cl * 4) % md->sector_size;
		val = (md->fat_buf[offset + 3] << 24) | (md->fat_buf[offset + 2] << 16) | (md->fat_buf[offset + 1] << 8) | (md->fat_buf[offset + 0] << 0);
		break;

	default:
		return EINVAL;
	}

	*next = val;
	return 0;
}

/*
 * read directory entry to buffer, with cache.
 */
static bool_t fat_read_dirent(struct fatfs_mount_data * md, u32_t sector)
{
	loff_t off = sector * md->sector_size;
	loff_t size = md->sector_size;

	if(block_read(md->blk, (u8_t *)(md->dir_buf), off, size) != size)
		return FALSE;

	return TRUE;
}

/*
 * write directory entry from buffer.
 */
static bool_t fat_write_dirent(struct fatfs_mount_data * md, u32_t sector)
{
	loff_t off = sector * md->sector_size;
	loff_t size = md->sector_size;

	if(block_write(md->blk, (const u8_t *)(md->dir_buf), off, size) != size)
		return FALSE;

	return TRUE;
}

/*
 * find directory entry in specified sector.
 */
static s32_t fat_lookup_dirent(struct fatfs_mount_data * md, u32_t sec, u8_t * name, struct fat_node * np)
{
	struct fat_dirent * de;
	s32_t i, num;

	if(fat_read_dirent(md, sec) != TRUE)
		return EIO;

	de = (struct fat_dirent *)md->dir_buf;
	num = md->sector_size / sizeof(struct fat_dirent);

	for(i = 0; i < num; i++)
	{
		if(IS_EMPTY(de))
			return ENOENT;

		if(!IS_VOL(de) && !fat_compare_name(de->name, name))
		{
			*(&np->dirent) = *de;
			np->sector = sec;
			np->offset = sizeof(struct fat_dirent) * i;
			return 0;
		}

		de++;
	}

	return EAGAIN;
}

/*
 * find directory entry for specified name in directory.
 */
static s32_t fat_lookup_node(struct vnode_t * dnode, u8_t * name, struct fat_node * np)
{
	struct fatfs_mount_data * md;
	u8_t fat_name[12];
	u32_t cl, sec;
	s32_t i, err;

	if(name == NULL)
		return ENOENT;

	fat_convert_name(name, fat_name);
	*(fat_name + 11) = '\0';

	md = (struct fatfs_mount_data *)dnode->v_mount->m_data;
	cl = dnode->v_blkno;
	if(cl == 0)
	{
		/* search entry in root directory */
		for(sec = md->root_start; sec < md->data_start; sec++)
		{
			err = fat_lookup_dirent(md, sec, fat_name, np);
			if(err != EAGAIN)
				return err;
		}
	}
	else
	{
#if 0
		/* search entry in sub directory */
		while(! (((cl) & 0xfffffff8) == ((md)->fat_mask & 0xfffffff8)) )
		{
            sec = (md->data_start + (cl - 2) * md->sectors_per_cluster);

			for(i = 0; i < md->sectors_per_cluster; i++)
			{
				err = fat_lookup_dirent(md, sec, fat_name, np);
				if(err != EAGAIN)
					return err;
				sec++;
			}
			err = fat_next_cluster(md, cl, &cl);
			if(err)
				return err;
		}
#endif
	}

	return ENOENT;
}

/*
 * vnode operations
 */
static s32_t fatfs_open(struct vnode_t * node, s32_t flag)
{
	return 0;
}

static s32_t fatfs_close(struct vnode_t * node, struct file_t * fp)
{
	return 0;
}

static s32_t fatfs_read(struct vnode_t * node, struct file_t * fp, void * buf, loff_t size, loff_t * result)
{
	return -1;
}

static s32_t fatfs_write(struct vnode_t * node , struct file_t * fp, void * buf, loff_t size, loff_t * result)
{
	return -1;
}

static s32_t fatfs_seek(struct vnode_t * node, struct file_t * fp, loff_t off1, loff_t off2)
{
	return 0;
}

static s32_t fatfs_ioctl(struct vnode_t * node, struct file_t * fp, int cmd, void * arg)
{
	return -1;
}

static s32_t fatfs_fsync(struct vnode_t * node, struct file_t * fp)
{
	return 0;
}

static s32_t fatfs_readdir(struct vnode_t * node, struct file_t * fp, struct dirent_t * dir)
{
	struct fatfs_mount_data * md;
	struct fat_node np;
	struct fat_dirent * de;
	s32_t err;

	LOG("%s\r\n", __FUNCTION__);

	md = node->v_mount->m_data;
//TODO
//	err = fatfs_get_node(node, fp->f_offset, &np);
	if(err != 0)
		return err;

	de = &np.dirent;
	fat_restore_name((u8_t *)&de->name, (u8_t *)dir->d_name);

	if(IS_DIR(de))
		dir->d_type = DT_DIR;
	else if(IS_FILE(de))
		dir->d_type = DT_REG;
	else
		dir->d_type = DT_UNKNOWN;

	dir->d_fileno = fp->f_offset;
	dir->d_namlen = strlen((const s8_t *)dir->d_name);
	fp->f_offset++;

	return 0;
}

static s32_t fatfs_lookup(struct vnode_t * dnode, char * name, struct vnode_t * node)
{
	struct fatfs_mount_data * md;
	struct fat_node * np;
	struct fat_dirent * de;
	s32_t err;

	LOG("%s\r\n", __FUNCTION__);

	if(*name == '\0')
		return ENOENT;

	md = node->v_mount->m_data;

	np = node->v_data;
	err = fat_lookup_node(dnode, (u8_t *)name, np);
	if(err != 0)
		return err;

	de = &np->dirent;
	node->v_type = IS_DIR(de) ? VDIR : VREG;
//	fat_attr_to_mode(de->attr, &vp->v_mode);
	node->v_mode = S_IRWXU | S_IRWXG | S_IRWXO;
	node->v_size = de->size;
	node->v_blkno = de->cluster;

	return 0;
}

static s32_t fatfs_create(struct vnode_t * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t fatfs_remove(struct vnode_t * dnode, struct vnode_t * node, char * name)
{
	return -1;
}

static s32_t fatfs_rename(struct vnode_t * dnode1, struct vnode_t * node1, char * name1, struct vnode_t *dnode2, struct vnode_t * node2, char * name2)
{
	return -1;
}

static s32_t fatfs_mkdir(struct vnode_t * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t fatfs_rmdir(struct vnode_t * dnode, struct vnode_t * node, char * name)
{
	return -1;
}

static s32_t fatfs_getattr(struct vnode_t * node, struct vattr_t * attr)
{
	return -1;
}

static s32_t fatfs_setattr(struct vnode_t * node, struct vattr_t * attr)
{
	return -1;
}

static s32_t fatfs_inactive(struct vnode_t * node)
{
	return -1;
}

static s32_t fatfs_truncate(struct vnode_t * node, loff_t length)
{
	return -1;
}

/*
 * fatfs vnode operations
 */
static struct vnops_t fatfs_vnops = {
	.vop_open 		= fatfs_open,
	.vop_close		= fatfs_close,
	.vop_read		= fatfs_read,
	.vop_write		= fatfs_write,
	.vop_seek		= fatfs_seek,
	.vop_ioctl		= fatfs_ioctl,
	.vop_fsync		= fatfs_fsync,
	.vop_readdir	= fatfs_readdir,
	.vop_lookup		= fatfs_lookup,
	.vop_create		= fatfs_create,
	.vop_remove		= fatfs_remove,
	.vop_rename		= fatfs_rename,
	.vop_mkdir		= fatfs_mkdir,
	.vop_rmdir		= fatfs_rmdir,
	.vop_getattr	= fatfs_getattr,
	.vop_setattr	= fatfs_setattr,
	.vop_inactive	= fatfs_inactive,
	.vop_truncate	= fatfs_truncate,
};

/*
 * file system operations
 */
static struct vfsops_t fatfs_vfsops = {
	.vfs_mount		= fatfs_mount,
	.vfs_unmount	= fatfs_unmount,
	.vfs_sync		= fatfs_sync,
	.vfs_vget		= fatfs_vget,
	.vfs_statfs		= fatfs_statfs,
	.vfs_vnops		= &fatfs_vnops,
};

/*
 * fatfs filesystem
 */
static struct filesystem_t fatfs = {
	.name		= "fatfs",
	.vfsops		= &fatfs_vfsops,
};

static __init void filesystem_fatfs_init(void)
{
	filesystem_register(&fatfs);
}

static __exit void filesystem_fatfs_exit(void)
{
	filesystem_unregister(&fatfs);
}

core_initcall(filesystem_fatfs_init);
core_exitcall(filesystem_fatfs_exit);

#endif
