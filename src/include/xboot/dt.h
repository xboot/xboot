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
int dt_read_bool(void * dt, const char * name, int def);
int dt_read_int(void * dt, const char * name, int def);
double dt_read_double(void * dt, const char * name, double def);
const char * dt_read_string(void * dt, const char * name, const char * def);

#ifdef __cplusplus
}
#endif

#endif /* __DT_H__ */
