#ifndef __DIV64_H__
#define __DIV64_H__

#include <types.h>

u64_t div64(u64_t num, u64_t den);
u64_t mod64(u64_t num, u64_t den);
u64_t div64_64(u64_t * num, u64_t den);

#endif /* __DIV64_H__ */
