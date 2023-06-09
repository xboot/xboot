#ifndef __PARTITION_H__
#define __PARTITION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct partition_parser_t
{
	struct kobj_t * kobj;
	struct list_head list;
	char * name;
	bool_t (*parse)(struct block_t * pblk);
};

bool_t register_partition_parser(struct partition_parser_t * parser);
bool_t unregister_partition_parser(struct partition_parser_t * parser);
void partition_parse(struct block_t * pblk);

#ifdef __cplusplus
}
#endif

#endif /* __PARTITION_H__ */
