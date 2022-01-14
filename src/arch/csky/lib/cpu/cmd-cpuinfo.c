/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <command/command.h>
#include <csky.h>

static int do_cpuinfo(int argc, char ** argv)
{
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