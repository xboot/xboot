#ifndef __LOOP_H__
#define __LOOP_H__


#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/blkdev.h>

struct blkdev * search_loop(const char * file);
bool_t register_loop(const char * file);
bool_t unregister_loop(const char * file);

#endif /* __LOOP_H__ */
