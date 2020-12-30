#ifndef __SOUND_SOUND_H__
#define __SOUND_SOUND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <xfs/xfs.h>

/*
 * The sound is short audio, fixed to stereo, 48khz, 16bits signed format.
 */
struct sound_t
{
	struct list_head list;
	uint32_t * source;
	int sample;
	int postion;
	int loop;
	int lvol;
	int rvol;
	float gain;
	float pan;
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

static inline void sound_set_callback(struct sound_t * snd, void (*cb)(struct sound_t *))
{
	snd->cb = cb;
}

static inline int sound_get_loop(struct sound_t * snd)
{
	return snd->loop;
}

static inline void sound_set_loop(struct sound_t * snd, int loop)
{
	snd->loop = loop;
}

static inline float sound_get_gain(struct sound_t * snd)
{
	return snd->gain;
}

static inline void sound_set_gain(struct sound_t * snd, float gain)
{
	snd->gain = gain;
	snd->lvol = clamp((int)(snd->gain * (snd->pan <= 0.0f ? 1.0f : 1.0f - snd->pan) * 4096), 0, 4096);
	snd->rvol = clamp((int)(snd->gain * (snd->pan >= 0.0f ? 1.0f : 1.0f + snd->pan) * 4096), 0, 4096);
}

static inline float sound_get_pan(struct sound_t * snd)
{
	return snd->pan;
}

static inline void sound_set_pan(struct sound_t * snd, float pan)
{
	snd->pan = clamp(pan, -1.0f, 1.0f);
	snd->lvol = clamp((int)(snd->gain * (snd->pan <= 0.0f ? 1.0f : 1.0f - snd->pan) * 4096), 0, 4096);
	snd->rvol = clamp((int)(snd->gain * (snd->pan >= 0.0f ? 1.0f : 1.0f + snd->pan) * 4096), 0, 4096);
}

struct sound_t * sound_alloc(int sample);
struct sound_t * sound_alloc_from_xfs(struct xfs_context_t * ctx, const char * filename);
struct sound_t * sound_alloc_tone(int frequency, int millisecond);
void sound_free(struct sound_t * snd);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_SOUND_H__ */
