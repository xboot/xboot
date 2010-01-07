#ifndef __DIV64_H__
#define __DIV64_H__

#include <configs.h>
#include <default.h>

x_u64 div64(x_u64 num, x_u64 den);
x_u64 mod64(x_u64 num, x_u64 den);
x_u64 div64_64(x_u64 * num, x_u64 den);

#endif /* __DIV64_H__ */
