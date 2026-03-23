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

/* https://www.nesdev.org/wiki/INES_Mapper_177 */
static void nes_mapper_init(nes_t* nes){
    // CPU $8000-$FFFF: 32 KB switchable PRG ROM bank
    nes_load_prgrom_32k(nes, 0, 0);
    // CHR capacity: 8 KiB ROM.
    nes_load_chrrom_8k(nes, 0, 0);
}

/*
            7  bit  0
            ---------
$8000-FFFF: ..MP PPPP
              |+-++++- Select 32 KiB PRG-ROM bank at CPU $8000-$FFFF
              +------- Select nametable mirroring
                       0: Vertical
                       1: Horizontal
*/
typedef struct {
    uint8_t P:5;
    uint8_t M:1;
    uint8_t :2;
}bank_select_t;

static void nes_mapper_write(nes_t* nes, uint16_t address, uint8_t date) {
    (void)address;
    const bank_select_t* bank_select = (bank_select_t*)&date;
    nes_load_prgrom_32k(nes, 0, bank_select->P);
    nes_ppu_screen_mirrors(nes, bank_select->M?NES_MIRROR_HORIZONTAL:NES_MIRROR_VERTICAL);
}

int nes_mapper117_init(nes_t* nes){
    nes->nes_mapper.mapper_init = nes_mapper_init;
    nes->nes_mapper.mapper_write = nes_mapper_write;
    return 0;
}
