#ifndef __COLOR_H__
#define __COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct color_t {
	u8_t r;
	u8_t g;
	u8_t b;
	u8_t a;
};

struct color_t * get_color_by_name(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __COLOR_H__ */
