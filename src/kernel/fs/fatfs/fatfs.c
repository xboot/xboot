/*
 * kernel/fs/fatfs/fatfs.c
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
#include <types.h>
#include <ctype.h>
#include <stdarg.h>
#include <vsprintf.h>
#include <malloc.h>
#include <error.h>
#include <bitpos.h>
#include <time/xtime.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/scank.h>
#include <xboot/chrdev.h>
#include <xboot/blkdev.h>
#include <xboot/device.h>
#include <fs/vfs/vfs.h>
#include <fs/fs.h>

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
	x_u8	jmp_instruction[3];
	x_u8	oem_name[8];

	/*
	 * the common part of the fat12, fat16 and fat32 bios parameter block
	 */
	x_u8	bytes_per_sector[2];
	x_u8	sectors_per_cluster;
	x_u8	reserved_sectors[2];
	x_u8	num_of_fats;
	x_u8	root_entries[2];
	x_u8	total_sectors[2];
	x_u8	media_descriptor;
	x_u8	sectors_per_fat[2];
	x_u8	sectors_per_track[2];
	x_u8	num_of_heads[2];
	x_u8	hidden_sectors[4];
	x_u8	big_total_sectors[4];

	/*
	 * the last part of fat12, fat16 and fat32
	 */
	union {
		x_u8 code[474];

		struct {
			x_u8	drive_number;
			x_u8	reserved;
			x_u8	boot_signature;
			x_u8	volume_id[4];
			x_u8	volume_lab[11];
			x_u8	fs_type[8];

			x_u8 	fill[474 - 26];
		}fat12;

		struct {
			x_u8	drive_number;
			x_u8	reserved;
			x_u8	boot_signature;
			x_u8	volume_id[4];
			x_u8	volume_lab[11];
			x_u8	fs_type[8];

			x_u8 	fill[474 - 26];
		}fat16;

		struct {
			x_u8	sectors_per_fat_32[4];
			x_u8	ext_flags[2];
			x_u8	fs_version[2];
			x_u8	root_clus[4];
			x_u8	fs_info[2];
			x_u8	back_boot_sector[2];
			x_u8	reserved_before[12];

			x_u8	driver_number;
			x_u8	reserved;
			x_u8	boot_signature;
			x_u8	volume_id[4];
			x_u8	volume_lab[11];
			x_u8	fs_type[8];

			x_u8 	fill[474 - 54];
		}fat32;
	} x;

	/*
	 * the signature 0x55, 0xaa
	 */
	x_u8	signature[2];
} __attribute__ ((packed));

/*
 * fat directory entry
 */
struct fat_dirent {
	x_u8	name[11];
	x_u8	attr;
	x_u8	reserve[10];
	x_u8	time[2];
	x_u8	date[2];
	x_u8	cluster[2];
	x_u8	size[4];
} __attribute__ ((packed));

/*
 * file / directory node
 */
struct fat_node {
	struct fat_dirent	dirent;		/* copy of directory entry */
	x_u32				sector;		/* sector for directory entry */
	x_u32				offset;		/* offset of directory entry in sector */
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
	x_u32 sector_size;

	/* sectors per cluster */
	x_u32 sectors_per_cluster;

	/* cluster size */
	x_u32 cluster_size;

	/* start sector for fat entries */
	x_u32 fat_start;

	/* start sector for root directory */
	x_u32 root_start;

	/* start sector for data */
	x_u32 data_start;

	/* last cluser */
	x_u32 last_cluster;

	/* start cluster to free search */
	x_u32 free_scan;

	/* mask for cluster */
	x_u32 fat_mask;

	/* id of end cluster */
	x_u32 fat_eof;

	/* vnode for root */
	struct vnode * root_vnode;

	/* local data buffer */
	char * io_buf;

	/* buffer for fat entry */
	char * fat_buf;

	/* buffer for directory entry */
	char * dir_buf;

	/* mounted block device */
	struct blkdev * blk;
};

/*
 * filesystem operations
 */
static x_s32 fatfs_mount(struct mount * m, char * dev, x_s32 flag)
{
	struct fatfs_mount_data * md;
	struct blkdev * blk;
	struct fat_boot_sector fbs;
	x_u32 sector_size;
	x_u32 tmp;

	if(dev == NULL)
		return EINVAL;

	blk = (struct blkdev *)m->m_dev;
	if(!blk || !blk->info)
		return ENODEV;

	if(get_blkdev_total_size(blk) <= sizeof(struct fat_boot_sector))
		return EINTR;

	if(bio_read(blk, (x_u8 *)(&fbs), 0, sizeof(struct fat_boot_sector)) != sizeof(struct fat_boot_sector))
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
	if( strncmp((const x_s8 *)fbs.x.fat12.fs_type, (const x_s8 *)"FAT12   ", 8) == 0 )
	{
		md->type = FAT_TYPE_FAT12;
	}
	else if( strncmp((const x_s8 *)fbs.x.fat16.fs_type, (const x_s8 *)"FAT16   ", 8) == 0 )
	{
		md->type = FAT_TYPE_FAT16;
	}
	else if( strncmp((const x_s8 *)fbs.x.fat32.fs_type, (const x_s8 *)"FAT32   ", 8) == 0 )
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
		printk("fat12\r\n");
		break;
	case FAT_TYPE_FAT16:
		printk("fat16\r\n");
		break;
	case FAT_TYPE_FAT32:
		printk("fat32\r\n");
		break;
	default:
		break;
	}

	printk("sector size: %ld\r\n", md->sector_size);
	printk("sectors per cluster: %ld\r\n", md->sectors_per_cluster);
	printk("cluster size: %ld\r\n", md->cluster_size);

	printk("fat start: %ld\r\n", md->fat_start);
	printk("root start: %ld\r\n", md->root_start);
	printk("data start: %ld\r\n", md->data_start);

	printk("last cluster: %ld\r\n", md->last_cluster);
	printk("free scan: %ld\r\n", md->free_scan);

	printk("fat mask: 0x%08x\r\n", md->fat_mask);
	printk("fat eof: 0x%08x\r\n", md->fat_eof);

	return 0;
}

static x_s32 fatfs_unmount(struct mount * m)
{
	struct fatfs_mount_data * md = m->m_data;

	free(m->m_root->v_data);

	free(md->dir_buf);
	free(md->fat_buf);
	free(md->io_buf);

	free(md);

	return 0;
}

static x_s32 fatfs_sync(struct mount * m)
{
	return 0;
}

static x_s32 fatfs_vget(struct mount * m, struct vnode * node)
{
	struct fat_node * n;

	n = malloc(sizeof(struct fat_node));
	if(!n)
		return ENOMEM;
	node->v_data = n;

	return 0;
}

static x_s32 fatfs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * convert file name to 8.3 format ("foo.bar" => "foo     bar")
 */
static void fat_convert_name(x_u8 * org, x_u8 * name)
{
	x_s32 i;

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
static void fat_restore_name(x_u8 * org, x_u8 * name)
{
	x_s32 i;

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
static x_bool fat_compare_name(x_u8 * n1, x_u8 * n2)
{
	x_s32 i;

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
static x_bool fat_valid_name(x_u8 * name)
{
	const x_u8 invalid_char[] = "*?<>|\"+=,;[] \345";
	x_s32 len = 0;

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
		if(strchr((const x_s8 *)invalid_char, *name))
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
		if(strchr((const x_s8 *)invalid_char, *name))
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
static x_bool read_fat_entry(struct fatfs_mount_data * md, x_u32 cl)
{
	return FALSE;
}

/*
 * get next cluster number of fat chain.
 */
static x_s32 fat_next_cluster(struct fatfs_mount_data * md, x_u32 cl, x_u32 * next)
{
	x_u32 offset;
	x_u32 val;

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
static x_bool fat_read_dirent(struct fatfs_mount_data * md, x_u32 sector)
{
	x_off off = sector * md->sector_size;
	x_size size = md->sector_size;

	if(bio_read(md->blk, (x_u8 *)(md->dir_buf), off, size) != size)
		return FALSE;

	return TRUE;
}

/*
 * write directory entry from buffer.
 */
static x_bool fat_write_dirent(struct fatfs_mount_data * md, x_u32 sector)
{
	x_off off = sector * md->sector_size;
	x_size size = md->sector_size;

	if(bio_write(md->blk, (const x_u8 *)(md->dir_buf), off, size) != size)
		return FALSE;

	return TRUE;
}

/*
 * find directory entry in specified sector.
 */
static x_s32 fat_lookup_dirent(struct fatfs_mount_data * md, x_u32 sec, x_u8 * name, struct fat_node * np)
{
	struct fat_dirent * de;
	x_s32 i, num;

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
static x_s32 fat_lookup_node(struct vnode * dnode, x_u8 * name, struct fat_node * np)
{
	struct fatfs_mount_data * md;
	x_u8 fat_name[12];
	x_u32 cl, sec;
	x_s32 i, err;

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
static x_s32 fatfs_open(struct vnode * node, x_s32 flag)
{
	return 0;
}

static x_s32 fatfs_close(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 fatfs_read(struct vnode * node, struct file * fp, void * buf, x_size size, x_size * result)
{
	return -1;
}

static x_s32 fatfs_write(struct vnode * node , struct file * fp, void * buf, x_size size, x_size * result)
{
	return -1;
}

static x_s32 fatfs_seek(struct vnode * node, struct file * fp, x_off off1, x_off off2)
{
	return 0;
}

static x_s32 fatfs_ioctl(struct vnode * node, struct file * fp, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 fatfs_fsync(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 fatfs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	struct fatfs_mount_data * md;
	struct fat_node np;
	struct fat_dirent * de;
	x_s32 err;

	printk("%s\r\n", __FUNCTION__);

	md = node->v_mount->m_data;
//TODO
//	err = fatfs_get_node(node, fp->f_offset, &np);
	if(err != 0)
		return err;

	de = &np.dirent;
	fat_restore_name((x_u8 *)&de->name, (x_u8 *)dir->d_name);

	if(IS_DIR(de))
		dir->d_type = DT_DIR;
	else if(IS_FILE(de))
		dir->d_type = DT_REG;
	else
		dir->d_type = DT_UNKNOWN;

	dir->d_fileno = fp->f_offset;
	dir->d_namlen = strlen((const x_s8 *)dir->d_name);
	fp->f_offset++;

	return 0;
}

static x_s32 fatfs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
	struct fatfs_mount_data * md;
	struct fat_node * np;
	struct fat_dirent * de;
	x_s32 err;

	printk("%s\r\n", __FUNCTION__);

	if(*name == '\0')
		return ENOENT;

	md = node->v_mount->m_data;

	np = node->v_data;
	err = fat_lookup_node(dnode, (x_u8 *)name, np);
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

static x_s32 fatfs_create(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 fatfs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 fatfs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static x_s32 fatfs_mkdir(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 fatfs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 fatfs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 fatfs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 fatfs_inactive(struct vnode * node)
{
	return -1;
}

static x_s32 fatfs_truncate(struct vnode * node, x_off length)
{
	return -1;
}

/*
 * fatfs vnode operations
 */
static struct vnops fatfs_vnops = {
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
static struct vfsops fatfs_vfsops = {
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
static struct filesystem fatfs = {
	.name		= "fatfs",
	.vfsops		= &fatfs_vfsops,
};

static __init void filesystem_fatfs_init(void)
{
	if(!filesystem_register(&fatfs))
		LOG_E("register 'fatfs' filesystem fail");
}

static __exit void filesystem_fatfs_exit(void)
{
	if(!filesystem_unregister(&fatfs))
		LOG_E("unregister 'fatfs' filesystem fail");
}

module_init(filesystem_fatfs_init, LEVEL_POSTCORE);
module_exit(filesystem_fatfs_exit, LEVEL_POSTCORE);
