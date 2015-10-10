#ifndef __POOL_H__
#define __POOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <audio/sound.h>

struct sound_list_t
{
	struct sound_t * snd;
	struct list_head entry;
};

extern struct sound_list_t __sound_pool;

void sound_pool_add(struct sound_t * snd);
void sound_pool_del(struct sound_t * snd);
void sound_pool_clr(void);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_H__ */
