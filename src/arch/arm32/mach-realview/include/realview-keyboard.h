#ifndef __REALVIEW_KEYBOARD_H__
#define __REALVIEW_KEYBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/input.h>
#include <input/keyboard.h>
#include <realview/reg-keyboard.h>

struct realview_keyboard_data_t
{
	physical_addr_t regbase;
};

#ifdef __cplusplus
}
#endif

#endif /* __REALVIEW_KEYBOARD_H__ */
