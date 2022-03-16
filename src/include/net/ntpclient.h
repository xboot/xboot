#ifndef __NTPCLIENT_H__
#define __NTPCLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <net/net.h>

int ntpclient_sync(struct net_t * net, const char * host);

#ifdef __cplusplus
}
#endif

#endif /* __NTPCLIENT_H__ */
