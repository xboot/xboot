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

#define NES_ENABLE_SOUND        (1)       /* enable sound */
#define NES_USE_SRAM            (0)       /* use SRAM */

#define NES_FRAME_SKIP          (0)       /* skip frames */
/* Color depth:
 * - 16: RGB565
 * - 32: ARGB8888
 */
#define NES_COLOR_DEPTH         (32)      /* color depth */
#define NES_COLOR_SWAP          (0)       /* swap color channels */
#define NES_RAM_LACK            (0)       /* lack of RAM */

#define NES_USE_FS              (1)       /* use file system */
/*
*  - NES_LOG_LEVEL_NONE     Do not log anything.
*  - NES_LOG_LEVEL_ERROR    Log error.
*  - NES_LOG_LEVEL_WARN     Log warning.
*  - NES_LOG_LEVEL_INFO     Log infomation.
*  - NES_LOG_LEVEL_DEBUG    Log debug.
*/
#define NES_LOG_LEVEL NES_LOG_LEVEL_INFO

/* log */
#define nes_log_printf(format,...)  printf(format, ##__VA_ARGS__)

#ifdef __cplusplus
    }
#endif
