#ifndef __FRAMEWORK_CORE_L_TEXT_H__
#define __FRAMEWORK_CORE_L_TEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define MT_TEXT	"__mt_text__"

struct ltext_t {
	char * utf8;
	char * family;
	struct color_t c;
	struct text_t txt;
};

int luaopen_text(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_TEXT_H__ */
