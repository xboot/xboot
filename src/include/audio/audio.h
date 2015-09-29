#ifndef __AUDIO_H__
#define __AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

typedef int (*audio_callback_t)(void * data, void * buf, int count);

enum audio_format_t {
	AUDIO_FORMAT_BIT8	= 8,
	AUDIO_FORMAT_BIT16	= 16,
	AUDIO_FORMAT_BIT24	= 24,
	AUDIO_FORMAT_BIT32	= 32,
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

struct audio_t
{
	/* The audio name */
	char * name;

	/* Initialize the audio */
	void (*init)(struct audio_t * audio);

	/* Clean up the audio */
	void (*exit)(struct audio_t * audio);

	/* Audio playback open */
	void (*playback_open)(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data);

	/* Audio playback start */
	void (*playback_start)(struct audio_t * audio);

	/* Audio playback stop */
	void (*playback_stop)(struct audio_t * audio);

	/* Audio playback close */
	void (*playback_close)(struct audio_t * audio);

	/* Audio capture open */
	void (*capture_open)(struct audio_t * audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void * data);

	/* Audio capture start */
	void (*capture_start)(struct audio_t * audio);

	/* Audio capture stop */
	void (*capture_stop)(struct audio_t * audio);

	/* Audio capture close */
	void (*capture_close)(struct audio_t * audio);

	/* Suspend audio */
	void (*suspend)(struct audio_t * audio);

	/* Resume audio */
	void (*resume)(struct audio_t * audio);

	/* Private data */
	void * priv;
};

struct audio_t * search_audio(const char * name);
struct audio_t * search_first_audio(void);
bool_t register_audio(struct audio_t * audio);
bool_t unregister_audio(struct audio_t * audio);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_H__ */
