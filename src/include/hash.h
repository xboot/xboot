#ifndef __HASH_H__
#define __HASH_H__

#include <configs.h>
#include <default.h>

x_u32 string_hash(const char * s);
inline x_u32 long_hash(x_u32 val, x_u32 bits);
inline x_u32 ptr_hash(void *ptr, x_u32 bits);


#endif /* __HASH_H__ */
