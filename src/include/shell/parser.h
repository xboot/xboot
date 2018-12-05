#ifndef __PARSER_H__
#define __PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

bool_t parser(const char * cmdline, int * argc, char *** argv, char ** pos);

#ifdef __cplusplus
}
#endif

#endif /* __PARSER_H__ */
