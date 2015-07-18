#ifndef __AUDIO_H__
#define __AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

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

	/* Audio open with params */
	void (*open)(struct audio_t * audio, enum audio_format_t fmt, enum audio_rate_t rate, int ch);

	/* Audio close */
	void (*close)(struct audio_t * audio);

	/* Audio playback */
	ssize_t (*playback)(struct audio_t * audio, const u8_t * buf, size_t count);

	/* Audio capture */
	ssize_t (*capture)(struct audio_t * audio, u8_t * buf, size_t count);

	/* Suspend audio */
	void (*suspend)(struct audio_t * audio);

	/* Resume audio */
	void (*resume)(struct audio_t * audio);

	/* Private data */
	void * priv;
};

struct audio_t * search_audio(const char * name);
bool_t register_audio(struct audio_t * audio);
bool_t unregister_audio(struct audio_t * audio);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_H__ */
