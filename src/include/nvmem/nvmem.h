#ifndef __NVMEM_H__
#define __NVMEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct nvmem_t
{
	char * name;
	int (*read)(struct nvmem_t * m, void * buf, int offset, int count);
	int (*write)(struct nvmem_t * m, void * buf, int offset, int count);
	int (*capacity)(struct nvmem_t * m);
	void * priv;
};

struct nvmem_t * search_nvmem(const char * name);
struct nvmem_t * search_first_nvmem(void);
bool_t register_nvmem(struct device_t ** device, struct nvmem_t * m);
bool_t unregister_nvmem(struct nvmem_t * m);

int nvmem_read(struct nvmem_t * m, void * buf, int offset, int count);
int nvmem_write(struct nvmem_t * m, void * buf, int offset, int count);
int nvmem_capacity(struct nvmem_t * m);

#ifdef __cplusplus
}
#endif

#endif /* __NVMEM_H__ */
