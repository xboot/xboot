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


int main(int argc, char** argv){
    nes_t* nes = nes_init();
    if (argc == 2){
        const char* nes_file_path = argv[1];
        size_t nes_file_path_len = strlen(nes_file_path);
        if (nes_memcmp(nes_file_path+nes_file_path_len-4,".nes",4)==0 || nes_memcmp(nes_file_path+nes_file_path_len-4,".NES",4)==0){
            NES_LOG_INFO("nes_file_path:%s\n",nes_file_path);
            int ret = nes_load_file(nes, nes_file_path);
            if (ret){
                NES_LOG_ERROR("nes load file fail\n");
                goto error;
            }
            nes_run(nes);
            nes_unload_file(nes);
            nes_deinit(nes);
            return 0;
        }else{
            NES_LOG_ERROR("Please enter xxx.nes\n");
            goto error;
        }
    }else{
        NES_LOG_ERROR("Please enter the nes file path\n");
        goto error;
    }
error:
    nes_deinit(nes);
    getchar();
    return -1;
}
