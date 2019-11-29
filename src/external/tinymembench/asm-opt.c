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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "asm-opt.h"

#if defined(__linux__) || defined(ANDROID) || defined(__ANDROID__)

#define SOMEWHAT_SANE_PROC_CPUINFO_SIZE_LIMIT (1024 * 1024)

#if defined(__i386__) || defined(__amd64__)
#define FEATURES_ID "flags"
#elif defined(__arm__)
#define FEATURES_ID "Features"
#elif defined(__mips__)
#define FEATURES_ID "cpu model"
#else
#define FEATURES_ID "?"
#endif

static int check_feature (char *buffer, const char *feature)
{
  char *p;
  if (*feature == 0)
    return 0;
  if (strncmp(buffer, FEATURES_ID, strlen(FEATURES_ID)) != 0)
    return 0;
  buffer += strlen(FEATURES_ID);
  while (isspace(*buffer))
    buffer++;

  /* Check if 'feature' is present in the buffer as a separate word */
  while ((p = strstr(buffer, feature))) {
    if (p > buffer && !isspace(*(p - 1))) {
      buffer++;
      continue;
    }
    p += strlen(feature);
    if (*p != 0 && !isspace(*p)) {
      buffer++;
      continue;
    }
    return 1;
  }
  return 0;
}

static int parse_proc_cpuinfo(int bufsize, const char *feature)
{
  char *buffer = (char *)malloc(bufsize);
  FILE *fd;
  int feature_support = 0;

  if (!buffer)
    return 0;

  fd = fopen("/proc/cpuinfo", "r");
  if (fd) {
    while (fgets(buffer, bufsize, fd)) {
      if (!strchr(buffer, '\n') && !feof(fd)) {
        /* "impossible" happened - insufficient size of the buffer! */
        fclose(fd);
        free(buffer);
        return -1;
      }
      if (check_feature(buffer, feature))
        feature_support = 1;
    }
    fclose(fd);
  }
  free(buffer);
  return feature_support;
}

#define SOMEWHAT_SANE_PROC_CPUINFO_SIZE_LIMIT (1024 * 1024)

int check_cpu_feature(const char *feature)
{
  int bufsize = 1024;
  int result;
  while ((result = parse_proc_cpuinfo(bufsize, feature)) == -1)
  {
    bufsize *= 2;
    if (bufsize > SOMEWHAT_SANE_PROC_CPUINFO_SIZE_LIMIT)
      return 0;
  }
  return result;
}

#else

int check_cpu_feature(const char *feature)
{
    return 0;
}

#endif

static bench_info empty[] = { { NULL, 0, NULL } };

#if defined(__i386__) || defined(__amd64__)

#include "x86-sse2.h"

static bench_info x86_sse2[] =
{
    { "SSE2 copy", 0, aligned_block_copy_sse2 },
    { "SSE2 nontemporal copy", 0, aligned_block_copy_nt_sse2 },
    { "SSE2 copy prefetched (32 bytes step)", 0, aligned_block_copy_pf32_sse2 },
    { "SSE2 copy prefetched (64 bytes step)", 0, aligned_block_copy_pf64_sse2 },
    { "SSE2 nontemporal copy prefetched (32 bytes step)", 0, aligned_block_copy_nt_pf32_sse2 },
    { "SSE2 nontemporal copy prefetched (64 bytes step)", 0, aligned_block_copy_nt_pf64_sse2 },
    { "SSE2 2-pass copy", 1, aligned_block_copy_sse2 },
    { "SSE2 2-pass copy prefetched (32 bytes step)", 1, aligned_block_copy_pf32_sse2 },
    { "SSE2 2-pass copy prefetched (64 bytes step)", 1, aligned_block_copy_pf64_sse2 },
    { "SSE2 2-pass nontemporal copy", 1, aligned_block_copy_nt_sse2 },
    { "SSE2 fill", 0, aligned_block_fill_sse2 },
    { "SSE2 nontemporal fill", 0, aligned_block_fill_nt_sse2 },
    { NULL, 0, NULL }
};

static int check_sse2_support(void)
{
#ifdef __amd64__
    return 1; /* We assume that all 64-bit processors have SSE2 support */
#else
    int cpuid_feature_information;
    __asm__ volatile (
        /* According to Intel manual, CPUID instruction is supported
         * if the value of ID bit (bit 21) in EFLAGS can be modified */
        "pushf\n"
        "movl     (%%esp),   %0\n"
        "xorl     $0x200000, (%%esp)\n" /* try to modify ID bit */
        "popf\n"
        "pushf\n"
        "xorl     (%%esp),   %0\n"      /* check if ID bit changed */
        "jz       1f\n"
        "push     %%eax\n"
        "push     %%ebx\n"
        "push     %%ecx\n"
        "mov      $1,        %%eax\n"
        "cpuid\n"
        "pop      %%ecx\n"
        "pop      %%ebx\n"
        "pop      %%eax\n"
        "1:\n"
        "popf\n"
        : "=d" (cpuid_feature_information)
        :
        : "cc");
    return cpuid_feature_information & (1 << 26);
#endif
}

bench_info *get_asm_benchmarks(void)
{
    if (check_sse2_support())
        return x86_sse2;
    else
        return empty;
}

#elif defined(__arm__)

#include "arm-neon.h"

static bench_info arm_neon[] =
{
    { "NEON read", 0, aligned_block_read_neon },
    { "NEON read prefetched (32 bytes step)", 0, aligned_block_read_pf32_neon },
    { "NEON read prefetched (64 bytes step)", 0, aligned_block_read_pf64_neon },
    { "NEON read 2 data streams", 0, aligned_block_read2_neon },
    { "NEON read 2 data streams prefetched (32 bytes step)", 0, aligned_block_read2_pf32_neon },
    { "NEON read 2 data streams prefetched (64 bytes step)", 0, aligned_block_read2_pf64_neon },
    { "NEON copy", 0, aligned_block_copy_neon },
    { "NEON copy prefetched (32 bytes step)", 0, aligned_block_copy_pf32_neon },
    { "NEON copy prefetched (64 bytes step)", 0, aligned_block_copy_pf64_neon },
    { "NEON unrolled copy", 0, aligned_block_copy_unrolled_neon },
    { "NEON unrolled copy prefetched (32 bytes step)", 0, aligned_block_copy_unrolled_pf32_neon },
    { "NEON unrolled copy prefetched (64 bytes step)", 0, aligned_block_copy_unrolled_pf64_neon },
    { "NEON copy backwards", 0, aligned_block_copy_backwards_neon },
    { "NEON copy backwards prefetched (32 bytes step)", 0, aligned_block_copy_backwards_pf32_neon },
    { "NEON copy backwards prefetched (64 bytes step)", 0, aligned_block_copy_backwards_pf64_neon },
    { "NEON 2-pass copy", 1, aligned_block_copy_neon },
    { "NEON 2-pass copy prefetched (32 bytes step)", 1, aligned_block_copy_pf32_neon },
    { "NEON 2-pass copy prefetched (64 bytes step)", 1, aligned_block_copy_pf64_neon },
    { "NEON unrolled 2-pass copy", 1, aligned_block_copy_unrolled_neon },
    { "NEON unrolled 2-pass copy prefetched (32 bytes step)", 1, aligned_block_copy_unrolled_pf32_neon },
    { "NEON unrolled 2-pass copy prefetched (64 bytes step)", 1, aligned_block_copy_unrolled_pf64_neon },
    { "NEON fill", 0, aligned_block_fill_neon },
    { "NEON fill backwards", 0, aligned_block_fill_backwards_neon },
    { "ARM fill (STRD)", 0, aligned_block_fill_strd_armv5te },
    { "ARM fill (STM with 8 registers)", 0, aligned_block_fill_stm8_armv4 },
    { "ARM fill (STM with 4 registers)", 0, aligned_block_fill_stm4_armv4 },
    { "ARM copy prefetched (incr pld)", 0, aligned_block_copy_incr_armv5te },
    { "ARM copy prefetched (wrap pld)", 0, aligned_block_copy_wrap_armv5te },
    { "ARM 2-pass copy prefetched (incr pld)", 1, aligned_block_copy_incr_armv5te },
    { "ARM 2-pass copy prefetched (wrap pld)", 1, aligned_block_copy_wrap_armv5te },
    { NULL, 0, NULL }
};

static bench_info arm_v5te[] =
{
    { "ARM fill (STRD)", 0, aligned_block_fill_strd_armv5te },
    { "ARM fill (STM with 8 registers)", 0, aligned_block_fill_stm8_armv4 },
    { "ARM fill (STM with 4 registers)", 0, aligned_block_fill_stm4_armv4 },
    { "ARM copy prefetched (incr pld)", 0, aligned_block_copy_incr_armv5te },
    { "ARM copy prefetched (wrap pld)", 0, aligned_block_copy_wrap_armv5te },
    { "ARM 2-pass copy prefetched (incr pld)", 1, aligned_block_copy_incr_armv5te },
    { "ARM 2-pass copy prefetched (wrap pld)", 1, aligned_block_copy_wrap_armv5te },
    { NULL, 0, NULL }
};

static bench_info arm_v4[] =
{
    { "ARM fill (STM with 8 registers)", 0, aligned_block_fill_stm8_armv4 },
    { "ARM fill (STM with 4 registers)", 0, aligned_block_fill_stm4_armv4 },
    { NULL, 0, NULL }
};

bench_info *get_asm_benchmarks(void)
{
	return arm_neon;

    if (check_cpu_feature("neon"))
        return arm_neon;
    else if (check_cpu_feature("edsp"))
        return arm_v5te;
    else
        return arm_v4;
}

#elif defined(__mips__) && defined(_ABIO32)

#include "mips-32.h"

static bench_info mips_32[] =
{
    { "MIPS32 copy prefetched (32 bytes step)", 0, aligned_block_copy_pf32_mips32 },
    { "MIPS32 2-pass copy prefetched (32 bytes step)", 1, aligned_block_copy_pf32_mips32 },
    { "MIPS32 fill prefetched (32 bytes step)", 0, aligned_block_fill_pf32_mips32 },
    { NULL, 0, NULL }
};

bench_info *get_asm_benchmarks(void)
{
    /* Enable only for MIPS32 processors which have 32 bytes cache line */
    if (check_cpu_feature("MIPS 24K") ||
        check_cpu_feature("MIPS 24Kc") ||
        check_cpu_feature("MIPS 24Kf") ||
        check_cpu_feature("MIPS 74K") ||
        check_cpu_feature("MIPS 74Kc") ||
        check_cpu_feature("MIPS 74Kf") ||
        check_cpu_feature("MIPS 1004K") ||
        check_cpu_feature("MIPS 1004Kc") ||
        check_cpu_feature("MIPS 1004Kf") ||
        check_cpu_feature("MIPS 1074K") ||
        check_cpu_feature("MIPS 1074Kc") ||
        check_cpu_feature("MIPS 1074Kf"))
    {
        return mips_32;
    }
    else
    {
        return empty;
    }
}

#else

bench_info *get_asm_benchmarks(void)
{
    return empty;
}

#endif
