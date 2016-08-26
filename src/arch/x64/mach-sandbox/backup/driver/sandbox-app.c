/*
 * drivers/sandbox-app.c
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

#include <block/block.h>
#include <sandbox.h>

struct application_t
{
	char * name;
	void * start;
	void * end;
};

static u64_t application_read(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct application_t * app = (struct application_t *)(blk->priv);
	u64_t offset = block_offset(blk, blkno);
	u64_t length = block_available_length(blk, blkno, blkcnt);
	u64_t count = block_available_count(blk, blkno, blkcnt);

	memcpy((void *)buf, (const void *)((u8_t *)(app->start) + offset), length);
	return count;
}

static u64_t application_write(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	return 0;
}

static void application_sync(struct block_t * blk)
{
}

static void application_suspend(struct block_t * blk)
{
}

static void application_resume(struct block_t * blk)
{
}

static bool_t register_application(const char * name, void * start, void * end)
{
	struct block_t * blk;
	struct application_t * app;
	u64_t size;

	if(!name)
		return FALSE;

	size = (u64_t)(end - start);
	size = (size + SZ_512) / SZ_512;
	if(size <= 0)
		return FALSE;

	blk = malloc(sizeof(struct block_t));
	if(!blk)
		return FALSE;

	app = malloc(sizeof(struct application_t));
	if(!app)
	{
		free(blk);
		return FALSE;
	}

	app->name = strdup(name);
	app->start = start;
	app->end = end;

	blk->name	= app->name;
	blk->blksz	= SZ_512;
	blk->blkcnt	= size;
	blk->read 	= application_read;
	blk->write	= application_write;
	blk->sync	= application_sync;
	blk->suspend = application_suspend;
	blk->resume = application_resume;
	blk->priv	= app;

	if(!register_block(blk))
	{
		free(app->name);
		free(app);
		free(blk);
		return FALSE;
	}

	return TRUE;
}

static bool_t unregister_application(const char * name)
{
	struct block_t * blk;
	struct application_t * app;

	if(!name)
		return FALSE;

	blk = search_block(name);
	if(!blk)
		return FALSE;

	app = (struct application_t *)(blk->priv);
	if(!unregister_block(blk))
		return FALSE;

	free(app->name);
	free(app);
	free(blk);
	return TRUE;
}

static __init void application_init(void)
{
	struct sandbox_t * sandbox = sandbox_get();

	if(sandbox->application.size > 0)
		register_application("application", (void *)(sandbox->application.buffer), (void *)(sandbox->application.buffer + sandbox->application.size));
}

static __exit void application_exit(void)
{
	struct sandbox_t * sandbox = sandbox_get();

	if(sandbox->application.size > 0)
		unregister_application("application");
}

device_initcall(application_init);
device_exitcall(application_exit);
