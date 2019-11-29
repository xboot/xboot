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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <xboot.h>
#include <stdint.h>

double gettime(void);
double fmin(double, double);

void aligned_block_copy(int64_t * __restrict dst,
                        int64_t * __restrict src,
                        int                  size);

void aligned_block_copy_backwards(int64_t * __restrict dst,
                                  int64_t * __restrict src,
                                  int                  size);

void aligned_block_copy_pf32(int64_t * __restrict dst,
                             int64_t * __restrict src,
                             int                  size);
void aligned_block_copy_pf64(int64_t * __restrict dst,
                             int64_t * __restrict src,
                             int                  size);

void aligned_block_fill(int64_t * __restrict dst,
                        int64_t * __restrict src,
                        int                  size);

void *alloc_four_nonaliased_buffers(void **buf1, int size1,
                                    void **buf2, int size2,
                                    void **buf3, int size3,
                                    void **buf4, int size4);

#endif
