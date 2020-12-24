#ifndef __SOUND_SOUNDPOOL_H__
#define __SOUND_SOUNDPOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sound/sound.h>

extern struct list_head __soundpool_list;

bool_t soundpool_add(struct sound_t * snd);
bool_t soundpool_remove(struct sound_t * snd);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_SOUNDPOOL_H__ */
