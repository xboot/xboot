#ifndef __READLINE_H__
#define __READLINE_H__

/*
 * The enum type of readline mode
 */
enum readline_mode {
	RL_NORMAL,
	RL_ESC,
	RL_CSI
};

char * readline(const char * prompt);

#endif /* __READLINE_H__ */
