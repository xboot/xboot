/*
 * kernel/vfs/fat/fat-control.c
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

#include <vfs/fat/fat-control.h>

static int __fatfs_control_flush_fat_cache(struct fatfs_control_t * ctrl, u32_t index)
{
	u32_t i, sect_num;
	u64_t fat_base, len;

	if(!ctrl->fat_cache_dirty[index])
		return 0;

	for(i = 0; i < ctrl->number_of_fat; i++)
	{
		fat_base = ((u64_t)ctrl->first_fat_sector + (i * ctrl->sectors_per_fat)) * ctrl->bytes_per_sector;
		sect_num = ctrl->fat_cache_num[index];
		len = block_write(ctrl->bdev, &ctrl->fat_cache_buf[index * ctrl->bytes_per_sector], fat_base + sect_num * ctrl->bytes_per_sector,
		        ctrl->bytes_per_sector);
		if(len != ctrl->bytes_per_sector)
			return -1;
	}
	ctrl->fat_cache_dirty[index] = FALSE;

	return 0;
}

static int __fatfs_control_find_fat_cache(struct fatfs_control_t * ctrl, u32_t sect_num)
{
	int index;

	for(index = 0; index < FAT_TABLE_CACHE_SIZE; index++)
	{
		if(ctrl->fat_cache_num[index] == sect_num)
			return index;
	}
	return -1;
}

static int __fatfs_control_load_fat_cache(struct fatfs_control_t * ctrl, u32_t sect_num)
{
	int rc;
	u32_t index;
	u64_t fat_base, len;

	if(-1 < __fatfs_control_find_fat_cache(ctrl, sect_num))
		return 0;

	index = ctrl->fat_cache_victim;
	ctrl->fat_cache_victim++;
	if(ctrl->fat_cache_victim == FAT_TABLE_CACHE_SIZE)
		ctrl->fat_cache_victim = 0;

	rc = __fatfs_control_flush_fat_cache(ctrl, index);
	if(rc)
		return rc;

	fat_base = (u64_t) ctrl->first_fat_sector * ctrl->bytes_per_sector;
	len = block_read(ctrl->bdev, &ctrl->fat_cache_buf[index * ctrl->bytes_per_sector], fat_base + sect_num * ctrl->bytes_per_sector, ctrl->bytes_per_sector);
	if(len != ctrl->bytes_per_sector)
		return -1;
	ctrl->fat_cache_num[index] = sect_num;

	return 0;
}

static u32_t __fatfs_control_read_fat_cache(struct fatfs_control_t * ctrl, u8_t * buf, u32_t pos)
{
	int rc, index;
	u32_t ret, sect_num, sect_off;

	if((ctrl->sectors_per_fat * ctrl->bytes_per_sector) <= pos)
		return 0;

	sect_num = udiv32(pos, ctrl->bytes_per_sector);
	sect_off = pos - (sect_num * ctrl->bytes_per_sector);

	rc = __fatfs_control_load_fat_cache(ctrl, sect_num);
	if(rc)
		return 0;

	index = __fatfs_control_find_fat_cache(ctrl, sect_num);
	if(index < 0)
		return 0;

	index = (index * ctrl->bytes_per_sector) + sect_off;

	switch(ctrl->type)
	{
	case FAT_TYPE_12:
	case FAT_TYPE_16:
		ret = 2;
		buf[0] = ctrl->fat_cache_buf[index + 0];
		buf[1] = ctrl->fat_cache_buf[index + 1];
		break;
	case FAT_TYPE_32:
		ret = 4;
		buf[0] = ctrl->fat_cache_buf[index + 0];
		buf[1] = ctrl->fat_cache_buf[index + 1];
		buf[2] = ctrl->fat_cache_buf[index + 2];
		buf[3] = ctrl->fat_cache_buf[index + 3];
		break;
	default:
		ret = 0;
		break;
	};

	return ret;
}

static u32_t __fatfs_control_write_fat_cache(struct fatfs_control_t * ctrl, u8_t * buf, u32_t pos)
{
	int rc, index, cache_buf_index;
	u32_t ret, sect_num, sect_off;

	if((ctrl->sectors_per_fat * ctrl->bytes_per_sector) <= pos)
		return 0;

	sect_num = udiv32(pos, ctrl->bytes_per_sector);
	sect_off = pos - (sect_num * ctrl->bytes_per_sector);

	rc = __fatfs_control_load_fat_cache(ctrl, sect_num);
	if(rc)
		return 0;

	index = __fatfs_control_find_fat_cache(ctrl, sect_num);
	if(index < 0)
		return 0;

	cache_buf_index = (index * ctrl->bytes_per_sector) + sect_off;

	switch(ctrl->type)
	{
	case FAT_TYPE_12:
	case FAT_TYPE_16:
		ret = 2;
		ctrl->fat_cache_buf[cache_buf_index + 0] = buf[0];
		ctrl->fat_cache_buf[cache_buf_index + 1] = buf[1];
		break;
	case FAT_TYPE_32:
		ret = 4;
		ctrl->fat_cache_buf[cache_buf_index + 0] = buf[0];
		ctrl->fat_cache_buf[cache_buf_index + 1] = buf[1];
		ctrl->fat_cache_buf[cache_buf_index + 2] = buf[2];
		ctrl->fat_cache_buf[cache_buf_index + 3] = buf[3];
		break;
	default:
		ret = 0;
		break;
	};
	ctrl->fat_cache_dirty[index] = TRUE;

	return ret;
}

static u32_t __fatfs_control_first_valid_cluster(struct fatfs_control_t * ctrl)
{
	switch(ctrl->type)
	{
	case FAT_TYPE_12:
		return FAT12_RESERVED1_CLUSTER + 1;
		break;
	case FAT_TYPE_16:
		return FAT16_RESERVED1_CLUSTER + 1;
		break;
	case FAT_TYPE_32:
		return FAT32_RESERVED1_CLUSTER + 1;
		break;
	};
	return 0x0;
}

static u32_t __fatfs_control_last_valid_cluster(struct fatfs_control_t * ctrl)
{
	switch(ctrl->type)
	{
	case FAT_TYPE_12:
		return FAT12_RESERVED2_CLUSTER - 1;
		break;
	case FAT_TYPE_16:
		return FAT16_RESERVED2_CLUSTER - 1;
		break;
	case FAT_TYPE_32:
		return FAT32_RESERVED2_CLUSTER - 1;
		break;
	}
	return 0x0;
}

static bool_t __fatfs_control_valid_cluster(struct fatfs_control_t * ctrl, u32_t cl)
{
	switch(ctrl->type)
	{
	case FAT_TYPE_12:
		if((cl <= FAT12_RESERVED1_CLUSTER) || (FAT12_RESERVED2_CLUSTER <= cl))
			return FALSE;
		break;
	case FAT_TYPE_16:
		if((cl <= FAT16_RESERVED1_CLUSTER) || (FAT16_RESERVED2_CLUSTER <= cl))
			return FALSE;
		break;
	case FAT_TYPE_32:
		if((cl <= FAT32_RESERVED1_CLUSTER) || (FAT32_RESERVED2_CLUSTER <= cl))
			return FALSE;
		break;
	};
	return TRUE;
}

static int __fatfs_control_get_next_cluster(struct fatfs_control_t * ctrl, u32_t clust, u32_t * next)
{
	u8_t fat_entry_b[4] = { 0 };
	u32_t fat_entry, fat_off, fat_len, len;

	if(!__fatfs_control_valid_cluster(ctrl, clust))
		return -1;

	switch(ctrl->type)
	{
	case FAT_TYPE_12:
		if(clust % 2)
			fat_off = (clust - 1) * 12 / 8 + 1;
		else
			fat_off = clust * 12 / 8;
		fat_len = 2;
		break;
	case FAT_TYPE_16:
		fat_off = clust * 2;
		fat_len = 2;
		break;
	case FAT_TYPE_32:
		fat_off = clust * 4;
		fat_len = 4;
		break;
	default:
		return -1;
	};

	len = __fatfs_control_read_fat_cache(ctrl, &fat_entry_b[0], fat_off);
	if(len != fat_len)
		return -1;

	if(fat_len == 2)
		fat_entry = ((u32_t) fat_entry_b[1] << 8) | ((u32_t) fat_entry_b[0]);
	else
		fat_entry = ((u32_t) fat_entry_b[3] << 24) | ((u32_t) fat_entry_b[2] << 16) | ((u32_t) fat_entry_b[1] << 8) | ((u32_t) fat_entry_b[0]);

	if(ctrl->type == FAT_TYPE_12)
	{
		if(clust % 2)
			fat_entry >>= 4;
		else
			fat_entry &= 0xFFF;
	}

	if(next)
		*next = fat_entry;

	return 0;
}

static int __fatfs_control_set_next_cluster(struct fatfs_control_t * ctrl, u32_t clust, u32_t next)
{
	u8_t fat_entry_b[4];
	u32_t fat_entry, fat_off, fat_len, len;

	if(!__fatfs_control_valid_cluster(ctrl, clust))
		return -1;

	switch(ctrl->type)
	{
	case FAT_TYPE_12:
		if(clust % 2)
			fat_off = (clust - 1) * 12 / 8 + 1;
		else
			fat_off = clust * 12 / 8;
		fat_len = 2;
		len = __fatfs_control_read_fat_cache(ctrl, &fat_entry_b[0], fat_off);
		if(len != fat_len)
			return -1;
		fat_entry = ((u32_t) fat_entry_b[1] << 8) | fat_entry_b[0];
		if(clust % 2)
		{
			fat_entry &= ~0xFFF0;
			fat_entry |= (next & 0xFFF) << 4;
		}
		else
		{
			fat_entry &= ~0xFFF;
			fat_entry |= (next & 0xFFF);
		}
		fat_entry_b[1] = (fat_entry >> 8) & 0xFF;
		fat_entry_b[0] = (fat_entry) & 0xFF;
		break;
	case FAT_TYPE_16:
		fat_off = clust * 2;
		fat_len = 2;
		fat_entry = next & 0xFFFF;
		fat_entry_b[1] = (fat_entry >> 8) & 0xFF;
		fat_entry_b[0] = (fat_entry) & 0xFF;
		break;
	case FAT_TYPE_32:
		fat_off = clust * 4;
		fat_len = 4;
		fat_entry = next & 0xFFFFFFFF;
		fat_entry_b[3] = (fat_entry >> 24) & 0xFF;
		fat_entry_b[2] = (fat_entry >> 16) & 0xFF;
		fat_entry_b[1] = (fat_entry >> 8) & 0xFF;
		fat_entry_b[0] = (fat_entry) & 0xFF;
		break;
	default:
		return -1;
	};

	len = __fatfs_control_write_fat_cache(ctrl, &fat_entry_b[0], fat_off);
	if(len != fat_len)
		return -1;

	return 0;
}

static int __fatfs_control_set_last_cluster(struct fatfs_control_t * ctrl, u32_t clust)
{
	int rc;

	if(!__fatfs_control_valid_cluster(ctrl, clust))
		return -1;

	switch(ctrl->type)
	{
	case FAT_TYPE_12:
		rc = __fatfs_control_set_next_cluster(ctrl, clust, FAT12_LAST_CLUSTER);
		break;
	case FAT_TYPE_16:
		rc = __fatfs_control_set_next_cluster(ctrl, clust, FAT16_LAST_CLUSTER);
		break;
	case FAT_TYPE_32:
		rc = __fatfs_control_set_next_cluster(ctrl, clust, FAT32_LAST_CLUSTER);
		break;
	default:
		rc = -1;
		break;
	};

	return rc;
}

static int __fatfs_control_nth_cluster(struct fatfs_control_t * ctrl, u32_t clust, u32_t pos, u32_t * next)
{
	int rc;
	u32_t i;

	if(next)
		*next = clust;

	if(!__fatfs_control_valid_cluster(ctrl, clust))
		return -1;

	for(i = 0; i < pos; i++)
	{
		rc = __fatfs_control_get_next_cluster(ctrl, clust, &clust);
		if(rc)
			return rc;

		if(next)
			*next = clust;

		if(!__fatfs_control_valid_cluster(ctrl, clust))
			return -1;
	}
	return 0;
}

static int __fatfs_control_alloc_cluster(struct fatfs_control_t * ctrl, u32_t clust, u32_t * newclust)
{
	int rc;
	bool_t found;
	u32_t current, next, first, last;

	found = FALSE;

	if(__fatfs_control_valid_cluster(ctrl, clust))
	{
		first = clust;
	}
	else
	{
		first = __fatfs_control_first_valid_cluster(ctrl);
	}

	last = __fatfs_control_last_valid_cluster(ctrl);
	for(current = first; current <= last; current++)
	{
		rc = __fatfs_control_get_next_cluster(ctrl, current, &next);
		if(rc)
			return rc;

		if(next == 0x0)
		{
			found = TRUE;
			break;
		}
	}

	if(!found)
		return -1;

	rc = __fatfs_control_set_last_cluster(ctrl, current);
	if(rc)
		return rc;

	if(newclust)
		*newclust = current;

	return 0;
}

static int __fatfs_control_append_free_cluster(struct fatfs_control_t *ctrl, u32_t clust, u32_t *newclust)
{
	int rc;

	rc = __fatfs_control_alloc_cluster(ctrl, clust, newclust);
	if(rc)
		return rc;

	rc = __fatfs_control_set_next_cluster(ctrl, clust, *newclust);
	if(rc)
		return rc;

	return 0;
}

static int __fatfs_control_truncate_clusters(struct fatfs_control_t *ctrl, u32_t clust)
{
	int rc;
	u32_t current, next = clust;

	while(__fatfs_control_valid_cluster(ctrl, next))
	{
		current = next;

		rc = __fatfs_control_get_next_cluster(ctrl, current, &next);
		if(rc)
			return rc;

		rc = __fatfs_control_set_next_cluster(ctrl, current, 0x0);
		if(rc)
			return rc;
	}

	return 0;
}

static s64_t fatfs_wallclock_mktime(unsigned int year, unsigned int mon, unsigned int day, unsigned int hour, unsigned int min, unsigned int sec)
{
	struct tm ti;
	time_t t;
	ti.tm_sec = sec;
	ti.tm_min = min;
	ti.tm_hour = hour;
	ti.tm_mday = day;
	ti.tm_mon = mon;
	ti.tm_year = year;

	t = mktime(&ti);

	return (s64_t)t;
}

u32_t fatfs_pack_timestamp(u32_t year, u32_t mon, u32_t day, u32_t hour, u32_t min, u32_t sec)
{
	return (u32_t)fatfs_wallclock_mktime(1980 - 1900 + year, mon, day, hour, min, sec);
}

void fatfs_current_timestamp(u32_t * year, u32_t * mon, u32_t * day, u32_t * hour, u32_t * min, u32_t * sec)
{
	struct tm * ti;
	time_t t;

	time(&t);
	ti = localtime(&t);

	if(year)
	{
		if(ti->tm_year < 80)
			*year = 0;
		else
			*year = ti->tm_year + 1900 - 1980;
	}
	if(mon)
		*mon = ti->tm_mon;
	if(day)
		*day = ti->tm_mday;
	if(hour)
		*hour = ti->tm_hour;
	if(min)
		*min = ti->tm_min;
	if(sec)
		*sec = ti->tm_sec;
}

void fatfs_timestamp(time_t *t, u32_t * year, u32_t * mon, u32_t * day, u32_t * hour, u32_t * min, u32_t * sec)
{
	struct tm * ti;
	ti = localtime(t);

	if(year)
	{
		if(ti->tm_year < 80)
			*year = 0;
		else
			*year = ti->tm_year + 1900 - 1980;
	}
	if(mon)
		*mon = ti->tm_mon;
	if(day)
		*day = ti->tm_mday;
	if(hour)
		*hour = ti->tm_hour;
	if(min)
		*min = ti->tm_min;
	if(sec)
		*sec = ti->tm_sec;
}

bool_t fatfs_control_valid_cluster(struct fatfs_control_t * ctrl, u32_t clust)
{
	return __fatfs_control_valid_cluster(ctrl, clust);
}

int fatfs_control_nth_cluster(struct fatfs_control_t * ctrl, u32_t clust, u32_t pos, u32_t * next)
{
	int rc;

	mutex_lock(&ctrl->fat_cache_lock);
	rc = __fatfs_control_nth_cluster(ctrl, clust, pos, next);
	mutex_unlock(&ctrl->fat_cache_lock);

	return rc;
}

int fatfs_control_set_last_cluster(struct fatfs_control_t * ctrl, u32_t clust)
{
	int rc;

	mutex_lock(&ctrl->fat_cache_lock);
	rc = __fatfs_control_set_last_cluster(ctrl, clust);
	mutex_unlock(&ctrl->fat_cache_lock);

	return rc;
}

int fatfs_control_alloc_first_cluster(struct fatfs_control_t * ctrl, u32_t * newclust)
{
	int rc;

	mutex_lock(&ctrl->fat_cache_lock);
	rc = __fatfs_control_alloc_cluster(ctrl, 0, newclust);
	mutex_unlock(&ctrl->fat_cache_lock);

	return rc;
}

int fatfs_control_append_free_cluster(struct fatfs_control_t * ctrl, u32_t clust, u32_t * newclust)
{
	int rc;

	mutex_lock(&ctrl->fat_cache_lock);
	rc = __fatfs_control_append_free_cluster(ctrl, clust, newclust);
	mutex_unlock(&ctrl->fat_cache_lock);

	return rc;
}

int fatfs_control_truncate_clusters(struct fatfs_control_t * ctrl, u32_t clust)
{
	int rc;

	mutex_lock(&ctrl->fat_cache_lock);
	rc = __fatfs_control_truncate_clusters(ctrl, clust);
	mutex_unlock(&ctrl->fat_cache_lock);

	return rc;
}

int fatfs_control_sync(struct fatfs_control_t * ctrl)
{
	int rc, index;

	/* Flush entire FAT sector cache */
	mutex_lock(&ctrl->fat_cache_lock);
	for(index = 0; index < FAT_TABLE_CACHE_SIZE; index++)
	{
		rc = __fatfs_control_flush_fat_cache(ctrl, index);
		if(rc)
		{
			mutex_unlock(&ctrl->fat_cache_lock);
			return rc;
		}
	}
	mutex_unlock(&ctrl->fat_cache_lock);

	/* Flush cached data in device request queue */
	block_sync(ctrl->bdev);

	return 0;
}

int fatfs_control_init(struct fatfs_control_t * ctrl, struct block_t * bdev)
{
	u32_t i;
	u64_t rlen;
	struct fat_bootsec_t *bsec = &ctrl->bsec;

	/* Read boot sector from block device */
	rlen = block_read(bdev, (u8_t *) bsec,
	FAT_BOOTSECTOR_OFFSET, sizeof(struct fat_bootsec_t));
	if(rlen != sizeof(struct fat_bootsec_t))
		return -1;

	/* Save underlying block device pointer */
	ctrl->bdev = bdev;

	/* Get bytes_per_sector and sector_per_cluster */
	ctrl->bytes_per_sector = le16_to_cpu(bsec->bytes_per_sector);
	ctrl->sectors_per_cluster = bsec->sectors_per_cluster;

	/* Sanity check bytes_per_sector and sector_per_cluster */
	if(!ctrl->bytes_per_sector || !ctrl->sectors_per_cluster)
		return -1;

	/* Frequently required info */
	ctrl->number_of_fat = bsec->number_of_fat;
	ctrl->bytes_per_cluster = ctrl->sectors_per_cluster * ctrl->bytes_per_sector;
	ctrl->total_sectors = le16_to_cpu(bsec->total_sectors_16);
	if(!ctrl->total_sectors)
		ctrl->total_sectors = le32_to_cpu(bsec->total_sectors_32);

	/* Calculate derived info assuming FAT12/FAT16 */
	ctrl->first_fat_sector = le16_to_cpu(bsec->reserved_sector_count);
	ctrl->sectors_per_fat = le16_to_cpu(bsec->sectors_per_fat);
	ctrl->fat_sectors = ctrl->number_of_fat * ctrl->sectors_per_fat;

	ctrl->first_root_sector = ctrl->first_fat_sector + ctrl->fat_sectors;
	ctrl->root_sectors = (le16_to_cpu(bsec->root_entry_count) * 32) + (ctrl->bytes_per_sector - 1);
	ctrl->root_sectors = udiv32(ctrl->root_sectors, ctrl->bytes_per_sector);
	ctrl->first_root_cluster = 0;

	ctrl->first_data_sector = ctrl->first_root_sector + ctrl->root_sectors;
	ctrl->data_sectors = ctrl->total_sectors - ctrl->first_data_sector;
	ctrl->data_clusters = udiv32(ctrl->data_sectors, ctrl->sectors_per_cluster);

	/* Determine FAT type */
	if(ctrl->data_clusters < 4085)
		ctrl->type = FAT_TYPE_12;
	else if(ctrl->data_clusters < 65525)
		ctrl->type = FAT_TYPE_16;
	else
		ctrl->type = FAT_TYPE_32;

	/* FAT type sanity check */
	switch(ctrl->type)
	{
	case FAT_TYPE_12:
		if(memcmp(bsec->ext.e16.fs_type, "FAT12", 5))
			return -1;
		break;
	case FAT_TYPE_16:
		if(memcmp(bsec->ext.e16.fs_type, "FAT16", 5))
			return -1;
		break;
	case FAT_TYPE_32:
		if(memcmp(bsec->ext.e32.fs_type, "FAT32", 5))
			return -1;
		break;
	default:
		return -1;
	}

	/* For FAT32, recompute derived info */
	if(ctrl->type == FAT_TYPE_32)
	{
		ctrl->first_fat_sector = le16_to_cpu(bsec->reserved_sector_count);
		ctrl->sectors_per_fat = le32_to_cpu(bsec->ext.e32.sectors_per_fat);
		ctrl->fat_sectors = ctrl->number_of_fat * ctrl->sectors_per_fat;

		ctrl->first_root_sector = 0;
		ctrl->root_sectors = 0;
		ctrl->first_root_cluster = le32_to_cpu(bsec->ext.e32.root_directory_cluster);

		ctrl->first_data_sector = ctrl->first_fat_sector + ctrl->fat_sectors;
		ctrl->data_sectors = ctrl->total_sectors - ctrl->first_data_sector;
		ctrl->data_clusters = udiv32(ctrl->data_sectors, ctrl->sectors_per_cluster);
	}

	/* Initialize fat cache */
	mutex_init(&ctrl->fat_cache_lock);
	ctrl->fat_cache_victim = 0;
	for(i = 0; i < FAT_TABLE_CACHE_SIZE; i++)
	{
		ctrl->fat_cache_dirty[i] = FALSE;
		ctrl->fat_cache_num[i] = i;
	}
	ctrl->fat_cache_buf = calloc(1, FAT_TABLE_CACHE_SIZE * ctrl->bytes_per_sector);
	if(!ctrl->fat_cache_buf)
		return -1;

	/* Load fat cache */
	rlen = block_read(ctrl->bdev, ctrl->fat_cache_buf, ctrl->first_fat_sector * ctrl->bytes_per_sector,
	FAT_TABLE_CACHE_SIZE * ctrl->bytes_per_sector);
	if(rlen != (FAT_TABLE_CACHE_SIZE * ctrl->bytes_per_sector))
	{
		free(ctrl->fat_cache_buf);
		return -1;
	}

	return 0;
}

int fatfs_control_exit(struct fatfs_control_t * ctrl)
{
	free(ctrl->fat_cache_buf);
	return 0;
}
