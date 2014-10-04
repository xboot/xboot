#ifndef __MODE_H__
#define __MODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

enum mode_t {
	MODE_NORMAL			= 0,
	MODE_SHELL			= 1,
};

enum mode_t xboot_get_mode(void);
bool_t xboot_set_mode(enum mode_t m);

void run_normal_mode(void);
void run_shell_mode(void);

#ifdef __cplusplus
}
#endif

#endif /* __MODE_H__ */
