#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <spinlock.h>

struct channel_t {
	unsigned char * buffer;
	unsigned int size;
	unsigned int in;
	unsigned int out;
	struct list_head swait;
	struct list_head rwait;
	spinlock_t lock;
};

struct channel_t * channel_alloc(unsigned int size);
void channel_free(struct channel_t * c);
void channel_send(struct channel_t * c, unsigned char * buf, unsigned int len);
void channel_recv(struct channel_t * c, unsigned char * buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __CHANNEL_H__ */
