#ifndef __FRAMEWORK_CORE_L_TEXT_H__
#define __FRAMEWORK_CORE_L_TEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define MT_TEXT	"__mt_text__"

struct ltext_t {
	struct font_context_t * f;
	struct surface_t * s;
	char * utf8;
	char * family;
	int size;
	struct color_t c;
	struct region_t e;
};

int luaopen_text(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_TEXT_H__ */
