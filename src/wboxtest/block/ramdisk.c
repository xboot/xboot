/*
 * wboxtest/block/ramdisk.c
 */

#include <wboxtest.h>

struct wbt_ramdisk_pdata_t
{
	struct block_t * blk;
	unsigned char * rambuf;
};

static void * ramdisk_setup(struct wboxtest_t * wbt)
{
	struct wbt_ramdisk_pdata_t * pdat;
	char json[256];
	int length;

	pdat = malloc(sizeof(struct wbt_ramdisk_pdata_t));
	if(!pdat)
		return NULL;

	pdat->rambuf = malloc(SZ_1M);
	if(!pdat->rambuf)
	{
		free(pdat);
		return NULL;
	}

	length = sprintf(json,
		"{\"blk-ramdisk@999\":{\"address\":%lld,\"size\":%lld}}",
		(unsigned long long)((virtual_addr_t)pdat->rambuf),
		(unsigned long long)((virtual_size_t)SZ_1M));
	probe_device(json, length, NULL);

	pdat->blk = search_block("blk-ramdisk.999");
	if(!pdat->blk)
	{
		free(pdat->rambuf);
		free(pdat);
		return NULL;
	}

	return pdat;
}

static void ramdisk_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_ramdisk_pdata_t * pdat = (struct wbt_ramdisk_pdata_t *)data;

	if(pdat)
	{
		unregister_block(pdat->blk);
		free(pdat->rambuf);
		free(pdat);
	}
}

static void ramdisk_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_ramdisk_pdata_t * pdat = (struct wbt_ramdisk_pdata_t *)data;
	u64_t blkno, blkcnt;
	char * buf1, * buf2;
	int len;

	if(pdat)
	{
		blkno = wboxtest_random_int(0, block_count(pdat->blk) - 1);
		blkcnt = wboxtest_random_int(1, min(block_count(pdat->blk), SZ_1M / block_size(pdat->blk)));
		blkcnt = block_available_count(pdat->blk, blkno, blkcnt);

		len = block_size(pdat->blk) * blkcnt;
		buf1 = malloc(len);
		buf2 = malloc(len);
		if(!buf1 || !buf2)
		{
			free(buf1);
			free(buf2);
		}

		wboxtest_random_buffer(buf1, len);
		pdat->blk->write(pdat->blk, (u8_t *)buf1, blkno, blkcnt);
		pdat->blk->sync(pdat->blk);
		pdat->blk->read(pdat->blk, (u8_t *)buf2, blkno, blkcnt);
		assert_memory_equal(buf1, buf2, len);

		wboxtest_random_buffer(buf1, len);
		block_write(pdat->blk, (u8_t *)buf1, block_size(pdat->blk) * blkno, block_size(pdat->blk) * blkcnt);
		block_sync(pdat->blk);
		block_read(pdat->blk, (u8_t *)buf2, block_size(pdat->blk) * blkno, block_size(pdat->blk) * blkcnt);
		assert_memory_equal(buf1, buf2, len);

		free(buf1);
		free(buf2);
	}
}

static struct wboxtest_t wbt_ramdisk = {
	.group	= "block",
	.name	= "ramdisk",
	.setup	= ramdisk_setup,
	.clean	= ramdisk_clean,
	.run	= ramdisk_run,
};

static __init void ramdisk_wbt_init(void)
{
	register_wboxtest(&wbt_ramdisk);
}

static __exit void ramdisk_wbt_exit(void)
{
	unregister_wboxtest(&wbt_ramdisk);
}

wboxtest_initcall(ramdisk_wbt_init);
wboxtest_exitcall(ramdisk_wbt_exit);
