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
#include <xboot.h>
#include <stdarg.h>

/* memory */
void *nes_malloc(int num){
    return malloc(num);
}

void nes_free(void *address){
    free(address);
}

void *nes_memcpy(void *str1, const void *str2, size_t n){
    return memcpy(str1, str2, n);
}

void *nes_memset(void *str, int c, size_t n){
    return memset(str,c,n);
}

int nes_memcmp(const void *str1, const void *str2, size_t n){
    return memcmp(str1,str2,n);
}

#if (NES_USE_FS == 1)
/* io */
FILE *nes_fopen(const char * filename, const char * mode ){
    return fopen(filename,mode);
}

size_t nes_fread(void *ptr, size_t size, size_t nmemb, FILE *stream){
    return fread(ptr, size, nmemb,stream);
}

size_t nes_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
    return fwrite(ptr, size, nmemb,stream);
}

int nes_fseek(FILE *stream, long int offset, int whence){
    return fseek(stream,offset,whence);
}

int nes_fclose(FILE *stream ){
    return fclose(stream);
}
#endif


#if (NES_ENABLE_SOUND == 1)

int nes_sound_output(uint8_t *buffer, size_t len){
    return 0;
}
#endif

int nes_initex(nes_t *nes){
    return 0;
}

int nes_deinitex(nes_t *nes){
    return 0;
}



#define FRAMES_PER_SECOND   1000/60

void nes_frame(nes_t* nes){
}
