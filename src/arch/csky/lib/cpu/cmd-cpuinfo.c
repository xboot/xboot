/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <command/command.h>
#include <csky.h>

static int do_cpuinfo(int argc, char ** argv)
{
    uint32_t cpuidrr = mfcr("cr13");
    uint32_t family, model, ic, dc;
    uint32_t foudary, ispm, dspm;
    uint32_t process, rev, ver;

    family  = (cpuidrr & CPUIDRR_FAMILY)    >> 28;
    model   = (cpuidrr & CPUIDRR_MODEL)     >> 24;
    ic      = (cpuidrr & CPUIDRR_IC)        >> 20;
    dc      = (cpuidrr & CPUIDRR_DC)        >> 16;
    foudary = (cpuidrr & CPUIDRR_FOUND)     >> 12;
    ispm    = (cpuidrr & CPUIDRR_ISPM)      >> 11;
    dspm    = (cpuidrr & CPUIDRR_DSPM)      >> 10;
    process = (cpuidrr & CPUIDRR_PROC)      >> 8;
    rev     = (cpuidrr & CPUIDRR_REV)       >> 4;
    ver     = (cpuidrr & CPUIDRR_VER)       >> 0;

    printf("  Family:   ");
    switch (family) {
        case 4:
            printf("CK6XX\r\n");
            break;

        default:
            printf("Unknow\r\n");
    }

    printf("  Model:    ");
    switch (model) {
        case 0:
            printf("CK610\r\n");
            break;

        case 1:
            printf("CK620\r\n");
            break;

        case 2:
            printf("CK660\r\n");
            break;

        case 3:
            printf("CK610e\r\n");
            break;

        default:
            printf("Unknow\r\n");
    }

    printf("  ICache:   ");
    switch (ic) {
        case 0:
            printf("None\r\n");
            break;

        case 1:
            printf("2KiB\r\n");
            break;

        case 2:
            printf("4KiB\r\n");
            break;

        case 3:
            printf("8KiB\r\n");
            break;

        default:
            printf("Unknow\r\n");
    }

    printf("  DCache:   ");
    switch (dc) {
        case 0:
            printf("None\r\n");
            break;

        case 1:
            printf("2KiB\r\n");
            break;

        case 2:
            printf("4KiB\r\n");
            break;

        case 3:
            printf("8KiB\r\n");
            break;

        default:
            printf("Unknow\r\n");
    }

    printf("  Foudary:  ");
    switch (foudary) {
        case 0:
            printf("TSMC\r\n");
            break;

        case 1:
            printf("SMIC\r\n");
            break;

        case 2:
            printf("Hejian\r\n");
            break;

        case 3:
            printf("HHNEC\r\n");
            break;

        default:
            printf("Unknow\r\n");
    }

    printf("  ISPM:     ");
    switch (ispm) {
        case 0:
            printf("Not Support\r\n");
            break;

        case 1:
            printf("Support\r\n");
            break;

        default:
            printf("Unknow\r\n");
    }

    printf("  DSPM:     ");
    switch (dspm) {
        case 0:
            printf("Not Support\r\n");
            break;

        case 1:
            printf("Support\r\n");
            break;

        default:
            printf("Unknow\r\n");
    }

    printf("  Process:  ");
    switch (process) {
        case 0:
            printf("0.18um\r\n");
            break;

        case 1:
            printf("0.13um\r\n");
            break;

        default:
            printf("Unknow\r\n");
    }

    printf("  Revision: %u\r\n", rev);
    printf("  Version:  %u\r\n", ver);

    return 0;
}

static void usage(void)
{
    printf("usage:\r\n");
    printf("    cpuinfo\r\n");
}

static struct command_t cmd_cpuinfo = {
    .name = "cpuinfo",
    .desc = "show information about CPU",
    .usage = usage,
    .exec = do_cpuinfo,
};

static __init void cpuinfo_cmd_init(void)
{
    register_command(&cmd_cpuinfo);
}

static __exit void cpuinfo_cmd_exit(void)
{
    unregister_command(&cmd_cpuinfo);
}

command_initcall(cpuinfo_cmd_init);
command_exitcall(cpuinfo_cmd_exit);