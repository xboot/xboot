#ifndef __NVMEM_H__
#define __NVMEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <nvmem/kvdb.h>

struct nvmem_t
{
	char * name;
	struct kvdb_t * db;
	int (*capacity)(struct nvmem_t * m);
	int (*read)(struct nvmem_t * m, void * buf, int offset, int count);
	int (*write)(struct nvmem_t * m, void * buf, int offset, int count);
	void * priv;
};

struct nvmem_t * search_nvmem(const char * name);
struct nvmem_t * search_first_nvmem(void);
bool_t register_nvmem(struct device_t ** device, struct nvmem_t * m);
bool_t unregister_nvmem(struct nvmem_t * m);

int nvmem_capacity(struct nvmem_t * m);
int nvmem_read(struct nvmem_t * m, void * buf, int offset, int count);
int nvmem_write(struct nvmem_t * m, void * buf, int offset, int count);
void nvmem_set(struct nvmem_t * m, const char * key, const char * value);
char * nvmem_get(struct nvmem_t * m, const char * key, const char * def);
void nvmem_clear(struct nvmem_t * m);
void nvmem_sync(struct nvmem_t * m);

#ifdef __cplusplus
}
#endif

#endif /* __NVMEM_H__ */
