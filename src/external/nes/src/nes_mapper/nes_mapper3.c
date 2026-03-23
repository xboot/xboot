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

/* https://www.nesdev.org/wiki/INES_Mapper_003 */
static void nes_mapper_init(nes_t* nes){
    // CPU $8000-$BFFF: First 16 KB of ROM.
    nes_load_prgrom_16k(nes, 0, 0);
    // CPU $C000-$FFFF: Last 16 KB of ROM or mirror of $8000-$BFFF.
    nes_load_prgrom_16k(nes, 1, nes->nes_rom.prg_rom_size - 1); // PRG ROM size: 16 KiB or 32 KiB, set mirror.
    // CHR capacity: 8 KiB ROM.
    nes_load_chrrom_8k(nes, 0, 0);
}

/*
    PPU $0000-$1FFF: 8 KB switchable CHR ROM bank
    7  bit  0
    ---- ----
    cccc ccCC
    |||| ||||
    ++++-++++- Select 8 KB CHR ROM bank for PPU $0000-$1FFF
    CNROM only implements the lowest 2 bits, capping it at 32 KiB CHR. Other boards may implement 4 or more bits for larger CHR.
*/
static void nes_mapper_write(nes_t* nes, uint16_t address, uint8_t date) {
    (void)address;
    const uint8_t bank = (date % nes->nes_rom.chr_rom_size);
    nes_load_chrrom_8k(nes, 0, bank);
}

int nes_mapper3_init(nes_t* nes){
    nes->nes_mapper.mapper_init = nes_mapper_init;
    nes->nes_mapper.mapper_write = nes_mapper_write;
    return 0;
}
