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

//https://www.nesdev.org/pal.txt

static nes_color_t nes_palette[]={
#if (NES_COLOR_DEPTH == 32) // ARGB8888
    0xFF757575, 0xFF271B8F, 0xFF0000AB, 0xFF47009F, 0xFF8F0077, 0xFFB0013, 0xFFA70000, 0xFF7F0B00,0xFF432F00, 0xFF004700, 0xFF005100, 0xFF003F17, 0xFF1B3F5F, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFBCBCBC, 0xFF0073EF, 0xFF233BEF, 0xFF8300F3, 0xFFBF00BF, 0xFF7005B, 0xFFDB2B00, 0xFFCB4F0F,0xFF8B7300, 0xFF009700, 0xFF00AB00, 0xFF00933B, 0xFF00838B, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFFFFFFF, 0xFF3FBFFF, 0xFF5F97FF, 0xFFA78BFD, 0xFFF77BFF, 0xFFF77B7, 0xFFFF7763, 0xFFFF9B3B,0xFFF3BF3F, 0xFF83D313, 0xFF4FDF4B, 0xFF58F898, 0xFF00EBDB, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFFFFFFF, 0xFFABE7FF, 0xFFC7D7FF, 0xFFD7CBFF, 0xFFFFC7FF, 0xFFFC7DB, 0xFFFFBFB3, 0xFFFFDBAB,0xFFFFE7A3, 0xFFE3FFA3, 0xFFABF3BF, 0xFFB3FFCF, 0xFF9FFFF3, 0xFF000000, 0xFF000000, 0xFF000000,
#elif (NES_COLOR_DEPTH == 16)
#if (NES_COLOR_SWAP == 0) // RGB565
    0x73AE, 0x20D1, 0x0015, 0x4013, 0x880E, 0x0802, 0xA000, 0x7840,0x4160, 0x0220, 0x0280, 0x01E2, 0x19EB, 0x0000, 0x0000, 0x0000,
    0xBDF7, 0x039D, 0x21DD, 0x801E, 0xB817, 0x000B, 0xD940, 0xCA61,0x8B80, 0x04A0, 0x0540, 0x0487, 0x0411, 0x0000, 0x0000, 0x0000,
    0xFFFF, 0x3DFF, 0x5CBF, 0xA45F, 0xF3DF, 0x0BB6, 0xFBAC, 0xFCC7,0xF5E7, 0x8682, 0x4EE9, 0x5FD3, 0x075B, 0x0000, 0x0000, 0x0000,
    0xFFFF, 0xAF3F, 0xC6BF, 0xD65F, 0xFE3F, 0x0E3B, 0xFDF6, 0xFED5,0xFF34, 0xE7F4, 0xAF97, 0xB7F9, 0x9FFE, 0x0000, 0x0000, 0x0000,
#else // RGB565_SWAP
    0xAE73, 0xD120, 0x1500, 0x1340, 0x0E88, 0x0208, 0x00A0, 0x4078,0x6041, 0x2002, 0x8002, 0xE201, 0xEB19, 0x0000, 0x0000, 0x0000,
    0xF7BD, 0x9D03, 0xDD21, 0x1E80, 0x17B8, 0x0B00, 0x40D9, 0x61CA,0x808B, 0xA004, 0x4005, 0x8704, 0x1104, 0x0000, 0x0000, 0x0000,
    0xFFFF, 0xFF3D, 0xBF5C, 0x5FA4, 0xDFF3, 0xB60B, 0xACFB, 0xC7FC,0xE7F5, 0x8286, 0xE94E, 0xD35F, 0x5B07, 0x0000, 0x0000, 0x0000,
    0xFFFF, 0x3FAF, 0xBFC6, 0x5FD6, 0x3FFE, 0x3B0E, 0xF6FD, 0xD5FE,0x34FF, 0xF4E7, 0x97AF, 0xF9B7, 0xFE9F, 0x0000, 0x0000, 0x0000,
#endif /* NES_COLOR_SWAP */
#endif /* NES_COLOR_DEPTH */
};

nes_t* nes_init(void){
    nes_t* nes = (nes_t *)nes_malloc(sizeof(nes_t));
    if (nes == NULL) {
        return NULL;
    }
    nes_memset(nes, 0, sizeof(nes_t));
    nes_initex(nes);
    return nes;
}

int nes_deinit(nes_t *nes){
    nes->nes_quit = 1;
    nes_deinitex(nes);
    if (nes){
        nes_free(nes);
        nes = NULL;
    }
    return NES_OK;
}

static inline void nes_palette_generate(nes_t* nes){
    for (uint8_t i = 0; i < 32; i++) {
        nes->nes_ppu.palette[i] = nes_palette[nes->nes_ppu.palette_indexes[i]];
    }
    for (uint8_t i = 1; i < 8; i++){
        nes->nes_ppu.palette[4 * i] = nes->nes_ppu.palette[0];
    }
}

static void nes_render_background_line(nes_t* nes,uint16_t scanline,nes_color_t* draw_data){
    (void)scanline;
    uint8_t p = 0;
    int8_t m = 7 - nes->nes_ppu.x;
    const uint8_t dx = (const uint8_t)nes->nes_ppu.v.coarse_x;
    const uint8_t dy = (const uint8_t)nes->nes_ppu.v.fine_y;
    const uint8_t tile_y = (const uint8_t)nes->nes_ppu.v.coarse_y;
    uint8_t nametable_id = (uint8_t)nes->nes_ppu.v.nametable;
    for (uint8_t tile_x = dx; tile_x < 32; tile_x++){
        const uint8_t pattern_id = nes->nes_ppu.name_table[nametable_id][tile_x + (tile_y << 5)];
        // if (pattern_id == 0x40){
        //     uint8_t tile_x1 = (tile_x + 1) & 0x1F;
        //     NES_LOG_DEBUG("scanline:%d pattern_id:0x%02x dx:%d dy:%d tile_x:%d tile_y:%d\n",scanline,pattern_id,dx,dy,tile_x,tile_y);
        //     NES_LOG_DEBUG("tile_x1:%d\n",tile_x1);
        // }
        // if (scanline == 170 && tile_x == 10)
        // {
        //     printf("scanline:%d pattern_id:0x%02x dx:%d dy:%d tile_x:%d tile_y:%d\n",scanline,pattern_id,dx,dy,tile_x,tile_y);
        // }

        const uint8_t* bit0_p = nes->nes_ppu.pattern_table[nes->nes_ppu.CTRL_B ? 4 : 0] + pattern_id * 16;
        const uint8_t* bit1_p = bit0_p + 8;
        const uint8_t bit0 = bit0_p[dy];
        const uint8_t bit1 = bit1_p[dy];
        const uint8_t attribute = nes->nes_ppu.name_table[nametable_id][960 + ((tile_y >> 2) << 3) + (tile_x >> 2)];
        // 1:D4-D5/D6-D7 0:D0-D1/D2-D3
        // 1:D2-D3/D6-D7 0:D0-D1/D4-D5
        const uint8_t high_bit = ((attribute >> (((tile_y & 2) << 1) | (tile_x & 2))) & 3) << 2;
        for (; m >= 0; m--){
            uint8_t low_bit = ((bit0 >> m) & 0x01) | ((bit1 >> m)<<1 & 0x02);
            uint8_t palette_index = (high_bit & 0x0c) | low_bit;
            draw_data[p++] = nes->nes_ppu.background_palette[palette_index];
        }
        m = 7;
    }
    nametable_id ^= 1;
    for (uint8_t tile_x = 0; tile_x <= dx; tile_x++){
        const uint8_t pattern_id = nes->nes_ppu.name_table[nametable_id][tile_x + (tile_y << 5)];
        const uint8_t* bit0_p = nes->nes_ppu.pattern_table[nes->nes_ppu.CTRL_B ? 4 : 0] + pattern_id * 16;
        const uint8_t* bit1_p = bit0_p + 8;
        const uint8_t bit0 = bit0_p[dy];
        const uint8_t bit1 = bit1_p[dy];
        const uint8_t attribute = nes->nes_ppu.name_table[nametable_id][960 + ((tile_y >> 2) << 3) + (tile_x >> 2)];
        // 1:D4-D5/D6-D7 0:D0-D1/D2-D3
        // 1:D2-D3/D6-D7 0:D0-D1/D4-D5
        const uint8_t high_bit = ((attribute >> (((tile_y & 2) << 1) | (tile_x & 2))) & 3) << 2;
        uint8_t skew = 0;
        if (tile_x == dx){
            if (nes->nes_ppu.x){
                skew = 8 - nes->nes_ppu.x;
            }else
                break;
        }
        for (; m >= skew; m--){
            const uint8_t low_bit = ((bit0 >> m) & 0x01) | ((bit1 >> m)<<1 & 0x02);
            const uint8_t palette_index = (high_bit & 0x0c) | low_bit;
            draw_data[p++] = nes->nes_ppu.background_palette[palette_index];
        }
        m = 7;
    }
}

static void nes_render_sprite_line(nes_t* nes,uint16_t scanline,nes_color_t* draw_data){
    const nes_color_t background_color = nes->nes_ppu.background_palette[0];
    uint8_t sprite[8] = {0};
    uint8_t sprite_numbers = 0;
    const uint8_t sprite_size = nes->nes_ppu.CTRL_H?16:8;

    // 遍历显示的精灵和检测是否精灵溢出
    for (uint8_t i = 0; i < 64; i++){
        if (nes->nes_ppu.sprite_info[i].y >= 0xEF){
            continue;
        }
        uint8_t sprite_y = (uint8_t)(nes->nes_ppu.sprite_info[i].y + 1);
        if (scanline < sprite_y || scanline >= sprite_y + sprite_size){
            continue;
        }
        if (sprite_numbers==8){
            nes->nes_ppu.STATUS_O = 1;
            break;
        }
        sprite[sprite_numbers++]=i;
    }
    // 显示精灵
    for (uint8_t sprite_number = sprite_numbers; sprite_number > 0; sprite_number--){
        const uint8_t sprite_id = sprite[sprite_number-1];
        const sprite_info_t sprite_info = nes->nes_ppu.sprite_info[sprite_id];
        const uint8_t sprite_y = (uint8_t)(sprite_info.y + 1);
        const uint8_t* sprite_bit0_p = nes->nes_ppu.pattern_table[nes->nes_ppu.CTRL_H?((sprite_info.pattern_8x16)?4:0):(nes->nes_ppu.CTRL_S?4:0)] \
                                        + (nes->nes_ppu.CTRL_H?(sprite_info.tile_index_8x16 << 1 ):(sprite_info.tile_index_number)) * 16;
        const uint8_t* sprite_bit1_p = sprite_bit0_p + 8;

        uint8_t dy = (uint8_t)(scanline - sprite_y);

        if (nes->nes_ppu.CTRL_H){
            if (sprite_info.flip_v){
                if (dy < 8){
                    sprite_bit0_p +=16;
                    sprite_bit1_p +=16;
                    dy = sprite_size - dy - 1 -8;
                }else{
                    dy = sprite_size - dy - 1;
                }
            }else{
                if (dy > 7){
                    sprite_bit0_p +=16;
                    sprite_bit1_p +=16;
                    dy-=8;
                }
            }
        }else{
            if (sprite_info.flip_v){
                dy = sprite_size - dy - 1;
            }
        }

        const uint8_t sprite_bit0 = sprite_bit0_p[dy];
        const uint8_t sprite_bit1 = sprite_bit1_p[dy];
#if (NES_FRAME_SKIP != 0)
        if(nes->nes_frame_skip_count == 0)
#endif
        {
            uint8_t p = sprite_info.x;
            if (sprite_info.flip_h){
                for (int8_t m = 0; m <= 7; m++){
                    const uint8_t low_bit = ((sprite_bit0 >> m) & 0x01) | ((sprite_bit1 >> m)<<1 & 0x02);
                    const uint8_t palette_index = (sprite_info.sprite_palette << 2) | low_bit;
                    if (palette_index%4 != 0){
                        if (sprite_info.priority){
                            if (draw_data[p] == background_color){
                                draw_data[p] = nes->nes_ppu.sprite_palette[palette_index];
                            }
                        }else{
                            draw_data[p] = nes->nes_ppu.sprite_palette[palette_index];
                        }
                    }
                    if (p == 255)
                        break;
                    p++;
                }
            }else{
                for (int8_t m = 7; m >= 0; m--){
                    const uint8_t low_bit = ((sprite_bit0 >> m) & 0x01) | ((sprite_bit1 >> m)<<1 & 0x02);
                    const uint8_t palette_index = (sprite_info.sprite_palette << 2) | low_bit;
                    if (palette_index%4 != 0){
                        if (sprite_info.priority){
                            if (draw_data[p] == background_color){
                                draw_data[p] = nes->nes_ppu.sprite_palette[palette_index];
                            }
                        }else{
                            draw_data[p] = nes->nes_ppu.sprite_palette[palette_index];
                        }
                    }
                    if (p == 255)
                        break;
                    p++;
                }
            }
        }
        // 检测精灵0命中
        if (sprite_id==0){
            const uint8_t sprite_date = sprite_bit0 | sprite_bit1;
            if (sprite_date && nes->nes_ppu.MASK_b && nes->nes_ppu.STATUS_S == 0){
                // printf("scanline:%d x:%d MASK_m:%d MASK_M:%d\n",
                //     scanline,nes->nes_ppu.x,nes->nes_ppu.MASK_m,nes->nes_ppu.MASK_M);
                const uint8_t nametable_id = (uint8_t)nes->nes_ppu.v.nametable;
                const uint8_t tile_x = (nes->nes_ppu.sprite_info[0].x) >> 3;
                const uint8_t tile_y = (uint8_t)(scanline >> 3);
                const uint8_t pattern_id = nes->nes_ppu.name_table[nametable_id][tile_x + (tile_y << 5)];
                const uint8_t* bit0_p = nes->nes_ppu.pattern_table[nes->nes_ppu.CTRL_B ? 4 : 0] + pattern_id * 16;
                const uint8_t* bit1_p = bit0_p + 8;
                const uint8_t background_date = bit0_p[dy] | bit1_p[dy] << 1;
                if (sprite_date & background_date){
                    nes->nes_ppu.STATUS_S = 1;
                    // printf("scanline:%d sprite_bit0:%d sprite_bit1:%d sprite_date:%d bit0_p:%d bit1_p:%d background_date:%d \n",
                    // scanline,sprite_bit0,sprite_bit1,sprite_date,bit0_p[dy],bit1_p[dy],background_date);
                }
            }
        }

    }
}

// https://www.nesdev.org/wiki/PPU_rendering


// static void nes_background_pattern_test(nes_t* nes){
//     nes_palette_generate(nes);
//     nes_memset(nes->nes_draw_data, nes->nes_ppu.background_palette[0], sizeof(nes_color_t) * NES_DRAW_SIZE);

//     uint8_t nametable_id = 0;
//     for (uint8_t j = 0; j < 16 * 8; j++){
//         uint16_t p = j*NES_WIDTH;
//         uint8_t tile_y = j/8;
//         uint8_t dy = j%8;
//         int8_t m = 7;
//         for (uint8_t i = 0; i < 16; i++){
//             uint8_t tile_x = i;
//             const uint8_t pattern_id = tile_y*16 + tile_x;
//             const uint8_t* bit0_p = nes->nes_ppu.pattern_table[1 ? 4 : 0] + pattern_id * 16;
//             const uint8_t* bit1_p = bit0_p + 8;
//             const uint8_t bit0 = bit0_p[dy];
//             const uint8_t bit1 = bit1_p[dy];
//             const uint8_t attribute = nes->nes_ppu.name_table[nametable_id][960 + ((tile_y >> 2) << 3) + (tile_x >> 2)];
//             const uint8_t high_bit = ((attribute >> (((tile_y & 2) << 1) | (tile_x & 2))) & 3) << 2;
//             for (; m >= 0; m--){
//                 uint8_t low_bit = ((bit0 >> m) & 0x01) | ((bit1 >> m)<<1 & 0x02);
//                 uint8_t palette_index = (high_bit & 0x0c) | low_bit;
//                 nes->nes_draw_data[p++] = nes->nes_ppu.background_palette[palette_index];
//             }
//             m = 7;
//         }
//     }
//     nes_draw(0, 0, NES_WIDTH-1, NES_HEIGHT-1, nes->nes_draw_data);
//     nes_frame(nes);
// }

void nes_run(nes_t* nes){
    NES_LOG_DEBUG("mapper:%03d\n",nes->nes_rom.mapper_number);
    NES_LOG_DEBUG("prg_rom_size:%d*16kB\n",nes->nes_rom.prg_rom_size);
    NES_LOG_DEBUG("chr_rom_size:%d*8kB\n",nes->nes_rom.chr_rom_size);
    NES_LOG_DEBUG("mirroring_type:%d\n",nes->nes_rom.mirroring_type);
    NES_LOG_DEBUG("four_screen:%d\n",nes->nes_rom.four_screen);
    // NES_LOG_DEBUG("save_ram:%d\n",nes->nes_rom.save_ram);

    nes_cpu_reset(nes);
    uint64_t frame_cnt = 0;

    while (!nes->nes_quit){
        // NES_LOG_DEBUG("frame_cnt:%d\n",frame_cnt);
        frame_cnt++;
#if (NES_FRAME_SKIP != 0)
        if(nes->nes_frame_skip_count == 0)
#endif
        {
            nes_palette_generate(nes);
        }
        if (nes->nes_ppu.MASK_b == 0){
#if (NES_FRAME_SKIP != 0)
            if(nes->nes_frame_skip_count == 0)
#endif
            {
                nes_memset(nes->nes_draw_data, nes->nes_ppu.background_palette[0], sizeof(nes_color_t) * NES_DRAW_SIZE);
            }
        }
#if (NES_ENABLE_SOUND==1)
        nes_apu_frame(nes);
#endif
        // https://www.nesdev.org/wiki/PPU_rendering#Visible_scanlines_(0-239)
        for(nes->scanline = 0; nes->scanline < NES_HEIGHT; nes->scanline++) { // 0-239 Visible frame
            if (nes->nes_ppu.MASK_b){
#if (NES_FRAME_SKIP != 0)
                if (nes->nes_frame_skip_count == 0)
#endif
                {
#if (NES_RAM_LACK == 1)
                nes_render_background_line(nes, nes->scanline, nes->nes_draw_data + nes->scanline%(NES_HEIGHT/2) * NES_WIDTH);
#else
                nes_render_background_line(nes, nes->scanline, nes->nes_draw_data + nes->scanline * NES_WIDTH);
#endif
                }
            }
            if (nes->nes_ppu.MASK_s){
#if (NES_RAM_LACK == 1)
                nes_render_sprite_line(nes, nes->scanline,nes->nes_draw_data + nes->scanline%(NES_HEIGHT/2) * NES_WIDTH);
#else
                nes_render_sprite_line(nes, nes-> scanline,nes->nes_draw_data + nes->scanline * NES_WIDTH);
#endif
            }
            nes_opcode(nes,85); // ppu cycles: 85*3=255
            // https://www.nesdev.org/wiki/PPU_scrolling#Wrapping_around
            if (nes->nes_ppu.MASK_b){
                // https://www.nesdev.org/wiki/PPU_scrolling#At_dot_256_of_each_scanline
                if ((nes->nes_ppu.v.fine_y) < 7) {
                    nes->nes_ppu.v.fine_y++;
                }else {
                    nes->nes_ppu.v.fine_y = 0;
                    uint8_t y = (uint8_t)(nes->nes_ppu.v.coarse_y);
                    if (y == 29) {
                        y = 0;
                        nes->nes_ppu.v_reg ^= 0x0800;
                    }else if (y == 31) {
                        y = 0;
                    }else {
                        y++;
                    }
                    nes->nes_ppu.v.coarse_y = y;
                }
                // https://www.nesdev.org/wiki/PPU_scrolling#At_dot_257_of_each_scanline
                // v: ....A.. ...BCDEF <- t: ....A.. ...BCDEF
                nes->nes_ppu.v_reg = (nes->nes_ppu.v_reg & (uint16_t)0xFBE0) | (nes->nes_ppu.t_reg & (uint16_t)0x041F);
            }
            nes_opcode(nes,NES_PPU_CPU_CLOCKS-85);
#if (NES_ENABLE_SOUND==1)
            if (nes->scanline % 66 == 65) nes_apu_frame(nes);
#endif
#if (NES_RAM_LACK == 1)
#if (NES_FRAME_SKIP != 0)
            if(nes->nes_frame_skip_count == 0)
#endif
            {
                if (nes->scanline == NES_HEIGHT/2-1){
                    nes_draw(0, 0, NES_WIDTH-1, NES_HEIGHT/2-1, nes->nes_draw_data);
                }else if(nes->scanline == NES_HEIGHT-1){
                    nes_draw(0, NES_HEIGHT/2, NES_WIDTH-1, NES_HEIGHT-1, nes->nes_draw_data);
                }
            }
#endif
        }
#if (NES_RAM_LACK == 0)
#if (NES_FRAME_SKIP != 0)
        if(nes->nes_frame_skip_count == 0)
#endif
        {
            nes_draw(0, 0, NES_WIDTH-1, NES_HEIGHT-1, nes->nes_draw_data);
        }
#endif
        nes_opcode(nes,NES_PPU_CPU_CLOCKS); //240 Post-render line

        nes->nes_ppu.STATUS_V = 1;// Set VBlank flag (241 line)
        if (nes->nes_ppu.CTRL_V) {
            nes->nes_cpu.irq_nmi=1;
        }

        for(uint8_t i = 0; i < 20; i++){ // 241-260行 垂直空白行 x20
            nes_opcode(nes,NES_PPU_CPU_CLOCKS);
        }
        nes->nes_ppu.ppu_status = 0;    // Clear:VBlank,Sprite 0,Overflow
        nes_opcode(nes,NES_PPU_CPU_CLOCKS); // Pre-render scanline (-1 or 261)

        if (nes->nes_ppu.MASK_b){
            // https://www.nesdev.org/wiki/PPU_scrolling#During_dots_280_to_304_of_the_pre-render_scanline_(end_of_vblank)
            // v: GHIA.BC DEF..... <- t: GHIA.BC DEF.....
            nes->nes_ppu.v_reg = (nes->nes_ppu.v_reg & (uint16_t)0x841F) | (nes->nes_ppu.t_reg & (uint16_t)0x7BE0);
        }
        nes_frame(nes);
#if (NES_FRAME_SKIP != 0)
        if ( ++nes->nes_frame_skip_count > NES_FRAME_SKIP){
            nes->nes_frame_skip_count = 0;
        }
#endif
    }
}
