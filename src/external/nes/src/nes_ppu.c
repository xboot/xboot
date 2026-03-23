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

static inline uint8_t nes_read_ppu_memory(nes_t* nes){
    const uint16_t address = nes->nes_ppu.v_reg & (uint16_t)0x3FFF;
    const uint8_t index = address >> 10;
    const uint16_t offset = address & (uint16_t)0x3FF;
    if (address < (uint16_t)0x3F00) {// BANK
        uint8_t data = nes->nes_ppu.buffer;
        nes->nes_ppu.buffer = nes->nes_ppu.chr_banks[index][offset];
        return data;
    } else {// 调色板
        nes->nes_ppu.buffer = nes->nes_ppu.chr_banks[index][offset];
        return nes->nes_ppu.palette_indexes[address & (uint16_t)0x1f];
    }
}

static inline void nes_write_ppu_memory(nes_t* nes,uint8_t data){
    const uint16_t address = nes->nes_ppu.v_reg & (uint16_t)0x3FFF;
    if (address < (uint16_t)0x3F00) {// BANK
        nes->nes_ppu.chr_banks[(uint8_t)(address >> 10)][(uint16_t)(address & (uint16_t)0x3FF)] = data;
    } else {// 调色板
        if ((uint8_t)address & 0x03) {
            nes->nes_ppu.palette_indexes[(uint8_t)address & 0x1f] = data & 0x3F;
        } else {
            const uint8_t offset = (uint8_t)address & 0x0f;
            nes->nes_ppu.palette_indexes[offset] = nes->nes_ppu.palette_indexes[offset | 0x10] = data & 0x3F;
        }
    }
}

// https://www.nesdev.org/wiki/PPU_registers
uint8_t nes_read_ppu_register(nes_t* nes,uint16_t address){
    uint8_t data = 0;
    switch (address & (uint16_t)0x07){
        case 2://Status ($2002) < read
            // w:                  <- 0
            data = nes->nes_ppu.ppu_status;
            nes->nes_ppu.STATUS_V = 0;
            nes->nes_ppu.w = 0;
            break;
        case 4://OAM data ($2004) <> read/write
            data = nes->nes_ppu.oam_data[nes->nes_ppu.oam_addr];
            break;
        case 7://Data ($2007) <> read/write
            data = nes_read_ppu_memory(nes);
            nes->nes_ppu.v_reg += (uint16_t)((nes->nes_ppu.CTRL_I) ? 32 : 1);
            break;
        default : // ($2000 $2001 $2003 $2005 $2006)
            // NES_LOG_DEBUG("nes_read_ppu_register error %04X\n",address);
            // break;
            return nes->nes_ppu.oam_addr;
    }
    // NES_LOG_DEBUG("nes_read_ppu_register %04X %02X\n",address,data);
    return data;
}

void nes_write_ppu_register(nes_t* nes,uint16_t address, uint8_t data){
    // NES_LOG_DEBUG("nes_write_ppu_register %04X %02X\n",address,data);
    switch (address & (uint16_t)0x07){
        case 0://Controller ($2000) > write
            // t: ....GH.. ........ <- d: ......GH
            //     <used elsewhere> <- d: ABCDEF..
            nes->nes_ppu.ppu_ctrl = data;
            nes->nes_ppu.t.nametable = nes->nes_ppu.CTRL_N;
            break;
        case 1://Mask ($2001) > write
            nes->nes_ppu.ppu_mask = data;
            break;
        case 3://OAM address ($2003) > write
            nes->nes_ppu.oam_addr = data;
            break;
        case 4://OAM data ($2004) <> read/write
            nes->nes_ppu.oam_data[nes->nes_ppu.oam_addr++] = data;
            break;
        case 5://Scroll ($2005) >> write x2
            if (nes->nes_ppu.w) {   // w is 1
                // t: FGH..AB CDE..... <- d: ABCDEFGH
                // w:                  <- 0
                nes->nes_ppu.t.fine_y = (data & 0x07);
                nes->nes_ppu.t.coarse_y = (data & 0xF8)>>3;
                nes->nes_ppu.w = 0;
            } else {                // w is 0
                // t: ....... ...ABCDE <- d: ABCDE...
                // x:              FGH <- d: .....FGH
                // w:                  <- 1
                nes->nes_ppu.t.coarse_x = (data & 0xF8)>>3;
                nes->nes_ppu.x = (data & 0x07);
                nes->nes_ppu.w = 1;
            }
            break;
        case 6://Address ($2006) >> write x2
            if (nes->nes_ppu.w) {   // w is 1
                // t: ....... ABCDEFGH <- d: ABCDEFGH
                // v: <...all bits...> <- t: <...all bits...>
                // w:                  <- 0
                nes->nes_ppu.t_reg = (nes->nes_ppu.t_reg & (uint16_t)0xFF00) | (uint16_t)data;
                nes->nes_ppu.v_reg = nes->nes_ppu.t_reg;
                nes->nes_ppu.w = 0;
            } else {                // w is 0
                // t: ..CDEFGH ........ <- d: ..CDEFGH
                //         <unused>     <- d: AB......
                // t: .Z...... ........ <- 0 (bit Z is cleared)
                // w:                   <- 1
                nes->nes_ppu.t_reg = (nes->nes_ppu.t_reg & (uint16_t)0xFF) | (((uint16_t)data & 0x3F) << 8);
                nes->nes_ppu.w = 1;
            }
            break;
        case 7://Data ($2007) <> read/write
            nes_write_ppu_memory(nes,data);
            nes->nes_ppu.v_reg += (uint16_t)((nes->nes_ppu.CTRL_I) ? 32 : 1);
            break;
        default :
            NES_LOG_DEBUG("nes_write_ppu_register error %04X %02X\n",address,data);
            break;
    }
}

static const uint8_t nes_mirror_table[NES_MIRROR_COUNT][4] ={
    { 0, 1, 2, 3 }, // NES_MIRROR_FOUR_SCREEN
    { 0, 0, 1, 1 }, // NES_MIRROR_HORIZONTAL
    { 0, 1, 0, 1 }, // NES_MIRROR_VERTICAL
    { 0, 0, 0, 0 }, // NES_MIRROR_ONE_SCREEN0
    { 1, 1, 1, 1 }, // NES_MIRROR_ONE_SCREEN1
    { 0, 0, 0, 1 }, // NES_MIRROR_MAPPER
};


void nes_ppu_screen_mirrors(nes_t *nes,nes_mirror_type_t mirror_type){
    if (mirror_type == NES_MIRROR_AUTO){
        if (nes->nes_rom.four_screen) {             // four_screen
            mirror_type = NES_MIRROR_FOUR_SCREEN;
        } else if (nes->nes_rom.mirroring_type) {   // Vertical
            mirror_type = NES_MIRROR_VERTICAL;
        } else {                                    // Horizontal
            mirror_type = NES_MIRROR_HORIZONTAL;
        }
    }
    nes->nes_ppu.name_table[0] = nes->nes_ppu.ppu_vram[nes_mirror_table[mirror_type][0]];
    nes->nes_ppu.name_table[1] = nes->nes_ppu.ppu_vram[nes_mirror_table[mirror_type][1]];
    nes->nes_ppu.name_table[2] = nes->nes_ppu.ppu_vram[nes_mirror_table[mirror_type][2]];
    nes->nes_ppu.name_table[3] = nes->nes_ppu.ppu_vram[nes_mirror_table[mirror_type][3]];
    // mirrors
    nes->nes_ppu.name_table_mirrors[0] = nes->nes_ppu.name_table[0];
    nes->nes_ppu.name_table_mirrors[1] = nes->nes_ppu.name_table[1];
    nes->nes_ppu.name_table_mirrors[2] = nes->nes_ppu.name_table[2];
    nes->nes_ppu.name_table_mirrors[3] = nes->nes_ppu.name_table[3];
}

void nes_ppu_init(nes_t *nes){
    nes_ppu_screen_mirrors(nes,NES_MIRROR_AUTO);
}
