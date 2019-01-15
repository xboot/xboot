#ifndef __FRAMEWORK_CORE_L_PATTERN_H__
#define __FRAMEWORK_CORE_L_PATTERN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <cairoint.h>
#include <framework/luahelper.h>

#define MT_PATTERN	"__mt_pattern__"

struct lpattern_t {
	cairo_pattern_t * pattern;
};

int luaopen_pattern(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_PATTERN_H__ */
