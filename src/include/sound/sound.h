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
 * The sound is short audio, fixed to stereo, 48khz, 16bits format.
 */
struct sound_t
{
	struct list_head list;
	void * datas;
	size_t length;
	size_t postion;
	enum sound_state_t state;
	int lgain, rgain;
	int loop;
	void * priv;
};

struct sound_t * sound_alloc(size_t length, void * priv);
void sound_free(struct sound_t * s);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_SOUND_H__ */
