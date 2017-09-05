#ifndef __SANDBOX_H__
#define __SANDBOX_H__

/*
 * Sandbox interface
 */
struct sandbox_t {
	struct {
		char * buffer;
		int size;
	} application;
};
void sandbox_init(int argc, char * argv[]);
void sandbox_exit(void);

#endif /* __SANDBOX_H__ */
