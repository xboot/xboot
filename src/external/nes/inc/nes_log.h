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

#include "nes_default.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*
 * The color for terminal (foreground)
 * BLACK    30
 * RED      31
 * GREEN    32
 * YELLOW   33
 * BLUE     34
 * PURPLE   35
 * CYAN     36
 * WHITE    37
 */

#define NES_LOG_LEVEL_NONE      0    /* Do not log anything. */
#define NES_LOG_LEVEL_ERROR     1    /* Log error. */
#define NES_LOG_LEVEL_WARN      2    /* Log warning. */
#define NES_LOG_LEVEL_INFO      3    /* Log infomation. */
#define NES_LOG_LEVEL_DEBUG     4    /* Log debug. */

#ifndef NES_LOG_LEVEL
#  define NES_LOG_LEVEL        NES_LOG_LEVEL_INFO
#endif

#  if NES_LOG_LEVEL >= NES_LOG_LEVEL_ERROR
#    define NES_LOG_ERROR(format, ...) nes_log_printf("\033[31m[ERROR][%s:%d(%s)]: \033[0m" format, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#  else
#    define NES_LOG_ERROR(format, ...) do {}while(0)
#  endif

#  if NES_LOG_LEVEL >= NES_LOG_LEVEL_WARN
#    define NES_LOG_WARN(format, ...) nes_log_printf("\033[33m[WARN][%s:%d(%s)]: \033[0m" format, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#  else
#    define NES_LOG_WARN(format, ...) do {}while(0)
#  endif

#  if NES_LOG_LEVEL >= NES_LOG_LEVEL_INFO
#    define NES_LOG_INFO(format, ...) nes_log_printf("\033[32m[INFO][%s:%d(%s)]: \033[0m" format, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#  else
#    define NES_LOG_INFO(format, ...) do {}while(0)
#  endif

#  if NES_LOG_LEVEL >= NES_LOG_LEVEL_DEBUG
#    define NES_LOG_DEBUG(format, ...) nes_log_printf("\033[0m[DEBUG][%s:%d(%s)]: \033[0m" format, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#  else
#    define NES_LOG_DEBUG(format, ...) do {}while(0)
#  endif

#ifdef __cplusplus
    }
#endif
