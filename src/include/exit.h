#ifndef __EXIT_H__
#define __EXIT_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	EXIT_SUCCESS	= 0,
	EXIT_FAILURE	= 1,
};

void __attribute__((__noreturn__)) abort(void);
void __attribute__((__noreturn__)) exit(int code);

#ifdef __cplusplus
}
#endif

#endif /* __EXIT_H__ */
