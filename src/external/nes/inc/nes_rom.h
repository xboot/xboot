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

#define SRAM_SIZE               (0x2000)      /* 8K */

#define TRAINER_SIZE            (0x200)     /* 512 */
#define PRG_ROM_UNIT_SIZE       (0x4000)    /* 16K */
#define CHR_ROM_UNIT_SIZE       (0x2000)    /* 8K */

struct nes;
typedef struct nes nes_t;

/* INES:    https://www.nesdev.org/wiki/INES */
typedef struct {
    uint8_t identification[4];          /*  0-3   Constant $4E $45 $53 $1A (ASCII "NES" followed by MS-DOS end-of-file) */
    uint8_t prg_rom_size;               /*  4     Size of PRG ROM in 16 KB units */
    uint8_t chr_rom_size;               /*  5     Size of CHR ROM in 8 KB units (value 0 means the board uses CHR RAM) */
    struct {
        uint8_t mirroring:1;            /*  D0    Nametable arrangement: 0: vertical arrangement ("horizontal mirrored") (CIRAM A10 = PPU A11)
                                                                         1: horizontal arrangement ("vertically mirrored") (CIRAM A10 = PPU A10) */
        uint8_t save:1;                 /*  D1    Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory */
        uint8_t trainer:1;              /*  D2    512-byte trainer at $7000-$71FF (stored before PRG data) */
        uint8_t four_screen:1;          /*  D3    Alternative nametable layout */
        uint8_t mapper_number_l:4;      /*  D4-7  Lower nybble of mapper number */
    };
    struct {
        uint8_t unisystem:1;            /*  D0    VS Unisystem */
        uint8_t playchoice_10:1;        /*  D1    PlayChoice-10 (8 KB of Hint Screen data stored after CHR data) */
        uint8_t identifier:2;           /*  D2-3  If equal to 2, flags 8-15 are in NES 2.0 format */
        uint8_t mapper_number_h:4;      /*  D4-7  Upper nybble of mapper number */
    };
    uint8_t prg_ram_size;               /*  PRG RAM size */
    struct {
        uint8_t tv_system:1;            /*  D0    TV system (0: NTSC; 1: PAL) */
        uint8_t :7;
    };
    struct {
        uint8_t tv_system_ex:2;         /*  D0-1  TV system (0: NTSC; 2: PAL; 1/3: dual compatible) */
        uint8_t :2;
        uint8_t prg_rom:1;              /*  D4  PRG RAM ($6000-$7FFF) (0: present; 1: not present) */
        uint8_t board_conflicts:1;      /*  D5  0: Board has no bus conflicts; 1: Board has bus conflicts */
        uint8_t :2;
    };
    uint8_t Reserved[5];                /*  11-15 Reserved */
} nes_header_ines_t;

/* NES 2.0: https://wiki.nesdev.org/w/index.php/NES_2.0 */
typedef struct {
    uint8_t identification[4];          /*  0-3   Identification String. Must be "NES<EOF>". */
    uint8_t prg_rom_size_l;             /*  4     PRG-ROM size LSB */
    uint8_t chr_rom_size_l;             /*  5     CHR-ROM size LSB */
    struct {
        uint8_t mirroring:1;            /*  D0    Hard-wired nametable mirroring type        0: Horizontal or mapper-controlled 1: Vertical */
        uint8_t save:1;                 /*  D1    "Battery" and other non-volatile memory    0: Not present 1: Present */
        uint8_t trainer:1;              /*  D2    512-byte Trainer                           0: Not present 1: Present between Header and PRG-ROM data */
        uint8_t four_screen:1;          /*  D3    Hard-wired four-screen mode                0: No 1: Yes */
        uint8_t mapper_number_l:4;      /*  D4-7  Mapper Number D0..D3 */
    };
    struct {
        uint8_t console_type:2;         /*  D0-1  Console type   0: Nintendo Entertainment System/Family Computer 1: Nintendo Vs. System 2: Nintendo Playchoice 10 3: Extended Console Type */
        uint8_t identifier:2;           /*  D2-3  NES 2.0 identifier */
        uint8_t mapper_number_m:4;      /*  D4-7  Mapper Number D4..D7 */
    };
    struct {
        uint8_t mapper_number_h:4;      /*  D0-3  Mapper number D8..D11 */
        uint8_t submapper:4;            /*  D4-7  Submapper number */
    };                                  /*  Mapper MSB/Submapper */
    struct {
        uint8_t prg_rom_size_m:4;       /*  D0-3  PRG-ROM size MSB */
        uint8_t chr_rom_size_m:4;       /*  D4-7  CHR-ROM size MSB */
    };                                  /*  PRG-ROM/CHR-ROM size MSB */
    struct {
        uint8_t prg_ram_size_m:4;       /*  D0-3  PRG-RAM (volatile) shift count */
        uint8_t eeprom_size_m:4;        /*  D4-7  PRG-NVRAM/EEPROM (non-volatile) shift count */
    };                                  /*  PRG-RAM/EEPROM size
                                            If the shift count is zero, there is no PRG-(NV)RAM.
                                            If the shift count is non-zero, the actual size is
                                            "64 << shift count" bytes, i.e. 8192 bytes for a shift count of 7. */
    struct {
        uint8_t chr_ram_size_m:4;       /*  D0-3  CHR-RAM size (volatile) shift count */
        uint8_t chr_nvram_size_m:4;     /*  D4-7  CHR-NVRAM size (non-volatile) shift count */
    };                                  /*  CHR-RAM size
                                            If the shift count is zero, there is no CHR-(NV)RAM.
                                            If the shift count is non-zero, the actual size is
                                            "64 << shift count" bytes, i.e. 8192 bytes for a shift count of 7. */
    struct {
        uint8_t timing_mode :2;         /*  D0-1    CPU/PPU timing mode
                                                    0: RP2C02 ("NTSC NES")
                                                    1: RP2C07 ("Licensed PAL NES")
                                                    2: Multiple-region
                                                    3: UMC 6527P ("Dendy") */
        uint8_t :6;
    };                                  /*  CPU/PPU Timing */
    struct {
        uint8_t ppu_type:4;             /*  D0-3  Vs. PPU Type */
        uint8_t hardware_type:4;        /*  D4-7  Vs. Hardware Type */
    };                                  /*  When Byte 7 AND 3 =1: Vs. System Type
                                            When Byte 7 AND 3 =3: Extended Console Type */
    struct {
        uint8_t miscellaneous_number:2; /*  D0-1  Number of miscellaneous ROMs present */
        uint8_t :6;
    };                                  /*  Miscellaneous ROMs */
    struct {
        uint8_t expansion_device:6;     /*  D0-5  Default Expansion Device */
        uint8_t :2;
    };                                  /*  Default Expansion Device */
} nes_header_nes2_t;

typedef struct nes_rom_info{
    uint16_t prg_rom_size;
    uint16_t chr_rom_size;
    uint8_t* prg_rom;
    uint8_t* chr_rom;
    uint8_t* sram;
    uint16_t mapper_number;             /*  Mapper Number */
    uint8_t  mirroring_type;            /*  0: Horizontal or mapper-controlled 1: Vertical */
    uint8_t  four_screen;               /*  0: No 1: Yes */
    uint8_t  save_ram;                  /*  0: Not present 1: Present */
} nes_rom_info_t;

#ifdef __cplusplus
    }
#endif
