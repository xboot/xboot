#ifndef __PATH_H__
#define __PATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot/module.h>
#include <types.h>
#include <string.h>

char * basename(char * path);
char * dirname(char * path);

#ifdef __cplusplus
}
#endif

#endif /* __PATH_H__ */
