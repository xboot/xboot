#ifndef __FRAMEWORK_CORE_L_ICON_H__
#define __FRAMEWORK_CORE_L_ICON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <luahelper.h>

#define MT_ICON	"__mt_icon__"

struct licon_t {
	char * family;
	struct color_t c;
	struct icon_t ico;
};

int luaopen_icon(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_ICON_H__ */
