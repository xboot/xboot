#ifndef __BCD_H__
#define __BCD_H__

#include <configs.h>
#include <default.h>

x_u32 bcd2bin(x_u8 val);
x_u8 bin2bcd(x_u32 val);

#endif /* __BCD_H__ */
