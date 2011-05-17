#ifndef __READLINE_H__
#define __READLINE_H__

#include <xboot.h>
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

char * readline(const char * prompt);

#endif /* __READLINE_H__ */
