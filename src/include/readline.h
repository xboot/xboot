#ifndef __READLINE_H__
#define __READLINE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The enum type of readline mode
 */
enum readline_mode {
	RL_NORMAL,
	RL_ESC,
	RL_CSI
};

char * readline(const char * prompt);

#ifdef __cplusplus
}
#endif

#endif /* __READLINE_H__ */
