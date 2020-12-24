#ifndef __SOUND_SOUND_H__
#define __SOUND_SOUND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <xfs/xfs.h>
#include <audio/audio.h>

/*
 * The sound is short audio, fixed to stereo, 48khz, 16bits signed format.
 */
struct sound_t
{
	struct list_head list;
	uint32_t * source;
	int sample;
	int postion;
	int lvol;
	int rvol;
	int loop;
	void (*cb)(struct sound_t *);
};

static inline uint32_t * sound_get_source(struct sound_t * snd)
{
	return snd->source;
}

static inline int sound_get_sample(struct sound_t * snd)
{
	return snd->sample;
}

static inline int sound_get_postion(struct sound_t * snd)
{
	return snd->postion;
}

static inline int sound_get_left_volume(struct sound_t * snd)
{
	return snd->lvol;
}

static inline int sound_get_right_volume(struct sound_t * snd)
{
	return snd->rvol;
}

static inline int sound_get_loop(struct sound_t * snd)
{
	return snd->loop;
}

static inline uint32_t * sound_get_remaining_source(struct sound_t * snd)
{
	if(snd->sample > snd->postion)
		return &snd->source[snd->postion];
	return NULL;
}

static inline int sound_get_remaining_sample(struct sound_t * snd)
{
	return snd->sample - snd->postion;
}

static inline void sound_set_left_volume(struct sound_t * snd, int vol)
{
	snd->lvol = clamp(vol, 0, 4096);
}

static inline void sound_set_right_volume(struct sound_t * snd, int vol)
{
	snd->rvol = clamp(vol, 0, 4096);
}

static inline void sound_set_loop(struct sound_t * snd, int loop)
{
	snd->loop = loop;
}

static inline void sound_set_callback(struct sound_t * snd, void (*cb)(struct sound_t *))
{
	snd->cb = cb;
}

struct sound_t * sound_alloc(int sample);
struct sound_t * sound_alloc_from_xfs(struct xfs_context_t * ctx, const char * filename);
struct sound_t * sound_alloc_tone(int frequency, int millisecond);
void sound_free(struct sound_t * snd);

void sound_play_by_audio(struct sound_t * snd, struct audio_t * audio);
void sound_play(struct sound_t * snd);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_SOUND_H__ */
