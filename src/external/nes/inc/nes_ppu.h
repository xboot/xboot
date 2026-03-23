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

#ifdef __cplusplus
    extern "C" {
#endif

#define NES_PPU_VRAM_SIZE       0x1000      /*  4KB */
#define NES_PPU_OAM_SIZE        0x100       /*  256B */

typedef enum {
    NES_MIRROR_FOUR_SCREEN  ,
    NES_MIRROR_HORIZONTAL   ,
    NES_MIRROR_VERTICAL     ,
    NES_MIRROR_ONE_SCREEN0  ,
    NES_MIRROR_ONE_SCREEN1  ,
    NES_MIRROR_MAPPER       ,
    NES_MIRROR_COUNT        ,
    NES_MIRROR_AUTO        ,
}nes_mirror_type_t;

struct nes;
typedef struct nes nes_t;

// https://www.nesdev.org/wiki/PPU_OAM
typedef struct{
    uint8_t	y;		                        /*  Y position of top of sprite */
    union {
        struct {
            uint8_t pattern_8x16:1;         /*  Bank ($0000 or $1000) of tiles */
            uint8_t tile_index_8x16 :7;     /*  Tile number of top of sprite (0 to 254; bottom half gets the next tile) */
        };
        uint8_t	tile_index_number;	        /*  Tile index number */
    };
    union {
        struct {
            uint8_t sprite_palette:2;       /*  Palette (4 to 7) of sprite */
            uint8_t :3;                     /*  nimplemented (read 0) */
            uint8_t priority :1;            /*  Priority (0: in front of background; 1: behind background) */
            uint8_t flip_h :1;              /*  Flip sprite horizontally */
            uint8_t flip_v :1;              /*  Flip sprite vertically */
        };
        uint8_t	attributes;	                /*  Attributes */
    };
    uint8_t	x;		                        /*  X position of left side of sprite. */
} sprite_info_t;

// https://www.nesdev.org/wiki/PPU_registers
typedef struct nes_ppu{
    union {
        struct {
            uint8_t CTRL_N:2;               /*  Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00) */
            uint8_t CTRL_I:1;               /*  VRAM address increment per CPU read/write of PPUDATA (0: add 1, going across; 1: add 32, going down) */
            uint8_t CTRL_S:1;               /*  Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode) */
            uint8_t CTRL_B:1;               /*  Background pattern table address (0: $0000; 1: $1000) */
            uint8_t CTRL_H:1;               /*  Sprite size (0: 8x8 pixels; 1: 8x16 pixels – see PPU OAM#Byte 1) */
            uint8_t CTRL_P:1;               /*  (0: read backdrop from EXT pins; 1: output color on EXT pins) */
            uint8_t CTRL_V:1;               /*  Generate an NMI at the start of the vertical blanking interval (0: off; 1: on) */
        };
        uint8_t ppu_ctrl;
    };
    union {
        struct {
            uint8_t MASK_Gr:1;              /*  Greyscale (0: normal color, 1: produce a greyscale display) */
            uint8_t MASK_m:1;               /*  1: Show background in leftmost 8 pixels of screen, 0: Hide */
            uint8_t MASK_M:1;               /*  1: Show sprites in leftmost 8 pixels of screen, 0: Hide */
            uint8_t MASK_b:1;               /*  1: Show background */
            uint8_t MASK_s:1;               /*  1: Show sprites */
            uint8_t MASK_R:1;               /*  Emphasize red (green on PAL/Dendy) */
            uint8_t MASK_G:1;               /*  Emphasize green (red on PAL/Dendy) */
            uint8_t MASK_B:1;               /*  Emphasize blue */
        };
        uint8_t ppu_mask;
    };
    union {
        struct {
            uint8_t  :5;
            uint8_t STATUS_O:1;             /*  Sprite overflow. The intent was for this flag to be set
                                                whenever more than eight sprites appear on a scanline, but a
                                                hardware bug causes the actual behavior to be more complicated
                                                and generate false positives as well as false negatives; see
                                                PPU sprite evaluation. This flag is set during sprite
                                                evaluation and cleared at dot 1 (the second dot) of the
                                                pre-render line. */
            uint8_t STATUS_S:1;             /*  Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps
                                                a nonzero background pixel; cleared at dot 1 of the pre-render
                                                line.  Used for raster timing. */
            uint8_t STATUS_V:1;             /*  Vertical blank has started (0: not in vblank; 1: in vblank).
                                                Set at dot 1 of line 241 (the line *after* the post-render
                                                line); cleared after reading $2002 and at dot 1 of the
                                                pre-render line. */
        };
        uint8_t ppu_status;
    };
    struct {
        uint8_t x:3;                        /*  Fine X scroll (3 bits) */
        uint8_t w:1;                        /*  First or second write toggle (1 bit) */
        uint8_t :4;// 可利用做xxx标志位
    };
    uint8_t oam_addr;                       /*  OAM read/write address */
    uint8_t buffer;                         /*  PPU internal buffer */
    union {
		struct{ // Scroll
			uint16_t coarse_x  : 5;
			uint16_t coarse_y  : 5;
			uint16_t nametable : 2;
			uint16_t fine_y    : 3;
			uint16_t           : 1;
		}v;
        uint16_t v_reg;                     /*  Current VRAM address (15 bits) */
    };
    union {
		struct{ // Scroll
			uint16_t coarse_x  : 5;
			uint16_t coarse_y  : 5;
			uint16_t nametable : 2;
			uint16_t fine_y    : 3;
			uint16_t           : 1;
		}t;
        uint16_t t_reg;                     /*  Temporary VRAM address (15 bits); can also be thought of as the address of the top left onscreen tile. */
    };
    uint8_t ppu_vram[4][NES_PPU_VRAM_SIZE/4];   // 4K: 0x1000 = 0x400 * 4
    union {
        sprite_info_t sprite_info[NES_PPU_OAM_SIZE/4];
        uint8_t oam_data[NES_PPU_OAM_SIZE]; /*  OAM data read/write
                                                The OAM (Object Attribute Memory) is internal memory inside the PPU that contains a display list of up to 64 sprites,
                                                where each sprite's information occupies 4 bytes.*/
    };

    uint8_t palette_indexes[0x20];          /*  $3F00-$3F1F Palette RAM indexes */
    union {
        struct {
            nes_color_t background_palette[0x10];
            nes_color_t sprite_palette[0x10];
        };
        nes_color_t palette[0x20];
    };
    union {
        struct {
            uint8_t* pattern_table[8];
            uint8_t* name_table[4];
            uint8_t* name_table_mirrors[4];
        };
        uint8_t* chr_banks[16];             /*  16k chr_banks,without background_palette and sprite_palette
                                                0 - 3 pattern_table_0 4k
                                                4 - 7 pattern_table_1 4k
                                                8     name_table_0    1k
                                                9     name_table_1    1k
                                                10    name_table_2    1k
                                                11    name_table_3    1k
                                                12-15 mirrors */
    };
} nes_ppu_t;

void nes_ppu_init(nes_t *nes);
void nes_ppu_screen_mirrors(nes_t *nes,nes_mirror_type_t mirror_type);

uint8_t nes_read_ppu_register(nes_t *nes,uint16_t address);
void nes_write_ppu_register(nes_t *nes,uint16_t address, uint8_t data);

#ifdef __cplusplus
    }
#endif
