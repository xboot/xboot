#ifndef __READLINE_H__
#define __READLINE_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>

/**
 * the enum type of readline mode
 */
enum readline_mode {
	RL_NORMAL,
	RL_ESC,
	RL_CSI
};


s8_t * readline(const s8_t *prompt);

#endif /* __READLINE_H__ */
