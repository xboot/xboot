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

/* https://www.nesdev.org/wiki/MMC1 */


typedef struct  {
    uint8_t shift;
    union {
        struct {
            uint8_t M:2;
            uint8_t P:2;
            uint8_t C:1;
            uint8_t  :3;
        }control;
        uint8_t control_byte;
    };
} mapper1_register_t;

static mapper1_register_t mapper_register = {0};

static void nes_mapper_init(nes_t* nes){
    // CPU $6000-$7FFF: 8 KB PRG-RAM bank, (optional)

    // CPU $8000-$BFFF: 16 KB PRG-ROM bank, either switchable or fixed to the first bank
    nes_load_prgrom_16k(nes, 0, 0);
    // CPU $C000-$FFFF: 16 KB PRG-ROM bank, either fixed to the last bank or switchable
    nes_load_prgrom_16k(nes, 1, nes->nes_rom.prg_rom_size - 1);

    // PPU $0000-$0FFF: 4 KB switchable CHR bank
    // PPU $1000-$1FFF: 4 KB switchable CHR bank

    // CHR capacity:
    nes_load_chrrom_8k(nes, 0, 0);

    nes_memset(&mapper_register, 0x00, sizeof(mapper1_register_t));
    mapper_register.shift = 0x10;
}

static inline void nes_mapper_write_control(nes_t* nes, uint8_t data) {
    mapper_register.control_byte = data;
    nes_ppu_screen_mirrors(nes, mapper_register.control.M);
}
/*
CHR bank 0 (internal, $A000-$BFFF)
    4bit0
    -----
    CCCCC
    |||||
    +++++- Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode)
*/
static inline void nes_mapper_write_chrbank0(nes_t* nes) {
    if (mapper_register.control.C) {
        nes_load_chrrom_4k(nes, 0, mapper_register.shift);
    } else {
        nes_load_chrrom_8k(nes, 0, (mapper_register.shift & (uint8_t)0x0E));
    }
}
/*
CHR bank 1 (internal, $C000-$DFFF)
    4bit0
    -----
    CCCCC
    |||||
    +++++- Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)
*/
static inline void nes_mapper_write_chrbank1(nes_t* nes) {
    if (mapper_register.control.C)
        nes_load_chrrom_4k(nes, 1, mapper_register.shift);
}
/*
PRG bank (internal, $E000-$FFFF)
    4bit0
    -----
    RPPPP
    |||||
    |++++- Select 16 KB PRG-ROM bank (low bit ignored in 32 KB mode)
    +----- MMC1B and later: PRG-RAM chip enable (0: enabled; 1: disabled; ignored on MMC1A)
        MMC1A: Bit 3 bypasses fixed bank logic in 16K mode (0: fixed bank affects A17-A14;
        1: fixed bank affects A16-A14 and bit 3 directly controls A17)
*/
static inline void nes_mapper_write_prgbank(nes_t* nes) {
    const uint8_t bankid = mapper_register.shift & (uint8_t)0x0F;
    switch (mapper_register.control.P){
    case 0: case 1:
        // 32KB mode - switch both 16KB banks together
        nes_load_prgrom_32k(nes, 0, bankid & (uint8_t)0x0E);
        break;
    case 2:
        // Fix first bank at $8000 and switch 16KB bank at $C000
        nes_load_prgrom_16k(nes, 0, 0);
        nes_load_prgrom_16k(nes, 1, bankid);
        break;
    case 3:
        // Fix last bank at $C000 and switch 16KB bank at $8000
        nes_load_prgrom_16k(nes, 0, bankid);
        nes_load_prgrom_16k(nes, 1, nes->nes_rom.prg_rom_size - 1);
        break;
    }
}

static inline void nes_mapper_write_register(nes_t* nes, uint16_t address) {
    switch ((address & 0x7FFF) >> 13){
    case 0:
        nes_mapper_write_control(nes, mapper_register.shift);
        break;
    case 1:
        nes_mapper_write_chrbank0(nes);
        break;
    case 2:
        nes_mapper_write_chrbank1(nes);
        break;
    case 3:
        nes_mapper_write_prgbank(nes);
        break;
    }
}
/*
Load register ($8000-$FFFF)
    7  bit  0
    ---- ----
    Rxxx xxxD
    |       |
    |       +- Data bit to be shifted into shift register, LSB first
    +--------- A write with bit set will reset shift register
                and write Control with (Control OR $0C),
                locking PRG-ROM at $C000-$FFFF to the last bank.
*/
static void nes_mapper_write(nes_t* nes, uint16_t write_addr, uint8_t data){
    if (data & (uint8_t)0x80){
        mapper_register.shift = 0x10; // reset shift register
        nes_mapper_write_control(nes, mapper_register.control.P);
    }else {
        const uint8_t finished = mapper_register.shift & 1;
        mapper_register.shift >>= 1;
        mapper_register.shift |= (data & 1) << 4;
        if (finished) {
            nes_mapper_write_register(nes, write_addr);
            mapper_register.shift = 0x10;
        }
    }
}

int nes_mapper1_init(nes_t* nes){
    nes->nes_mapper.mapper_init = nes_mapper_init;
    nes->nes_mapper.mapper_write = nes_mapper_write;
    return 0;
}
