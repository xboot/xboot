/*
 * kernel/fs/vfs/vfs_bio.c
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
#include <xboot/list.h>
#include <xboot/blkdev.h>
#include <xboot/device.h>
#include <fs/fs.h>
#include <fs/vfs/vfs.h>

/*
 * the struct bio, for io cache.
 */
struct bio
{
	/* block device pointer */
	struct blkdev * dev;

	/* block's no */
	x_s32 blkno;

	/* block's size */
	x_s32 size;

	/* mark flags */
	x_s32 flags;

	/* pointer to buffer */
	x_u8 * buf;
};

/*
 * the list of bio
 */
struct bio_list
{
	struct bio * bio;
	struct list_head entry;
};

/* the list of bio */
static struct bio_list __bio_list = {
	.entry = {
		.next	= &(__bio_list.entry),
		.prev	= &(__bio_list.entry),
	},
};
struct bio_list * bio_list = &__bio_list;

