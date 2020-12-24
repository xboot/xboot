#ifndef __SOUND_SOUND_H__
#define __SOUND_SOUND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <xfs/xfs.h>

enum sound_state_t {
	SOUND_STATE_PLAYING	= 0,
	SOUND_STATE_PAUSED	= 1,
	SOUND_STATE_STOPPED	= 2,
};

/*
 * The sound is short audio, fixed to stereo, 48khz, 16bits signed format.
 */
struct sound_t
{
	struct list_head list;
	void * source;
	size_t length;
	size_t postion;
	enum sound_state_t state;
	int lgain, rgain;
	int loop;
};

static inline void * sound_get_source(struct sound_t * snd)
{
	return snd->source;
}

static inline size_t sound_get_length(struct sound_t * snd)
{
	return snd->length;
}

static inline size_t sound_get_sample(struct sound_t * snd)
{
	return (snd->length >> 2);
}

static inline size_t sound_get_postion(struct sound_t * snd)
{
	return snd->postion;
}

static inline void * sound_get_remaining_source(struct sound_t * snd)
{
	if(snd->length - snd->postion > 1)
		return &((uint8_t *)snd->source)[snd->postion];
	return NULL;
}

static inline size_t sound_get_remaining_length(struct sound_t * snd)
{
	return snd->length - snd->postion - 1;
}

static inline size_t sound_get_remaining_sample(struct sound_t * snd)
{
	return (snd->length - snd->postion - 1) >> 2;
}

struct sound_t * sound_alloc(size_t length);
struct sound_t * sound_alloc_from_xfs(struct xfs_context_t * ctx, const char * filename);
struct sound_t * sound_alloc_tone(int frequency, int millisecond);
void sound_free(struct sound_t * snd);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_SOUND_H__ */
