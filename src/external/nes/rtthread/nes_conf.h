/*
 * Copyright 2023-2025 Dozingfiretruck
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
#include <rtthread.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef PKG_NES_ENABLE_SOUND
#define NES_ENABLE_SOUND        (1)       /* enable sound */
#endif

#define NES_FRAME_SKIP          PKG_NES_FRAME_SKIP       /* skip frames */

#ifdef PKG_NES_USE_FS
#define NES_USE_FS              (1)       /* use file system */
#endif

/* Color depth:
 * - 16: RGB565
 * - 32: ARGB8888
 */
#ifdef PKG_NES_COLOR_DEPTH_32
#define NES_COLOR_DEPTH         (32)      /* color depth */
#else
#define NES_COLOR_DEPTH         (16)      /* color depth */
#endif

#ifdef PKG_NES_COLOR_SWAP
#define NES_COLOR_SWAP          (1)       /* swap color channels */
#endif

#define NES_RAM_LACK            (0)       /* lack of RAM */

/*
*  - NES_LOG_LEVEL_NONE     Do not log anything.
*  - NES_LOG_LEVEL_ERROR    Log error.
*  - NES_LOG_LEVEL_WARN     Log warning.
*  - NES_LOG_LEVEL_INFO     Log infomation.
*  - NES_LOG_LEVEL_DEBUG    Log debug.
*/
#ifdef PKG_NES_LOG_LEVEL_NONE
#define NES_LOG_LEVEL NES_LOG_LEVEL_NONE
#elif defined(PKG_NES_LOG_LEVEL_ERROR)
#define NES_LOG_LEVEL NES_LOG_LEVEL_ERROR
#elif defined(PKG_NES_LOG_LEVEL_WARN)
#define NES_LOG_LEVEL NES_LOG_LEVEL_WARN
#elif defined(PKG_NES_LOG_LEVEL_INFO)
#define NES_LOG_LEVEL NES_LOG_LEVEL_INFO
#elif defined(PKG_NES_LOG_LEVEL_DEBUG)
#define NES_LOG_LEVEL NES_LOG_LEVEL_DEBUG
#endif

/* log */
#define nes_log_printf(format,...)  rt_kprintf(format, ##__VA_ARGS__)

#ifdef __cplusplus
    }
#endif
