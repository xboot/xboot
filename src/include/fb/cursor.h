#ifndef __FB_CURSOR_H__
#define __FB_CURSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

s32_t cursor_xpos_with_offset(s32_t relx);
s32_t cursor_ypos_with_offset(s32_t rely);
s32_t get_cursor_xpos(void);
s32_t get_cursor_ypos(void);

#ifdef __cplusplus
}
#endif

#endif /* __FB_CURSOR_H__ */
