#ifndef __ENVIRON_H__
#define __ENVIRON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

/*
 * environment variable
 */
struct environ_t {
	char * content;
	struct environ_t * prev;
	struct environ_t * next;
};

char * getenv(const char * name);
int putenv(const char * str);
int setenv(const char * name, const char * val, int overwrite);
int unsetenv(const char * name);
int clearenv(void);

bool_t loadenv(char * file);
bool_t saveenv(char * file);

#ifdef __cplusplus
}
#endif

#endif /* __ENVIRON_H__ */
