#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <dma/dmapool.h>

enum {
	DMA_TYPE_MEMTOMEM		= 0,
	DMA_TYPE_MEMTODEV		= 1,
	DMA_TYPE_DEVTOMEM		= 2,
	DMA_TYPE_DEVTODEV		= 3,
};

enum {
	DMA_INCREASE			= 0,
	DMA_CONSTANT			= 1,
};

enum {
	DMA_WIDTH_8BIT			= 0,
	DMA_WIDTH_16BIT			= 1,
	DMA_WIDTH_32BIT			= 2,
	DMA_WIDTH_64BIT			= 3,
};

enum {
	DMA_BURST_SIZE_1		= 0,
	DMA_BURST_SIZE_4		= 1,
	DMA_BURST_SIZE_8		= 2,
	DMA_BURST_SIZE_16		= 3,
	DMA_BURST_SIZE_32		= 4,
	DMA_BURST_SIZE_64		= 5,
	DMA_BURST_SIZE_128		= 6,
	DMA_BURST_SIZE_256		= 7,
	DMA_BURST_SIZE_512		= 8,
	DMA_BURST_SIZE_1024		= 9,
	DMA_BURST_SIZE_2048		= 10,
	DMA_BURST_SIZE_4096		= 11,
	DMA_BURST_SIZE_8192		= 12,
	DMA_BURST_SIZE_16384	= 13,
	DMA_BURST_SIZE_32768	= 14,
	DMA_BURST_SIZE_65536	= 15,
};

#define DMA_S_TYPE(x)		(((x) & 0x3) << 0)
#define DMA_S_SRC_INC(x)	(((x) & 0x1) << 2)
#define DMA_S_DST_INC(x)	(((x) & 0x1) << 3)
#define DMA_S_SRC_WIDTH(x)	(((x) & 0x3) << 4)
#define DMA_S_DST_WIDTH(x)	(((x) & 0x3) << 6)
#define DMA_S_SRC_BURST(x)	(((x) & 0xf) << 8)
#define DMA_S_DST_BURST(x)	(((x) & 0xf) << 12)
#define DMA_S_SRC_PORT(x)	(((x) & 0xff) << 16)
#define DMA_S_DST_PORT(x)	(((x) & 0xff) << 24)

#define DMA_G_TYPE(x)		(((x) >> 0) & 0x3)
#define DMA_G_SRC_INC(x)	(((x) >> 2) & 0x1)
#define DMA_G_DST_INC(x)	(((x) >> 3) & 0x1)
#define DMA_G_SRC_WIDTH(x)	(((x) >> 4) & 0x3)
#define DMA_G_DST_WIDTH(x)	(((x) >> 6) & 0x3)
#define DMA_G_SRC_BURST(x)	(((x) >> 8) & 0xf)
#define DMA_G_DST_BURST(x)	(((x) >> 12) & 0xf)
#define DMA_G_SRC_PORT(x)	(((x) >> 16) & 0xff)
#define DMA_G_DST_PORT(x)	(((x) >> 24) & 0xff)

struct dma_channel_t
{
	spinlock_t lock;
	void * src;
	void * dst;
	int size;
	int flag;
	int len;
	void * data;
	void (*complete)(void * data);
};

struct dmachip_t
{
	char * name;
	int base;
	int ndma;

	struct dma_channel_t * channel;
	void (*start)(struct dmachip_t * chip, int offset);
	void (*stop)(struct dmachip_t * chip, int offset);
	int (*busying)(struct dmachip_t * chip, int offset);
	void * priv;
};

struct dmachip_t * search_dmachip(int dma);
struct device_t * register_dmachip(struct dmachip_t * chip, struct driver_t * drv);
void unregister_dmachip(struct dmachip_t * chip);
bool_t dma_is_valid(int dma);
void dma_start(int dma, void * src, void * dst, int size, int flag, void (*complete)(void *), void * data);
void dma_stop(int dma);
void dma_wait(int dma);

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */
