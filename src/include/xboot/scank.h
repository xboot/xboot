#ifndef __SCANK_H__
#define __SCANK_H__

#include <xboot.h>
#include <stdarg.h>


bool_t getcode(u32_t * code);
bool_t getcode_with_timeout(u32_t * code, u32_t timeout);
s32_t scank(const char * fmt, ...);

#endif /* __SCANK_H__ */
