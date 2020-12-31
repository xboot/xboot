#ifndef __AUDIO_H__
#define __AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum audio_format_t {
	AUDIO_FORMAT_S8		= 8,
	AUDIO_FORMAT_S16	= 16,
	AUDIO_FORMAT_S24	= 24,
	AUDIO_FORMAT_S32	= 32,
};

enum audio_rate_t {
	AUDIO_RATE_8000		= 8000,
	AUDIO_RATE_11025	= 11025,
	AUDIO_RATE_16000	= 16000,
	AUDIO_RATE_22050	= 22050,
	AUDIO_RATE_32000	= 32000,
	AUDIO_RATE_44100	= 44100,
	AUDIO_RATE_48000	= 48000,
	AUDIO_RATE_64000	= 64000,
	AUDIO_RATE_88200	= 88200,
	AUDIO_RATE_96000	= 96000,
	AUDIO_RATE_176400	= 176400,
	AUDIO_RATE_192000	= 192000,
};

typedef int (*audio_callback_t)(void * data, void * buf, int count);

struct audio_t
{
	/* The audio name */
	char * name;

	/* The sound pool */
	struct {
		struct list_head list;
		spinlock_t lock;
	} soundpool;

	/* Audio playback start */
	void (*playback_start)(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data);

	/* Audio playback stop */
	void (*playback_stop)(struct audio_t * audio);

	/* Audio capture start */
	void (*capture_start)(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data);

	/* Audio capture stop */
	void (*capture_stop)(struct audio_t * audio);

	/* Ioctl interface */
	int (*ioctl)(struct audio_t * audio, const char * cmd, void * arg);

	/* Private data */
	void * priv;
};

struct audio_t * search_audio(const char * name);
struct audio_t * search_first_audio(void);
struct device_t * register_audio(struct audio_t * audio, struct driver_t * drv);
void unregister_audio(struct audio_t * audio);

void audio_playback_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data);
void audio_playback_stop(struct audio_t * audio);
void audio_capture_start(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data);
void audio_capture_stop(struct audio_t * audio);
int audio_ioctl(struct audio_t * audio, const char * cmd, void * arg);

void audio_playback(struct audio_t * audio, struct sound_t * snd);

static inline int audio_get_playback_volume(struct audio_t * audio)
{
	int vol = 0;

	audio_ioctl(audio, "audio-get-playback-volume", &vol);
	return vol;
}

static inline void audio_set_playback_volume(struct audio_t * audio, int vol)
{
	audio_ioctl(audio, "audio-set-playback-volume", &vol);
}

static inline int audio_get_capture_volume(struct audio_t * audio)
{
	int vol = 0;

	audio_ioctl(audio, "audio-get-capture-volume", &vol);
	return vol;
}

static inline void audio_set_capture_volume(struct audio_t * audio, int vol)
{
	audio_ioctl(audio, "audio-set-capture-volume", &vol);
}

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_H__ */
