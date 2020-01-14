#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <spinlock.h>
#include <dma/dmapool.h>

struct dma_channel_t {
	spinlock_t lock;
	void * src;
	void * dst;
	int size;
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
	void (*wait)(struct dmachip_t * chip, int offset);
	void * priv;
};

struct dmachip_t * search_dmachip(int dma);
struct device_t * register_dmachip(struct dmachip_t * chip, struct driver_t * drv);
void unregister_dmachip(struct dmachip_t * chip);
bool_t dma_is_valid(int dma);
void dma_start(int dma, void * src, void * dst, int size, void (*complete)(void *), void * data);
void dma_stop(int dma);
void dma_wait(int dma);

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */
