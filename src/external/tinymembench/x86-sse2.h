/*
 * Copyright © 2011 Siarhei Siamashka <siarhei.siamashka@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __X86_SSE2_H__
#define __X86_SSE2_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void aligned_block_copy_sse2(int64_t * __restrict dst,
                             int64_t * __restrict src,
                             int                  size);
void aligned_block_copy_nt_sse2(int64_t * __restrict dst,
                                int64_t * __restrict src,
                                int                  size);

void aligned_block_copy_pf32_sse2(int64_t * __restrict dst,
                                  int64_t * __restrict src,
                                  int                  size);
void aligned_block_copy_pf64_sse2(int64_t * __restrict dst,
                                  int64_t * __restrict src,
                                  int                  size);

void aligned_block_copy_nt_pf32_sse2(int64_t * __restrict dst,
                                     int64_t * __restrict src,
                                     int                  size);
void aligned_block_copy_nt_pf64_sse2(int64_t * __restrict dst,
                                     int64_t * __restrict src,
                                     int                  size);

void aligned_block_fill_sse2(int64_t * __restrict dst,
                             int64_t * __restrict src,
                             int                  size);

void aligned_block_fill_nt_sse2(int64_t * __restrict dst,
                                int64_t * __restrict src,
                                int                  size);

#ifdef __cplusplus
}
#endif

#endif
