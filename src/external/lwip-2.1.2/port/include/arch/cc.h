#ifndef __LWIP_CC_H__
#define __LWIP_CC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

#define LWIP_TIMEVAL_PRIVATE	0
#define LWIP_PROVIDE_ERRNO		1
#define LWIP_NO_UNISTD_H		1
#define SSIZE_MAX				1
#define LWIP_RAND()				((u32_t)rand())
#define LWIP_ERR_T				int

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_CC_H__ */
