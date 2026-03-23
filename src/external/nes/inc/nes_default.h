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
#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nes_conf.h"

#ifdef __cplusplus
    extern "C" {
#endif

#if defined(__ARMCC_VERSION)
#define NES_WEAK                     __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define NES_WEAK                     __weak
#elif defined(__GNUC__)
#define NES_WEAK                     __attribute__((weak))
#elif defined(_MSC_VER)
#define NES_WEAK
#else
#define NES_WEAK                     __attribute__((weak))
#endif

#ifndef NES_ENABLE_SOUND
#define NES_ENABLE_SOUND        (0)
#endif

#ifndef NES_USE_FS
#define NES_USE_FS              (0)
#endif

#ifndef NES_FRAME_SKIP
#define NES_FRAME_SKIP          (0)
#endif

#ifndef NES_RAM_LACK
#define NES_RAM_LACK            (0)
#endif

#if (NES_RAM_LACK == 1)
#define NES_DRAW_SIZE           (NES_WIDTH * NES_HEIGHT / 2)
#else
#define NES_DRAW_SIZE           (NES_WIDTH * NES_HEIGHT)
#endif

#ifndef NES_COLOR_SWAP
#define NES_COLOR_SWAP          (0)
#endif

/* Color depth:
 * - 16: RGB565
 * - 32: ARGB8888
 */
#ifndef NES_COLOR_DEPTH
#define NES_COLOR_DEPTH         (32)
#endif

#if (NES_COLOR_DEPTH == 32)
#define nes_color_t uint32_t
#elif (NES_COLOR_DEPTH == 16)
#define nes_color_t uint16_t
#else
#error "no supprt color depth"
#endif

/* memory */
void *nes_malloc(int num);
void nes_free(void *address);
void *nes_memcpy(void *str1, const void *str2, size_t n);
void *nes_memset(void *str, int c, size_t n);
int nes_memcmp(const void *str1, const void *str2, size_t n);

#if (NES_USE_FS == 1)

/* io */
FILE *nes_fopen(const char * filename, const char * mode );
size_t nes_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t nes_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int nes_fseek(FILE *stream, long int offset, int whence);
int nes_fclose(FILE *stream );

#endif

int nes_draw(int x1, int y1, int x2, int y2, nes_color_t* color_data);
int nes_sound_output(uint8_t *buffer, size_t len);

#ifdef __cplusplus
    }
#endif
