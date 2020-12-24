#ifndef __SOUND_SOUNDPOOL_H__
#define __SOUND_SOUNDPOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sound/sound.h>

bool_t soundpool_add(struct sound_t * snd);
bool_t soundpool_remove(struct sound_t * snd);
bool_t soundpool_clear(void (*cb)(struct sound_t *));

int soundpool_get_volume(void);
void soundpool_set_volume(int vol);

void soundpool_playback(struct audio_t * audio);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_SOUNDPOOL_H__ */
