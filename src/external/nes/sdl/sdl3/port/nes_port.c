/*
 * Copyright PeakRacing
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "nes.h"

#include <SDL3/SDL.h>

/* memory */
void *nes_malloc(int num){
    return SDL_malloc(num);
}

void nes_free(void *address){
    SDL_free(address);
}

void *nes_memcpy(void *str1, const void *str2, size_t n){
    return SDL_memcpy(str1, str2, n);
}

void *nes_memset(void *str, int c, size_t n){
    return SDL_memset(str,c,n);
}

int nes_memcmp(const void *str1, const void *str2, size_t n){
    return SDL_memcmp(str1,str2,n);
}

#if (NES_USE_FS == 1)
/* io */
FILE *nes_fopen(const char * filename, const char * mode ){
    return fopen(filename,mode);
}

size_t nes_fread(void *ptr, size_t size, size_t nmemb, FILE *stream){
    return fread(ptr, size, nmemb,stream);
}

size_t nes_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
    return fwrite(ptr, size, nmemb,stream);
}

int nes_fseek(FILE *stream, long int offset, int whence){
    return fseek(stream,offset,whence);
}

int nes_fclose(FILE *stream ){
    return fclose(stream);
}
#endif

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *framebuffer = NULL;

static void sdl_event(nes_t *nes) {
    SDL_Event event;
    if (SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.scancode){
                    case 26://W
                        nes->nes_cpu.joypad.U1 = 1;
                        break;
                    case 22://S
                        nes->nes_cpu.joypad.D1 = 1;
                        break;
                    case 4://A
                        nes->nes_cpu.joypad.L1 = 1;
                        break;
                    case 7://D
                        nes->nes_cpu.joypad.R1 = 1;
                        break;
                    case 13://J
                        nes->nes_cpu.joypad.A1 = 1;
                        break;
                    case 14://K
                        nes->nes_cpu.joypad.B1 = 1;
                        break;
                    case 25://V
                        nes->nes_cpu.joypad.SE1 = 1;
                        break;
                    case 5://B
                        nes->nes_cpu.joypad.ST1 = 1;
                        break;
                    case 82://↑
                        nes->nes_cpu.joypad.U2 = 1;
                        break;
                    case 81://↓
                        nes->nes_cpu.joypad.D2 = 1;
                        break;
                    case 80://←
                        nes->nes_cpu.joypad.L2 = 1;
                        break;
                    case 79://→
                        nes->nes_cpu.joypad.R2 = 1;
                        break;
                    case 93://5
                        nes->nes_cpu.joypad.A2 = 1;
                        break;
                    case 94://6
                        nes->nes_cpu.joypad.B2 = 1;
                        break;
                    case 89://1
                        nes->nes_cpu.joypad.SE2 = 1;
                        break;
                    case 90://2
                        nes->nes_cpu.joypad.ST2 = 1;
                        break;
                    default:
                        break;
                    }
                break;
            case SDL_EVENT_KEY_UP:
                switch (event.key.scancode){
                    case 26://W
                        nes->nes_cpu.joypad.U1 = 0;
                        break;
                    case 22://S
                        nes->nes_cpu.joypad.D1 = 0;
                        break;
                    case 4://A
                        nes->nes_cpu.joypad.L1 = 0;
                        break;
                    case 7://D
                        nes->nes_cpu.joypad.R1 = 0;
                        break;
                    case 13://J
                        nes->nes_cpu.joypad.A1 = 0;
                        break;
                    case 14://K
                        nes->nes_cpu.joypad.B1 = 0;
                        break;
                    case 25://V
                        nes->nes_cpu.joypad.SE1 = 0;
                        break;
                    case 5://B
                        nes->nes_cpu.joypad.ST1 = 0;
                        break;
                    case 82://↑
                        nes->nes_cpu.joypad.U2 = 0;
                        break;
                    case 81://↓
                        nes->nes_cpu.joypad.D2 = 0;
                        break;
                    case 80://←
                        nes->nes_cpu.joypad.L2 = 0;
                        break;
                    case 79://→
                        nes->nes_cpu.joypad.R2 = 0;
                        break;
                    case 93://5
                        nes->nes_cpu.joypad.A2 = 0;
                        break;
                    case 94://6
                        nes->nes_cpu.joypad.B2 = 0;
                        break;
                    case 89://1
                        nes->nes_cpu.joypad.SE2 = 0;
                        break;
                    case 90://2
                        nes->nes_cpu.joypad.ST2 = 0;
                        break;
                    default:
                        break;
                    }
                break;
            case SDL_EVENT_QUIT:
                nes_deinit(nes);
                return;
        }
    }
}

#if (NES_ENABLE_SOUND == 1)

#define SDL_AUDIO_NUM_CHANNELS          (1)
static SDL_AudioStream* nes_audio_stream = NULL;

static uint8_t apu_output = 0;
static void AudioCallback(void* userdata, SDL_AudioStream* astream, int additional_amount, int total_amount) {
    (void)total_amount;
    nes_t *nes = (nes_t*)userdata;
    static int total = NES_APU_SAMPLE_PER_SYNC;
    if (apu_output){
        uint8_t samples[441] = {0};
        uint8_t* nes_sample_buffer = &nes->nes_apu.sample_buffer;
        int StreamSend = SDL_min(additional_amount, total);

        nes_memcpy(samples, (StreamSend == additional_amount)? nes_sample_buffer : (nes_sample_buffer)+additional_amount, StreamSend);
        SDL_PutAudioStreamData(astream, samples, StreamSend);

        total -= StreamSend;
        if (total == 0){
            total = NES_APU_SAMPLE_PER_SYNC;
            apu_output = 0;
        }
    }
}

int nes_sound_output(uint8_t *buffer, size_t len){
    (void)buffer;
    (void)len;
    apu_output = 1;
    return 0;
}
#endif

int nes_initex(nes_t *nes){
    SDL_SetAppMetadata(NES_NAME, NES_VERSION_STRING, NES_URL);
    if (!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK| SDL_INIT_EVENTS)) {
        SDL_Log("Can not init video, %s", SDL_GetError());
        return -1;
    }
    if (!SDL_CreateWindowAndRenderer(NES_NAME,NES_WIDTH * 2, NES_HEIGHT * 2,      // 二倍分辨率
                                    SDL_WINDOW_OCCLUDED|SDL_WINDOW_HIGH_PIXEL_DENSITY,
                                    &window,&renderer)) {
        SDL_Log("Can not create window, %s", SDL_GetError());
        return -1;
    }
    framebuffer = SDL_CreateTexture(renderer,
                                    SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    NES_WIDTH,
                                    NES_HEIGHT);
#if (NES_ENABLE_SOUND == 1)
    SDL_AudioSpec spec = {
        .freq = NES_APU_SAMPLE_RATE,
        .format = SDL_AUDIO_S8,
        .channels = SDL_AUDIO_NUM_CHANNELS,
    };
    nes_audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, AudioCallback, nes);
    if (!nes_audio_stream) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open audio: %s\n", SDL_GetError());
    }
    SDL_ResumeAudioStreamDevice(nes_audio_stream);
#endif
    return 0;
}

int nes_deinitex(nes_t *nes){
    (void)nes;
    SDL_DestroyTexture(framebuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int nes_draw(int x1, int y1, int x2, int y2, nes_color_t* color_data){
    if (!framebuffer){
        return -1;
    }
    SDL_Rect rect;
    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1 + 1;
    rect.h = y2 - y1 + 1;
    SDL_UpdateTexture(framebuffer, &rect, color_data, rect.w * 4);
    return 0;
}

#define FRAMES_PER_SECOND   1000/60

void nes_frame(nes_t* nes){
    SDL_RenderTexture(renderer, framebuffer, NULL, NULL);
    SDL_RenderPresent(renderer);
    sdl_event(nes);
    SDL_Delay(FRAMES_PER_SECOND);
}
