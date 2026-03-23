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

#if (NES_USE_FS == 1)
int nes_load_file(nes_t* nes, const char* file_path ){
    nes_header_ines_t nes_header_info = {0};

    void* nes_file = nes_fopen(file_path, "rb");
    if (nes_file == NULL){
        NES_LOG_ERROR("nes_load_file: failed to open file %s\n", file_path);
        goto error;
    }
#if (NES_USE_SRAM == 1)
    nes->nes_rom.sram = (uint8_t*)nes_malloc(SRAM_SIZE);
    if (nes->nes_rom.sram == NULL) {
        goto error;
    }
    nes_memset(nes->nes_rom.sram, 0x00, SRAM_SIZE);
#endif
    if (nes_fread(&nes_header_info, sizeof(nes_header_info), 1, nes_file)) {
        if (nes_memcmp(nes_header_info.identification, "NES\x1a", 4)){
            goto error;
        }
        if (nes_header_info.trainer){
#if (NES_USE_SRAM == 1)
            if (nes_fread(nes->nes_rom.sram, TRAINER_SIZE, 1, nes_file)==0){
                goto error;
            }
#else
            nes_fseek(nes_file, TRAINER_SIZE, SEEK_CUR);
#endif
        }
        if (nes_header_info.identifier==2){ //NES 2.0
            nes_header_nes2_t* nes2_header_info = (nes_header_nes2_t*)&nes_header_info;
            nes->nes_rom.prg_rom_size = ((nes2_header_info->prg_rom_size_m << 8) & 0xF00) | nes2_header_info->prg_rom_size_l;
            nes->nes_rom.chr_rom_size = ((nes2_header_info->chr_rom_size_m << 8) & 0xF00) | nes2_header_info->chr_rom_size_l;
            nes->nes_rom.mapper_number = ((nes2_header_info->mapper_number_h << 8) & 0xF00) | ((nes2_header_info->mapper_number_m << 4) & 0xF0) | (nes2_header_info->mapper_number_l & 0x0F);

        }else{  //INES
            nes_header_ines_t* ines_header_info = (nes_header_ines_t*)&nes_header_info;
            nes->nes_rom.prg_rom_size = ines_header_info->prg_rom_size;
            nes->nes_rom.chr_rom_size = ines_header_info->chr_rom_size;
            nes->nes_rom.mapper_number = ines_header_info->mapper_number_l | ines_header_info->mapper_number_h << 4;
        }
        nes->nes_rom.mirroring_type = nes_header_info.mirroring;
        nes->nes_rom.four_screen = nes_header_info.four_screen;
        nes->nes_rom.save_ram = nes_header_info.save;
        nes->nes_rom.prg_rom = (uint8_t*)nes_malloc(PRG_ROM_UNIT_SIZE * nes->nes_rom.prg_rom_size);
        if (nes->nes_rom.prg_rom == NULL) {
            goto error;
        }
        if (nes_fread(nes->nes_rom.prg_rom, PRG_ROM_UNIT_SIZE, nes->nes_rom.prg_rom_size, nes_file)==0){
            goto error;
        }
        nes->nes_rom.chr_rom = (uint8_t*)nes_malloc(CHR_ROM_UNIT_SIZE * (nes->nes_rom.chr_rom_size ? nes->nes_rom.chr_rom_size : 1));
        if (nes->nes_rom.chr_rom == NULL) {
            goto error;
        }
        if (nes->nes_rom.chr_rom_size){
            if (nes_fread(nes->nes_rom.chr_rom, CHR_ROM_UNIT_SIZE, (nes->nes_rom.chr_rom_size), nes_file)==0){
                goto error;
            }
        }
    }else{
        goto error;
    }
    nes_fclose(nes_file);
    nes_cpu_init(nes);
#if (NES_ENABLE_SOUND==1)
    nes_apu_init(nes);
#endif
    nes_ppu_init(nes);
    if(nes_load_mapper(nes)){
        goto error;
    }
    nes->nes_mapper.mapper_init(nes);
    return NES_OK;
error:
    if (nes_file){
        nes_fclose(nes_file);
    }
    if (nes){
        nes_unload_file(nes);
    }
    return NES_ERROR;

}


int nes_unload_file(nes_t* nes){
    if (nes->nes_rom.prg_rom){
        nes_free(nes->nes_rom.prg_rom);
    }
    if (nes->nes_rom.chr_rom){
        nes_free(nes->nes_rom.chr_rom);
    }
    if (nes->nes_rom.sram){
        nes_free(nes->nes_rom.sram);
    }
    return NES_OK;
}

#endif

int nes_load_rom(nes_t* nes, const uint8_t* nes_rom){
    nes_header_ines_t* nes_header_info = (nes_header_ines_t*)nes_rom;
#if (NES_USE_SRAM == 1)
    nes->nes_rom.sram = (uint8_t*)nes_malloc(SRAM_SIZE);
#endif
    if ( nes_memcmp( nes_header_info->identification, "NES\x1a", 4 )){
        goto error;
    }
    uint8_t* nes_bin = (uint8_t*)nes_rom + sizeof(nes_header_ines_t);
    if (nes_header_info->trainer){
#if (NES_USE_SRAM == 1)
#else
#endif
        nes_bin += TRAINER_SIZE;
    }

    if (nes_header_info->identifier==2){ //NES 2.0
        nes_header_nes2_t* nes2_header_info = (nes_header_nes2_t*)nes_header_info;
        nes->nes_rom.prg_rom_size = ((nes2_header_info->prg_rom_size_m << 8) & 0xF00) | nes2_header_info->prg_rom_size_l;
        nes->nes_rom.chr_rom_size = ((nes2_header_info->chr_rom_size_m << 8) & 0xF00) | nes2_header_info->chr_rom_size_l;
        nes->nes_rom.mapper_number = ((nes2_header_info->mapper_number_h << 8) & 0xF00) | ((nes2_header_info->mapper_number_m << 4) & 0xF0) | (nes2_header_info->mapper_number_l & 0x0F);

    }else{  //INES
        nes_header_ines_t* ines_header_info = (nes_header_ines_t*)nes_header_info;
        nes->nes_rom.prg_rom_size = ines_header_info->prg_rom_size;
        nes->nes_rom.chr_rom_size = ines_header_info->chr_rom_size;
        nes->nes_rom.mapper_number = ines_header_info->mapper_number_l | ines_header_info->mapper_number_h << 4;
    }

    nes->nes_rom.mirroring_type = (nes_header_info->mirroring);
    nes->nes_rom.four_screen = (nes_header_info->four_screen);
    nes->nes_rom.save_ram = (nes_header_info->save);

    nes->nes_rom.prg_rom = nes_bin;
    nes_bin += PRG_ROM_UNIT_SIZE * nes->nes_rom.prg_rom_size;

    if (nes->nes_rom.chr_rom_size){
        nes->nes_rom.chr_rom = nes_bin;
    }
    nes_cpu_init(nes);
#if (NES_ENABLE_SOUND==1)
    nes_apu_init(nes);
#endif
    nes_ppu_init(nes);
    if(nes_load_mapper(nes)){
        return NES_ERROR;
    }
    nes->nes_mapper.mapper_init(nes);
    return NES_OK;
error:
    if (nes){
        nes_unload_rom(nes);
    }
    return NES_ERROR;
}

int nes_unload_rom(nes_t* nes){
    (void)nes;
    return NES_OK;
}
