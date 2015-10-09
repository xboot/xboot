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

struct sound_t
{
	/* Sound list */
	struct list_head entry;

	/* Sound sample rate */
	enum pcm_rate_t rate;

	/* Sound format */
	enum pcm_format_t fmt;

	/* Sound channel */
	int channel;

	/* Sound position */
	int position;

	/* Sound length */
	int length;

	/* Sound pause */
	int pause;

	/* Sound loop */
	int loop;

	/* Sound volume */
	int volume;

	/* Sound seek */
	int (*seek)(struct sound_t * sound, int offset);

	/* Sound read */
	int (*read)(struct sound_t * sound, void * buf, int count);

	/* Sound close */
	void (*close)(struct sound_t * sound);

	/* Private data */
	void * priv;
};

struct sound_loader_t
{
	const char * ext;
	bool_t (*load)(struct sound_t * sound, const char * filename);
};

struct sound_t * sound_alloc(const char * filename);
void sound_set_pause(struct sound_t * sound, int pause);
int sound_get_pause(struct sound_t * sound);
void sound_set_loop(struct sound_t * sound, int loop);
int sound_get_loop(struct sound_t * sound);
void sound_set_volume(struct sound_t * sound, int percent);
int sound_get_volume(struct sound_t * sound);
void sound_set_position(struct sound_t * sound, int position);
int sound_get_position(struct sound_t * sound);
int sound_rate(struct sound_t * sound);
int sound_format(struct sound_t * sound);
int sound_channel(struct sound_t * sound);
int sound_length(struct sound_t * sound);
int sound_read(struct sound_t * sound, void * buf, int count);
void sound_close(struct sound_t * sound);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_H__ */
