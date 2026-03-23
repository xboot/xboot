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
#include "nes_mapper.h"

/* https://www.nesdev.org/wiki/NROM */

static void nes_mapper_init(nes_t* nes){
    // $6000-$7FFF: Family Basic only: PRG RAM, mirrored as necessary to fill entire 8 KiB window, write protectable with an external switch.

    // CPU $8000-$BFFF: First 16 KB of ROM.
    nes_load_prgrom_16k(nes, 0, 0);
    // CPU $C000-$FFFF: Last 16 KB of ROM (NROM-256) or mirror of $8000-$BFFF (NROM-128).
    nes_load_prgrom_16k(nes, 1, nes->nes_rom.prg_rom_size - 1); // PRG-ROM 16k or 32k, set mirror.
    // CHR capacity: 8 KiB ROM (DIP-28 standard pinout) but most emulators support RAM.
    nes_load_chrrom_8k(nes, 0, 0);
}

static void nes_mapper_write(nes_t* nes, uint16_t write_addr, uint8_t data){
    (void)nes;
    (void)write_addr;
    (void)data;
}

int nes_mapper0_init(nes_t* nes){
    nes->nes_mapper.mapper_init = nes_mapper_init;
    nes->nes_mapper.mapper_write = nes_mapper_write;
    return 0;
}
