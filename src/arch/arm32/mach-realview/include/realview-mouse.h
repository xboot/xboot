#ifndef __REALVIEW_MOUSE_H__
#define __REALVIEW_MOUSE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/input.h>
#include <realview-irq.h>
#include <realview/reg-mouse.h>

struct realview_mouse_data_t
{
	physical_addr_t regbase;
	s32_t width;
	s32_t height;
};

#ifdef __cplusplus
}
#endif

#endif /* __REALVIEW_MOUSE_H__ */
