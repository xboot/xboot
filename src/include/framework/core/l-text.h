#ifndef __FRAMEWORK_CORE_L_TEXT_H__
#define __FRAMEWORK_CORE_L_TEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <cairoint.h>
#include <cairo-ft.h>
#include <framework/luahelper.h>

#define MT_TEXT	"__mt_text__"

struct ltext_t {
	char * utf8;
	cairo_scaled_font_t * font;
	cairo_pattern_t * pattern;
	cairo_text_extents_t metric;
};

int luaopen_text(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_TEXT_H__ */
