#ifndef __KEYHANDLER_H__
#define __KEYHANDLER_H__

#include <configs.h>
#include <default.h>
#include <input/input.h>
#include <input/keyboard/keyboard.h>

typedef void (*handler_onkeyraw)(struct input_event * event);
typedef void (*handler_onkeyup)(enum key_code key);
typedef void (*handler_onkeydown)(enum key_code key);


x_bool install_listener_onkeyraw(handler_onkeyraw keyraw);
x_bool remove_listener_onkeyraw(handler_onkeyraw keyraw);
x_bool install_listener_onkeyup(handler_onkeyup keyup);
x_bool remove_listener_onkeyup(handler_onkeyup keyup);
x_bool install_listener_onkeydown(handler_onkeydown keydown);
x_bool remove_listener_onkeydown(handler_onkeydown keydown);

#endif /* __KEYHANDLER_H__ */
