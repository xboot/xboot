#ifndef __DT_H__
#define __DT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stddef.h>
#include <string.h>

u8_t dt_read_u8(void * dt, const char * name, u8_t def);
u16_t dt_read_u16(void * dt, const char * name, u16_t def);
u32_t dt_read_u32(void * dt, const char * name, u32_t def);
u64_t dt_read_u64(void * dt, const char * name, u64_t def);
bool_t dt_read_boolean(void * dt, const char * name, bool_t def);
double dt_read_double(void * dt, const char * name, double def);
const char * dt_read_string(void * dt, const char * name, const char * def);

#ifdef __cplusplus
}
#endif

#endif /* __DT_H__ */
