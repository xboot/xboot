#ifndef __PARTITION_H__
#define __PARTITION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct partition_map_t
{
	struct kobj_t * kobj;
	struct list_head list;
	char * name;
	bool_t (*map)(struct block_t * pblk);
};

bool_t register_partition_map(struct partition_map_t * map);
bool_t unregister_partition_map(struct partition_map_t * map);
void partition_map(struct block_t * pblk);

#ifdef __cplusplus
}
#endif

#endif /* __PARTITION_H__ */
