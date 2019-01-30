#ifndef __FRAMEWORK_CORE_L_FONT_H__
#define __FRAMEWORK_CORE_L_FONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <cairoint.h>
#include <cairo-ft.h>
#include <framework/luahelper.h>

#define	MT_FONT		"__mt_font__"

struct lfont_t {
	FT_Library library;
	FT_Face fface;
	cairo_font_face_t * face;
	cairo_scaled_font_t * font;
};

int luaopen_font(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_FONT_H__ */
