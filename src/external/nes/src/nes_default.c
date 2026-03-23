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

#include "nes_default.h"

#ifndef _MSC_VER
#include <stdlib.h>


/* memory */
NES_WEAK void *nes_malloc(int num){
    return malloc(num);
}

NES_WEAK void nes_free(void *address){
    free(address);
}

NES_WEAK void *nes_memcpy(void *str1, const void *str2, size_t n){
    return memcpy(str1, str2, n);
}

NES_WEAK void *nes_memset(void *str, int c, size_t n){
    return memset(str,c,n);
}

NES_WEAK int nes_memcmp(const void *str1, const void *str2, size_t n){
    return memcmp(str1,str2,n);
}

#if (NES_USE_FS == 1)
/* io */
NES_WEAK FILE *nes_fopen(const char * filename, const char * mode ){
    return fopen(filename,mode);
}

NES_WEAK size_t nes_fread(void *ptr, size_t size, size_t nmemb, FILE *stream){
    return fread(ptr, size, nmemb,stream);
}

NES_WEAK size_t nes_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
    return fwrite(ptr, size, nmemb,stream);
}

NES_WEAK int nes_fseek(FILE *stream, long int offset, int whence){
    return fseek(stream,offset,whence);
}

NES_WEAK int nes_fclose(FILE *stream ){
    return fclose(stream);
}
#endif

#endif
