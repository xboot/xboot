#ifndef __SOUND_H__
#define __SOUND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum pcm_format_t {
	PCM_FORMAT_BIT8		= 8,
	PCM_FORMAT_BIT16	= 16,
	PCM_FORMAT_BIT24	= 24,
	PCM_FORMAT_BIT32	= 32,
};

enum pcm_rate_t {
	PCM_RATE_8000		= 8000,
	PCM_RATE_11025		= 11025,
	PCM_RATE_16000		= 16000,
	PCM_RATE_22050		= 22050,
	PCM_RATE_32000		= 32000,
	PCM_RATE_44100		= 44100,
	PCM_RATE_48000		= 48000,
	PCM_RATE_64000		= 64000,
	PCM_RATE_88200		= 88200,
	PCM_RATE_96000		= 96000,
	PCM_RATE_176400		= 176400,
	PCM_RATE_192000		= 192000,
};

enum sound_status_t {
	SOUND_STATUS_STOP	= 0,
	SOUND_STATUS_PLAY	= 1,
	SOUND_STATUS_PAUSE	= 2,
};

struct sound_info_t {
	char * title;
	char * singer;
	enum pcm_rate_t rate;
	enum pcm_format_t fmt;
	int channel;
	int length;
};

struct sound_t
{
	/* Sound information */
	struct sound_info_t info;

	/* Sound status */
	enum sound_status_t status;

	/* Sound volume */
	int volume;

	/* Sound position */
	int position;

	/* Sound seek */
	int (*seek)(struct sound_t * snd, int offset);

	/* Sound read */
	int (*read)(struct sound_t * snd, void * buf, int count);

	/* Sound close */
	void (*close)(struct sound_t * snd);

	/* Private data */
	void * priv;
};

struct sound_t * sound_alloc(const char * filename);
void sound_free(struct sound_t * snd);
struct sound_info_t * sound_get_info(struct sound_t * snd);
enum sound_status_t sound_get_status(struct sound_t * snd);
void sound_set_volume(struct sound_t * snd, int percent);
int sound_get_volume(struct sound_t * snd);
void sound_set_position(struct sound_t * snd, int position);
int sound_get_position(struct sound_t * snd);
void sound_play(struct sound_t * snd);
void sound_pause(struct sound_t * snd);
void sound_stop(struct sound_t * snd);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_H__ */
