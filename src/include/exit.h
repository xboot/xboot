#ifndef __EXIT_H__
#define __EXIT_H__

enum {
	EXIT_SUCCESS	= 0,
	EXIT_FAILURE	= 1,
};

void abort(void);
void exit(int status);

#endif /* __EXIT_H__ */
