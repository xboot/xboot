#ifndef __CONSOLE_INPUT_H__
#define __CONSOLE_INPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/keyboard.h>
#include <input/input.h>

bool_t register_console_input(struct input_t * input);
bool_t unregister_console_input(struct input_t * input);

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_INPUT_H__ */
