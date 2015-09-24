#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandbox.h>

void sandbox_sdl_audio_init(void)
{
}

void sandbox_sdl_audio_exit(void)
{
}

void sandbox_sdl_audio_open(int fmt, int rate, int ch, int sample, void(*cb)(void *, void *, int), void * data)
{
	SDL_AudioSpec spec;

	switch(fmt)
	{
	case 8:
		spec.format = AUDIO_S8;
		break;
	case 16:
		spec.format = AUDIO_S16LSB;
		break;
	case 24:
		spec.format = AUDIO_S16LSB;
		break;
	case 32:
		spec.format = AUDIO_S32LSB;
		break;
	default:
		spec.format = AUDIO_S16LSB;
		break;
	}
	spec.freq = rate;
	spec.channels = ch;
	spec.samples = sample;
	spec.callback = (SDL_AudioCallback)cb;
	spec.userdata = data;

	SDL_LockAudio();
	SDL_OpenAudio(&spec, 0);
	SDL_UnlockAudio();
}

void sandbox_sdl_audio_close(void)
{
	SDL_CloseAudio();
}

void sandbox_sdl_audio_start(void)
{
	if(SDL_GetAudioStatus() != SDL_AUDIO_PLAYING)
		SDL_PauseAudio(0);
}

void sandbox_sdl_audio_stop(void)
{
	if(SDL_GetAudioStatus() != SDL_AUDIO_PAUSED)
		SDL_PauseAudio(1);
}

struct audio_wave_callback_data_t
{
	int frequency;
	int amplitude;
	int sample;
	int channel;
	int phase;
};
static struct audio_wave_callback_data_t wcd;

static void audio_wave_callback(void * data, void * buf, int count)
{
	struct audio_wave_callback_data_t * wcd = (struct audio_wave_callback_data_t *)data;
	uint16_t * p = buf;
	int i;

	for(i = 0; i < count / 4; i++)
	{
		uint16_t v = (uint16_t)(wcd->amplitude * sin((2 * M_PI * (wcd->phase++) * wcd->frequency) / wcd->sample));
		p[2*i] = v;
		p[2*i + 1] = v;
	}
}

void sandbox_sdl_audio_buzzer_init(void)
{
	SDL_AudioSpec spec;

	wcd.amplitude = 0x7fff;
	wcd.frequency = 440;
	wcd.sample = 44100;
	wcd.channel = 2;
	wcd.phase = 0;

	spec.freq = wcd.sample;
	spec.format = AUDIO_S16LSB;
	spec.channels = wcd.channel;
	spec.samples = 1024;
	spec.callback = (SDL_AudioCallback)audio_wave_callback;
	spec.userdata = &wcd;

	SDL_LockAudio();
	SDL_OpenAudio(&spec, 0);
	SDL_UnlockAudio();
}

void sandbox_sdl_audio_buzzer_exit(void)
{

}

void sandbox_sdl_audio_buzzer_set_frequency(int frequency)
{
	if(frequency == 0)
		sandbox_sdl_audio_stop();
	else
		sandbox_sdl_audio_start();
	wcd.frequency = frequency;
}
