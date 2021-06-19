#ifndef __NVMEM_H__
#define __NVMEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct nvmem_t
{
	char * name;
	struct {
		struct timer_t timer;
		struct hmap_t * map;
		spinlock_t lock;
		int dirty;
	} kvdb;
	int (*capacity)(struct nvmem_t * m);
	int (*read)(struct nvmem_t * m, void * buf, int offset, int count);
	int (*write)(struct nvmem_t * m, void * buf, int offset, int count);
	void * priv;
};

struct nvmem_t * search_nvmem(const char * name);
struct nvmem_t * search_first_nvmem(void);
struct device_t * register_nvmem(struct nvmem_t * m, struct driver_t * drv);
void unregister_nvmem(struct nvmem_t * m);

int nvmem_capacity(struct nvmem_t * m);
int nvmem_read(struct nvmem_t * m, void * buf, int offset, int count);
int nvmem_write(struct nvmem_t * m, void * buf, int offset, int count);
void nvmem_set(struct nvmem_t * m, const char * key, const char * value);
const char * nvmem_get(struct nvmem_t * m, const char * key, const char * def);
void nvmem_clear(struct nvmem_t * m);

#ifdef __cplusplus
}
#endif

#endif /* __NVMEM_H__ */
