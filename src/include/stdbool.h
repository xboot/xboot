#ifndef __STDBOOL_H__
#define __STDBOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

typedef bool_t	bool;

enum {
	FALSE		= 0,
	TRUE		= 1,
};

enum {
	false		= 0,
	true		= 1,
};

#ifdef __cplusplus
}
#endif

#endif /* __STDBOOL_H__ */
