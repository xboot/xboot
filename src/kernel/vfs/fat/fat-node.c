/*
 * kernel/vfs/fat/fat-node.c
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
#include <vfs/fat/fat-node.h>

static int fatfs_node_sync_cached_cluster(struct fatfs_node_t * node)
{
	u64_t wlen, woff;
	struct fatfs_control_t *ctrl = node->ctrl;

	if(node->cached_dirty && fatfs_control_valid_cluster(ctrl, node->cached_clust))
	{
		woff = (u64_t) ctrl->first_data_sector * ctrl->bytes_per_sector;
		woff += (u64_t) (node->cached_clust - 2) * ctrl->bytes_per_cluster;

		wlen = block_write(ctrl->bdev, node->cached_data, woff, ctrl->bytes_per_cluster);
		if(wlen != ctrl->bytes_per_cluster)
			return -1;

		node->cached_dirty = FALSE;
	}
	return 0;
}

static int fatfs_node_sync_parent_dent(struct fatfs_node_t * node)
{
	int rc;
	u32_t woff, wlen;

	if(!node->parent || !node->parent_dent_dirty)
		return 0;

	woff = node->parent_dent_off + node->parent_dent_len - sizeof(node->parent_dent);
	if(woff < node->parent_dent_off)
		return -1;

	wlen = fatfs_node_write(node->parent, woff, sizeof(node->parent_dent), (u8_t *) &node->parent_dent);
	if(wlen != sizeof(node->parent_dent))
		return -1;

	rc = fatfs_node_sync_cached_cluster(node->parent);
	if(rc)
		return rc;
	node->parent_dent_dirty = FALSE;
	return 0;
}

u32_t fatfs_node_get_size(struct fatfs_node_t * node)
{
	if(!node)
		return 0;
	return le32_to_cpu(node->parent_dent.file_size);
}

static int fatfs_node_alloc_cached_cluster(struct fatfs_node_t * node, u32_t clust, bool_t read)
{
	struct fatfs_control_t * ctrl = node->ctrl;
	u64_t woff, wlen;

	/* Allocate cached cluster memory if not already allocated */
	if(!node->cached_data)
	{
		node->cached_data = calloc(1, ctrl->bytes_per_cluster);
		if(!node->cached_data)
			return 0;
		node->cached_clust = 0;
	}

	if(node->cached_clust != clust)
	{
		/* Sync and zero-out cached cluster buffer */
		if(fatfs_control_valid_cluster(ctrl, node->cached_clust))
		{
			if(fatfs_node_sync_cached_cluster(node))
				return 0;
			node->cached_clust = 0;
			memset(node->cached_data, 0, ctrl->bytes_per_cluster);
		}

		if(read == TRUE)
		{
			/* Write zeros to new cluster */
			woff = (u64_t) ctrl->first_data_sector * ctrl->bytes_per_sector;
			woff += (u64_t) (clust - 2) * ctrl->bytes_per_cluster;
			wlen = block_read(ctrl->bdev, node->cached_data, woff, ctrl->bytes_per_cluster);
			if(wlen != ctrl->bytes_per_cluster)
				return -1;
		}
	}
	node->cached_clust = clust;
	return 0;
}

static u64_t fatfs_node_write_cluster(struct fatfs_node_t * node, u32_t clust, u8_t * buf, u64_t off, u64_t count)
{
	int rc;
	struct fatfs_control_t * ctrl = node->ctrl;

	if(ctrl->bytes_per_cluster < count + off)
			return 0;

	rc = fatfs_node_alloc_cached_cluster(node, clust, count == ctrl->bytes_per_cluster ? FALSE : TRUE);

	if(rc)
		return rc;

	/* Write to cached cluster */
	memcpy(node->cached_data + off, buf, count);

	node->cached_dirty = TRUE;
	return count;
}

static u64_t fatfs_node_read_cluster(struct fatfs_node_t * node, u32_t clust, u8_t * buf, u64_t off, u64_t count)
{
	int rc;
	struct fatfs_control_t * ctrl = node->ctrl;

	if(ctrl->bytes_per_cluster < count + off)
		return 0;

	rc = fatfs_node_alloc_cached_cluster(node, clust, TRUE);

	if(rc)
		return rc;

	/* Read from cached cluster */
	memcpy(buf, node->cached_data + off, count);
	return count;
}

static int fatfs_node_clear_cluster(struct fatfs_node_t * node, u32_t clust)
{
	int rc;
	struct fatfs_control_t * ctrl = node->ctrl;

	rc = fatfs_node_alloc_cached_cluster(node, clust, FALSE);

	if(rc)
		return rc;

	/* init cluster */
	memset(node->cached_data, 0, ctrl->bytes_per_cluster);
	return 0;
}

static int fatfs_node_nth_cluster(struct fatfs_node_t * node, u32_t clust, u32_t pos, u32_t * next)
{
	return fatfs_control_nth_cluster(node->ctrl, clust, pos, next);
}

static int fatfs_node_next_cluster(struct fatfs_node_t * node, u32_t clust, u32_t * next)
{
	return fatfs_node_nth_cluster(node, clust, 1, next);
}

static int fatfs_node_auto_alloc_next_cluster(struct fatfs_node_t * node, u32_t clust, u32_t * next)
{
	int rc;
	struct fatfs_control_t * ctrl = node->ctrl;
	u32_t cl = clust;

	/* Go to next cluster */
	rc = fatfs_node_next_cluster(node, clust, next);
	if(!rc)
		return 0;

	/* Add new cluster */
	if(fatfs_control_valid_cluster(ctrl, clust))
	{
		rc = fatfs_control_append_free_cluster(ctrl, cl, next);
	}
	else
	{
		rc = fatfs_control_alloc_first_cluster(ctrl, next);
	}

	if(rc)
		return rc;

	rc = fatfs_node_clear_cluster(node, *next);

	if(rc)
		return rc;

	return 0;
}

static void fatfs_node_fast_jump_cluster(struct fatfs_node_t * node, u32_t pos, u32_t * cl_pos, u32_t * cl_num)
{
	struct fatfs_control_t *ctrl = node->ctrl;

	node->cur_pos = ctrl->bytes_per_cluster * udiv32(node->cur_pos, ctrl->bytes_per_cluster);

	/* Jump to current pos if pos >= cur_cluster*/
	if(pos >= node->cur_pos)
	{
		*cl_pos = udiv32(pos - node->cur_pos, ctrl->bytes_per_cluster);
		*cl_num = node->cur_cluster;
	}
	else
	{
		*cl_pos = udiv32(pos, ctrl->bytes_per_cluster);
		*cl_num = node->first_cluster;
		node->cur_cluster = node->first_cluster;
		node->cur_pos = 0;
	}
}

u32_t fatfs_node_read(struct fatfs_node_t * node, u32_t pos, u32_t len, u8_t * buf)
{
	u64_t roff, rlen;
	u32_t r, rstartcl;
	u32_t cl_off, cl_num, cl_len;
	struct fatfs_control_t *ctrl = node->ctrl;

	if(!node->parent && ctrl->type != FAT_TYPE_32)
	{
		rlen = (u64_t) ctrl->bytes_per_sector * ctrl->root_sectors;
		if(pos >= rlen)
			return 0;
		if((pos + len) > rlen)
			rlen = rlen - pos;
		else
			rlen = len;
		roff = (u64_t) ctrl->first_root_sector * ctrl->bytes_per_sector;
		roff += pos;
		return block_read(ctrl->bdev, (u8_t *) buf, roff, rlen);
	}

	fatfs_node_fast_jump_cluster(node, pos, &rstartcl, &cl_num);

	if(fatfs_node_nth_cluster(node, cl_num, rstartcl, &cl_num))
		return 0;

	r = 0;
	cl_off = umod32(pos + r, ctrl->bytes_per_cluster);
	do
	{
		/* Current cluster info */
		cl_len = ctrl->bytes_per_cluster - cl_off;
		cl_len = (len - r < cl_len) ? len - r : cl_len;

		/* Update current cluster */
		node->cur_cluster = cl_num;
		node->cur_pos = pos + r;

		/* Read from cached cluster */
		rlen = fatfs_node_read_cluster(node, cl_num, buf, cl_off, cl_len);

		if(rlen != cl_len)
		{
			break;
		}

		/* Update iteration */
		r += cl_len;
		buf += cl_len;
		cl_off -= cl_off;
	} while(r < len && !fatfs_node_next_cluster(node, cl_num, &cl_num));

	return r;
}

u32_t fatfs_node_write(struct fatfs_node_t * node, u32_t pos, u32_t len, u8_t * buf)
{
	int rc;
	u64_t woff, wlen;
	u32_t w = 0, wstartcl;
	u32_t cl_off, cl_num, cl_len;
	struct fatfs_control_t *ctrl = node->ctrl;

	if(!node->parent && ctrl->type != FAT_TYPE_32)
	{
		wlen = (u64_t) ctrl->bytes_per_sector * ctrl->root_sectors;
		if(pos >= wlen)
			return 0;
		if((pos + len) > wlen)
			wlen = wlen - pos;
		woff = (u64_t) ctrl->first_root_sector * ctrl->bytes_per_sector;
		woff += pos;
		return block_write(ctrl->bdev, (u8_t *) buf, woff, wlen);
	}

	/* If first cluster is zero then allocate first cluster */
	if(node->first_cluster == 0)
	{
		rc = fatfs_node_auto_alloc_next_cluster(node, 0, &cl_num);
		if(rc)
			return 0;

		node->first_cluster = cl_num;
		node->cur_cluster = node->first_cluster;
		node->cur_pos = 0;

		/* Mark node directory entry as dirty */
		node->parent_dent_dirty = TRUE;
	}

	fatfs_node_fast_jump_cluster(node, pos, &wstartcl, &cl_num);

	/* Make room for new data by appending free clusters */
	for(int i = 0; i < wstartcl; i++)
	{
		rc = fatfs_node_auto_alloc_next_cluster(node, cl_num, &cl_num);
		if(rc)
			return 0;
	}

	w = 0;
	cl_off = umod32(pos + w, ctrl->bytes_per_cluster);
	do
	{
		/* Current cluster info */
		cl_len = ctrl->bytes_per_cluster - cl_off;
		cl_len = (len - w < cl_len) ? len - w : cl_len;

		/* Update current cluster */
		node->cur_cluster = cl_num;
		node->cur_pos = pos + w;

		/* Write next cluster */
		wlen = fatfs_node_write_cluster(node, cl_num, buf, cl_off, cl_len);

		if(wlen != cl_len)
		{
			break;
		}

		/* Update iteration */
		w += cl_len;
		buf += cl_len;
		cl_off -= cl_off;
	} while(w < len && !fatfs_node_auto_alloc_next_cluster(node, cl_num, &cl_num));

	/* Mark node directory entry as dirty */
	node->parent_dent_dirty = TRUE;

	return w;
}

int fatfs_node_truncate(struct fatfs_node_t * node, u32_t pos)
{
	int rc;
	u32_t cl_pos, cl_num, t_pos;
	struct fatfs_control_t * ctrl = node->ctrl;

	if(!node->parent && ctrl->type != FAT_TYPE_32)
	{
		/* For FAT12 and FAT16 ignore root node truncation */
		return 0;
	}

	/* Determine last cluster after truncation */
	t_pos = udiv32(pos + ctrl->bytes_per_cluster - 1, ctrl->bytes_per_cluster);
	fatfs_node_fast_jump_cluster(node, t_pos + ctrl->bytes_per_cluster - 1, &cl_pos, &cl_num);

	rc = fatfs_control_nth_cluster(ctrl, cl_num, cl_pos, &cl_num);
	if(rc)
		return rc;

	/* Remove all clusters after last cluster */
	rc = fatfs_control_truncate_clusters(ctrl, cl_num);
	if(rc)
		return rc;

	/* If we are removing first cluster then set it to zero
	 * else set previous cluster as last cluster
	 */
	if(cl_pos == 0)
	{
		node->first_cluster = 0;
	}
	else
	{
		/* if go here, t_pos >= node->first_cluster */
		t_pos -= ctrl->bytes_per_cluster;
		fatfs_node_fast_jump_cluster(node, t_pos, &cl_pos, &cl_num);
		rc = fatfs_control_nth_cluster(ctrl, cl_num, cl_pos, &cl_num);
		if(rc)
			return rc;
		rc = fatfs_control_set_last_cluster(ctrl, cl_num);
		if(rc)
			return rc;
	}

	/* Mark node directory entry as dirty */
	node->parent_dent_dirty = TRUE;
	node->cur_cluster = node->first_cluster;
	node->cur_pos = 0;
	return 0;
}

int fatfs_node_sync(struct fatfs_node_t * node)
{
	int rc;

	rc = fatfs_node_sync_cached_cluster(node);
	if(rc)
		return rc;

	rc = fatfs_node_sync_parent_dent(node);
	if(rc)
		return rc;

	return 0;
}

int fatfs_node_init(struct fatfs_control_t * ctrl, struct fatfs_node_t * node)
{
	node->ctrl = ctrl;
	node->parent = NULL;
	node->parent_dent_off = 0;
	node->parent_dent_len = 0;
	memset(&node->parent_dent, 0, sizeof(struct fat_dirent_t));
	node->parent_dent_dirty = FALSE;
	node->first_cluster = 0;
	node->cur_cluster = 0;
	node->cur_pos = 0;

	node->cached_clust = 0;
	node->cached_data = NULL;
	node->cached_dirty = FALSE;


	return 0;
}

int fatfs_node_exit(struct fatfs_node_t * node)
{
	if(node->cached_data)
	{
		free(node->cached_data);
		node->cached_clust = 0;
		node->cached_data = NULL;
		node->cached_dirty = FALSE;
	}

	return 0;
}

static u8_t fatfs_node_lfn_checksum(const u8_t * name)
{
	int i;
	u8_t sum = 0;

	for(i = 11; i; i--)
	{
		sum = ((sum & 1) << 7) + (sum >> 1) + *name++;
	}

	return sum;
}

int fatfs_node_read_dirent(struct fatfs_node_t * dnode, s64_t off, struct vfs_dirent_t * d)
{
	u32_t i, rlen, len;
	u8_t lcsum = 0, dcsum = 0, check[11];
	char lname[VFS_MAX_NAME];
	struct fat_dirent_t dent;
	struct fat_longname_t lfn;
	u32_t fileoff = (u32_t) off;

	if(umod32(fileoff, sizeof(struct fat_dirent_t)))
	{
		return -1;
	}

	memset(lname, 0, sizeof(lname));
	d->d_off = off;
	d->d_reclen = 0;

	while(1)
	{
		rlen = fatfs_node_read(dnode, fileoff, sizeof(struct fat_dirent_t), (u8_t *) &dent);
		if(rlen != sizeof(struct fat_dirent_t))
			return -1;

		if(dent.dos_file_name[0] == 0x0)
			return -1;

		d->d_reclen += sizeof(struct fat_dirent_t);
		fileoff += sizeof(struct fat_dirent_t);

		if((dent.dos_file_name[0] == 0xE5) || (dent.dos_file_name[0] == 0x2E))
			continue;

		if(dent.file_attributes == FAT_LONGNAME_ATTRIBUTE)
		{
			memcpy(&lfn, &dent, sizeof(struct fat_longname_t));
			if(FAT_LONGNAME_LASTSEQ(lfn.seqno))
			{
				memset(lname, 0, sizeof(lname));
				lfn.seqno = FAT_LONGNAME_SEQNO(lfn.seqno);
				lcsum = lfn.checksum;
			}
			if((lfn.seqno < FAT_LONGNAME_MINSEQ) || (FAT_LONGNAME_MAXSEQ < lfn.seqno))
			{
				continue;
			}
			len = (lfn.seqno - 1) * 13;
			lname[len + 0] = (char) le16_to_cpu(lfn.name_utf16_1[0]);
			lname[len + 1] = (char) le16_to_cpu(lfn.name_utf16_1[1]);
			lname[len + 2] = (char) le16_to_cpu(lfn.name_utf16_1[2]);
			lname[len + 3] = (char) le16_to_cpu(lfn.name_utf16_1[3]);
			lname[len + 4] = (char) le16_to_cpu(lfn.name_utf16_1[4]);
			lname[len + 5] = (char) le16_to_cpu(lfn.name_utf16_2[0]);
			lname[len + 6] = (char) le16_to_cpu(lfn.name_utf16_2[1]);
			lname[len + 7] = (char) le16_to_cpu(lfn.name_utf16_2[2]);
			lname[len + 8] = (char) le16_to_cpu(lfn.name_utf16_2[3]);
			lname[len + 9] = (char) le16_to_cpu(lfn.name_utf16_2[4]);
			lname[len + 10] = (char) le16_to_cpu(lfn.name_utf16_2[5]);
			lname[len + 11] = (char) le16_to_cpu(lfn.name_utf16_3[0]);
			lname[len + 12] = (char) le16_to_cpu(lfn.name_utf16_3[1]);
			continue;
		}

		if(dent.file_attributes & FAT_DIRENT_VOLLABLE)
		{
			continue;
		}

		memcpy(check, dent.dos_file_name, 8);
		memcpy(&check[8], dent.dos_extension, 3);
		dcsum = fatfs_node_lfn_checksum(check);

		if(!strlen(lname))
		{
			i = 8;
			while(i && (dent.dos_file_name[i - 1] == ' '))
			{
				dent.dos_file_name[i - 1] = '\0';
				i--;
			}
			i = 3;
			while(i && (dent.dos_extension[i - 1] == ' '))
			{
				dent.dos_extension[i - 1] = '\0';
				i--;
			}

			for(i = 0; i < 8 && dent.dos_file_name[i]; i++)
			{
				lname[i] = tolower(dent.dos_file_name[i]);
			}
			lname[8] = '\0';

			if(dent.dos_extension[0] != '\0')
			{
				len = strlen(lname);
				lname[len] = '.';
				lname[len + 1] = tolower(dent.dos_extension[0]);
				lname[len + 2] = tolower(dent.dos_extension[1]);
				lname[len + 3] = tolower(dent.dos_extension[2]);
				lname[len + 4] = '\0';
			}

			lcsum = dcsum;
		}

		if(lcsum != dcsum)
			continue;

		if(strlcpy(d->d_name, lname, sizeof(d->d_name)) >= sizeof(d->d_name))
			return -1;

		break;
	}

	if(dent.file_attributes & FAT_DIRENT_SUBDIR)
		d->d_type = VDT_DIR;
	else
		d->d_type = VDT_REG;

	return 0;
}

int fatfs_node_find_dirent(struct fatfs_node_t * dnode, const char * name, struct fat_dirent_t * dent, u32_t * dent_off, u32_t * dent_len)
{
	u8_t lcsum = 0, dcsum = 0, check[11];
	u32_t i, off, rlen, len, lfn_off, lfn_len;
	struct fat_longname_t lfn;
	char lname[VFS_MAX_NAME];

	lfn_off = 0;
	lfn_len = 0;
	memset(lname, 0, sizeof(lname));

	off = 0;
	while(1)
	{
		rlen = fatfs_node_read(dnode, off, sizeof(struct fat_dirent_t), (u8_t *) dent);
		if(rlen != sizeof(struct fat_dirent_t))
			return -1;

		if(dent->dos_file_name[0] == 0x0)
			return -1;

		off += sizeof(struct fat_dirent_t);

		if((dent->dos_file_name[0] == 0xE5) || (dent->dos_file_name[0] == 0x2E))
			continue;

		if(dent->file_attributes == FAT_LONGNAME_ATTRIBUTE)
		{
			memcpy(&lfn, dent, sizeof(struct fat_longname_t));
			if(FAT_LONGNAME_LASTSEQ(lfn.seqno))
			{
				lfn.seqno = FAT_LONGNAME_SEQNO(lfn.seqno);
				lfn_off = off - sizeof(struct fat_dirent_t);
				lfn_len = lfn.seqno * sizeof(struct fat_longname_t);
				lcsum = lfn.checksum;
				memset(lname, 0, sizeof(lname));
			}
			if((lfn.seqno < FAT_LONGNAME_MINSEQ) || (FAT_LONGNAME_MAXSEQ < lfn.seqno))
			{
				continue;
			}
			len = (lfn.seqno - 1) * 13;
			lname[len + 0] = (char) le16_to_cpu(lfn.name_utf16_1[0]);
			lname[len + 1] = (char) le16_to_cpu(lfn.name_utf16_1[1]);
			lname[len + 2] = (char) le16_to_cpu(lfn.name_utf16_1[2]);
			lname[len + 3] = (char) le16_to_cpu(lfn.name_utf16_1[3]);
			lname[len + 4] = (char) le16_to_cpu(lfn.name_utf16_1[4]);
			lname[len + 5] = (char) le16_to_cpu(lfn.name_utf16_2[0]);
			lname[len + 6] = (char) le16_to_cpu(lfn.name_utf16_2[1]);
			lname[len + 7] = (char) le16_to_cpu(lfn.name_utf16_2[2]);
			lname[len + 8] = (char) le16_to_cpu(lfn.name_utf16_2[3]);
			lname[len + 9] = (char) le16_to_cpu(lfn.name_utf16_2[4]);
			lname[len + 10] = (char) le16_to_cpu(lfn.name_utf16_2[5]);
			lname[len + 11] = (char) le16_to_cpu(lfn.name_utf16_3[0]);
			lname[len + 12] = (char) le16_to_cpu(lfn.name_utf16_3[1]);
			continue;
		}

		if(dent->file_attributes & FAT_DIRENT_VOLLABLE)
		{
			continue;
		}

		memcpy(check, dent->dos_file_name, 8);
		memcpy(&check[8], dent->dos_extension, 3);
		dcsum = fatfs_node_lfn_checksum(check);

		if(!strlen(lname))
		{
			lfn_off = off - sizeof(struct fat_dirent_t);
			lfn_len = 0;
			i = 8;
			while(i && (dent->dos_file_name[i - 1] == ' '))
			{
				dent->dos_file_name[i - 1] = '\0';
				i--;
			}
			i = 3;
			while(i && (dent->dos_extension[i - 1] == ' '))
			{
				dent->dos_extension[i - 1] = '\0';
				i--;
			}

			for(i = 0; i < 8 && dent->dos_file_name[i]; i++)
			{
				lname[i] = tolower(dent->dos_file_name[i]);
			}
			lname[8] = '\0';

			if(dent->dos_extension[0] != '\0')
			{
				len = strlen(lname);
				lname[len + 0] = '.';
				lname[len + 1] = tolower(dent->dos_extension[0]);
				lname[len + 2] = tolower(dent->dos_extension[1]);
				lname[len + 3] = tolower(dent->dos_extension[2]);
				lname[len + 4] = '\0';
			}
			lcsum = dcsum;
		}

		if(!strncmp(lname, name, VFS_MAX_NAME) && (lcsum == dcsum))
		{
			*dent_off = lfn_off;
			*dent_len = sizeof(struct fat_dirent_t) + lfn_len;

			return 0;
		}

		lfn_off = off;
		lfn_len = 0;
		memset(lname, 0, sizeof(lname));
	}

	return -1;
}

int fatfs_node_add_dirent(struct fatfs_node_t * dnode, const char * name, struct fat_dirent_t * ndent)
{
	bool_t found;
	u8_t dcsum, check[11];
	u32_t i, len, off, cnt, dent_cnt, dent_off;
	struct fat_dirent_t dent;
	struct fat_longname_t lfn;

	/* Determine count of long filename enteries required */
	len = strlen(name) + 1;
	dent_cnt = udiv32(len, 13);
	if((dent_cnt * 13) < len)
		dent_cnt++;

	/* Atleast one entry in existing FAT directory entry format */
	dent_cnt += 1;

	/* Determine offset for directory enteries */
	cnt = 0;
	found = FALSE;
	dent_off = 0x0;
	while(1)
	{
		len = fatfs_node_read(dnode, dent_off, sizeof(struct fat_dirent_t), (u8_t *) &dent);
		if(len != sizeof(struct fat_dirent_t))
		{
			cnt = 0;
			found = TRUE;
			break;
		}

		if(dent.dos_file_name[0] == 0x0)
		{
			cnt = 0;
			found = TRUE;
			break;
		}

		if((dent.dos_file_name[0] == 0xE5) || (dent.dos_file_name[0] == 0x2E))
		{
			cnt++;
			if(cnt == dent_cnt)
			{
				dent_off -= (cnt - 1) * sizeof(dent);
				found = TRUE;
				break;
			}
		}
		else
		{
			cnt = 0;
		}

		dent_off += sizeof(dent);
	}

	if(!found)
		return -1;

	/* Prepare final directory entry */
	i = (u32_t)ktime_to_ns(ktime_get());
	snprintf((char *)check, sizeof(check), "%08x", i);
	memcpy(&dent, ndent, sizeof(dent));
	dent.dos_file_name[0] = ' ';
	dent.dos_file_name[1] = '\0';
	dent.dos_file_name[2] = check[0]; /* Random byte */
	dent.dos_file_name[3] = check[1]; /* Random byte */
	dent.dos_file_name[4] = check[2]; /* Random byte */
	dent.dos_file_name[5] = check[3]; /* Random byte */
	dent.dos_file_name[6] = check[4]; /* Random byte */
	dent.dos_file_name[7] = check[5]; /* Random byte */
	dent.dos_extension[0] = '/';
	dent.dos_extension[1] = check[6]; /* Random byte */
	dent.dos_extension[2] = check[7]; /* Random byte */

	/* Compute checksum of final directory entry */
	memcpy(check, dent.dos_file_name, 8);
	memcpy(&check[8], dent.dos_extension, 3);
	dcsum = fatfs_node_lfn_checksum(check);

	/* Write long filename enteries */
	for(cnt = 0; cnt < (dent_cnt - 1); cnt++)
	{
		memset(&lfn, 0xFF, sizeof(lfn));
		lfn.seqno = FAT_LONGNAME_SEQNO((dent_cnt - 1) - cnt);
		if(cnt == 0)
			lfn.seqno |= FAT_LONGNAME_LASTSEQ_MASK;
		lfn.file_attributes = FAT_LONGNAME_ATTRIBUTE;
		lfn.type = 0;
		lfn.checksum = dcsum;
		lfn.first_cluster = 0;
		off = ((dent_cnt - 2) - cnt) * 13;
		len = strlen(name) + 1;
		for(i = 0; i < 13; i++)
		{
			if((off + i) == len)
				break;
			if(i < 5)
				lfn.name_utf16_1[i] = le16_to_cpu((u16_t )name[off + i]);
			else if(i < 11)
				lfn.name_utf16_2[i - 5] = le16_to_cpu((u16_t )name[off + i]);
			else
				lfn.name_utf16_3[i - 11] = le16_to_cpu((u16_t )name[off + i]);
		}

		off = dent_off + cnt * sizeof(lfn);
		len = fatfs_node_write(dnode, off, sizeof(lfn), (u8_t *) &lfn);
		if(len != sizeof(lfn))
			return -1;
	}

	/* Write final directory entry */
	off = dent_off + (dent_cnt - 1) * sizeof(dent);
	len = fatfs_node_write(dnode, off, sizeof(dent), (u8_t *) &dent);
	if(len != sizeof(dent))
		return -1;

	return 0;
}

int fatfs_node_del_dirent(struct fatfs_node_t * dnode, const char * name, u32_t dent_off, u32_t dent_len)
{
	u32_t off, len;
	struct fat_dirent_t dent;

	//FIXME
	memset(&dent, 0, sizeof(dent));
	dent.dos_file_name[0] = 0xE5;

	for(off = 0; off < dent_len; off += sizeof(dent))
	{
		if((dent_len - off) < sizeof(dent))
			break;

		len = fatfs_node_write(dnode, dent_off + off, sizeof(dent), (u8_t *) &dent);
		if(len != sizeof(dent))
			return -1;
	}
	return 0;
}
