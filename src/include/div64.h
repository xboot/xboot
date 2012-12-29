#ifndef __DIV64_H__
#define __DIV64_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

u64_t div64(u64_t num, u64_t den);
u64_t mod64(u64_t num, u64_t den);
u64_t div64_64(u64_t * num, u64_t den);

#ifdef __cplusplus
}
#endif

#endif /* __DIV64_H__ */
