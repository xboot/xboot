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

#include "nes_default.h"
#include "nes_log.h"
#include "nes_rom.h"
#include "nes_cpu.h"
#include "nes_ppu.h"
#include "nes_apu.h"
#include "nes_mapper.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define NES_VERSION_MAJOR       0
#define NES_VERSION_MINOR       0
#define NES_VERSION_PATCH       4

#define STRINGIFY_HELPER(x)     #x
#define NES_VERSION_STRING      STRINGIFY_HELPER(NES_VERSION_MAJOR) "." STRINGIFY_HELPER(NES_VERSION_MINOR) "." STRINGIFY_HELPER(NES_VERSION_PATCH)

#define NES_NAME                "NES"

#define NES_URL                 "https://github.com/PeakRacing/nes"

#define NES_WIDTH               256
#define NES_HEIGHT              240

// https://www.nesdev.org/wiki/Cycle_reference_chart
#define NES_CPU_CLOCK_FREQ      (1789773) // 21.47~ MHz ÷ 12 = 1.789773 MHz

// https://www.nesdev.org/w/images/default/4/4f/Ppu.svg
#define NES_PPU_CPU_CLOCKS		(113)     // 341 × 4 ÷ 12 = 113 2⁄3

#define NES_OK                  (0)
#define NES_ERROR               (-1)

typedef struct nes{
    uint8_t nes_quit;
#if (NES_FRAME_SKIP != 0)
    uint8_t nes_frame_skip_count;
#endif
    uint16_t scanline;
    nes_rom_info_t nes_rom;
    nes_cpu_t nes_cpu;
    nes_ppu_t nes_ppu;
#if (NES_ENABLE_SOUND==1)
    nes_apu_t nes_apu;
#endif
    nes_mapper_t nes_mapper;
    nes_color_t nes_draw_data[NES_DRAW_SIZE];
} nes_t;


nes_t* nes_init(void);
int nes_deinit(nes_t *nes);

void nes_run(nes_t* nes);

#if (NES_USE_FS == 1)
int nes_load_file(nes_t* nes, const char* file_path);
int nes_unload_file(nes_t* nes);
#endif

int nes_load_rom(nes_t* nes, const uint8_t* nes_rom);
int nes_unload_rom(nes_t* nes);

int nes_initex(nes_t* nes);
int nes_deinitex(nes_t* nes);
void nes_frame(nes_t* nes);

#ifdef __cplusplus
    }
#endif
