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

/* load 8k PRG-ROM */
void nes_load_prgrom_8k(nes_t* nes,uint8_t des, uint16_t src) {
    nes->nes_cpu.prg_banks[des] = nes->nes_rom.prg_rom + 8 * 1024 * src;
}

/* load 16k PRG-ROM */
void nes_load_prgrom_16k(nes_t* nes,uint8_t des, uint16_t src) {
    nes->nes_cpu.prg_banks[des * 2] = nes->nes_rom.prg_rom + 8 * 1024 * src * 2;
    nes->nes_cpu.prg_banks[des * 2 + 1] = nes->nes_rom.prg_rom + 8 * 1024 * (src * 2 + 1);
}

/* load 32k PRG-ROM */
void nes_load_prgrom_32k(nes_t* nes,uint8_t des, uint16_t src) {
    (void)des;
    nes->nes_cpu.prg_banks[0] = nes->nes_rom.prg_rom + 8 * 1024 * src * 4;
    nes->nes_cpu.prg_banks[1] = nes->nes_rom.prg_rom + 8 * 1024 * (src * 4 + 1);
    nes->nes_cpu.prg_banks[2] = nes->nes_rom.prg_rom + 8 * 1024 * (src * 4 + 2);
    nes->nes_cpu.prg_banks[3] = nes->nes_rom.prg_rom + 8 * 1024 * (src * 4 + 3);
}

/* load 1k CHR-ROM */
void nes_load_chrrom_1k(nes_t* nes,uint8_t des, uint8_t src) {
    nes->nes_ppu.pattern_table[des] = nes->nes_rom.chr_rom + 1024 * src;
}

/* load 4k CHR-ROM */
void nes_load_chrrom_4k(nes_t* nes,uint8_t des, uint8_t src) {
    for (size_t i = 0; i < 4; i++){
        nes->nes_ppu.pattern_table[des * 4 + i] = nes->nes_rom.chr_rom + 1024 * (src * 4 + i);
    }
}

/* load 8k CHR-ROM */
void nes_load_chrrom_8k(nes_t* nes,uint8_t des, uint8_t src) {
    for (size_t i = 0; i < 8; i++){
        nes->nes_ppu.pattern_table[des + i] = nes->nes_rom.chr_rom + 1024 * (src * 8 + i);
    }
}

#define NES_CASE_LOAD_MAPPER(mapper_id) case mapper_id: return nes_mapper##mapper_id##_init(nes)

int nes_load_mapper(nes_t* nes){
    switch (nes->nes_rom.mapper_number){
        NES_CASE_LOAD_MAPPER(0);
        NES_CASE_LOAD_MAPPER(1);
        NES_CASE_LOAD_MAPPER(2);
        NES_CASE_LOAD_MAPPER(3);
        // NES_CASE_LOAD_MAPPER(4);
        NES_CASE_LOAD_MAPPER(7);
        NES_CASE_LOAD_MAPPER(94);
        NES_CASE_LOAD_MAPPER(117);
        NES_CASE_LOAD_MAPPER(180);
        default :
            NES_LOG_ERROR("mapper:%03d is unsupported\n",nes->nes_rom.mapper_number);
            return NES_ERROR;
    }
}
