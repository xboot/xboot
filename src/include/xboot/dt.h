#ifndef __DT_H__
#define __DT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stddef.h>
#include <string.h>
#include <json.h>

struct dtnode_t {
	const char * name;
	physical_addr_t addr;
	json_value * value;
};

const char * dt_read_name(struct dtnode_t * n);
int dt_read_id(struct dtnode_t * n);
physical_addr_t dt_read_address(struct dtnode_t * n);
struct dtnode_t * dt_read_object(struct dtnode_t * n, const char * name);
int dt_read_bool(struct dtnode_t * n, const char * name, int def);
int dt_read_int(struct dtnode_t * n, const char * name, int def);
double dt_read_double(struct dtnode_t * n, const char * name, double def);
char * dt_read_string(struct dtnode_t * n, const char * name, char * def);
u8_t dt_read_u8(struct dtnode_t * n, const char * name, u8_t def);
u16_t dt_read_u16(struct dtnode_t * n, const char * name, u16_t def);
u32_t dt_read_u32(struct dtnode_t * n, const char * name, u32_t def);
u64_t dt_read_u64(struct dtnode_t * n, const char * name, u64_t def);
void dt_for_each(const char * path);

#ifdef __cplusplus
}
#endif

#endif /* __DT_H__ */
