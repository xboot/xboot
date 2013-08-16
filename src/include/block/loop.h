#ifndef __LOOP_H__
#define __LOOP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <block/block.h>

struct block_t * search_loop(const char * file);
bool_t register_loop(const char * file);
bool_t unregister_loop(const char * file);

#ifdef __cplusplus
}
#endif

#endif /* __LOOP_H__ */
