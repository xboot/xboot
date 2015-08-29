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
