/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <command/command.h>

static int number_table[] = {
    [0] = 8, [1] = 4, [2] = 4, [4] = 2,
};

static int align_table[] = {
    [0] = 3, [1] = 6, [2] = 11, [4] = 16,
};

enum mem_action {
    ACTION_MEM_READ,
    ACTION_MEM_WRITE,
    ACTION_IOPORT_IN,
    ACTION_IOPORT_OUT,
};

enum write_operator {
    OPERATOR_EQUAL  = 1,
    OPERATOR_ADD    = 2,
    OPERATOR_SUB    = 3,
    OPERATOR_MUL    = 4,
    OPERATOR_DIV    = 5,
    OPERATOR_AND    = 6,
    OPERATOR_OR     = 7,
    OPERATOR_XOR    = 8,
    OPERATOR_LEFT   = 9,
    OPERATOR_RIGHT  = 10,
};

enum value_type {
    VALUE_BIT,
    VALUE_BIT_SHIFT,
    VALUE_BIT_RANGE,
};

static enum write_operator get_operator(const char *str)
{
    if (!strcmp(str, "="))
        return OPERATOR_EQUAL;

    else if (!strcmp(str, "+="))
        return OPERATOR_ADD;

    else if (!strcmp(str, "-="))
        return OPERATOR_SUB;

    else if (!strcmp(str, "*="))
        return OPERATOR_MUL;

    else if (!strcmp(str, "/="))
        return OPERATOR_DIV;

    else if (!strcmp(str, "&="))
        return OPERATOR_AND;

    else if (!strcmp(str, "|="))
        return OPERATOR_OR;

    else if (!strcmp(str, "^="))
        return OPERATOR_XOR;

    else if (!strcmp(str, "<<="))
        return OPERATOR_LEFT;

    else if (!strcmp(str, ">>="))
        return OPERATOR_RIGHT;

    return 0;
}

static uint64_t do_operator(enum write_operator oper, uint64_t va, uint64_t vb)
{
    switch (oper) {
        case OPERATOR_EQUAL:
            va = vb;
            break;

        case OPERATOR_ADD:
            va += vb;
            break;

        case OPERATOR_SUB:
            va -= vb;
            break;

        case OPERATOR_MUL:
            va *= vb;
            break;

        case OPERATOR_DIV:
            if (!vb)
                break;
            va /= vb;
            break;

        case OPERATOR_AND:
            va &= vb;
            break;

        case OPERATOR_OR:
            va |= vb;
            break;

        case OPERATOR_XOR:
            va ^= vb;
            break;

        case OPERATOR_LEFT:
            va <<= vb;
            break;

        case OPERATOR_RIGHT:
            va >>= vb;
            break;

        default:
            va = va;
            break;
    }

    return va;
}

static bool_t legal_val(char *str)
{
    if (*str == '~')
        ++str;

    if (isdigit(*str))
        return TRUE;

    if (!strncmp(str, "BIT(", 4))
        return TRUE;

    if (!strncmp(str, "SHIFT(", 6))
        return TRUE;

    if (!strncmp(str, "RANGE(", 6))
        return TRUE;

    return FALSE;
}

static uint64_t convert_val(char *str)
{
    enum value_type type;
    unsigned long long va;
    unsigned long vb;
    char *tmp;
    bool_t reversal = FALSE;

    if (*str == '~') {
        reversal = TRUE;
        ++str;
    }

    if (!strncmp(str, "BIT(", 4)) {
        type = VALUE_BIT;
        str += 4;
        goto skip_va;
    }

    else if (!strncmp(str, "SHIFT(", 6)) {
        type = VALUE_BIT_SHIFT;
        str += 6;
    }

    else if (!strncmp(str, "RANGE(", 6)) {
        type = VALUE_BIT_RANGE;
        str += 6;
    }

    else {
        va = strtoull(str, NULL, 0);
        goto skip_type;
    }

    tmp = strchr(str, ',');
    if (!tmp || tmp == str)
        return 0;
    *tmp = '\0';
    va = strtoull(str, NULL, 0);
    str = tmp + 1;

skip_va:
    tmp = strchr(str, ')');
    if (!tmp || tmp == str)
        return 0;
    *tmp = '\0';
    vb = strtoul(str, NULL, 0);

    switch (type) {
        case VALUE_BIT:
            va = BIT_ULL(vb);
            break;

        case VALUE_BIT_SHIFT:
            va = BIT_SHIFT_ULL(va, vb);
            break;

        case VALUE_BIT_RANGE:
            va = BIT_RANGE_ULL(va, vb);
            break;

        default:
            va = va;
    }

skip_type:
    return reversal ? ~va : va;
}

static void mem_read_usage(void)
{
    printf("usage: mem -r /nfu <addr>               \r\n");
    printf("  /n - length                           \r\n");
    printf("  /o - octal                            \r\n");
    printf("  /x - hexadecimal                      \r\n");
    printf("  /d - decimal                          \r\n");
    printf("  /u - unsigned decimal                 \r\n");
    printf("  /a - address                          \r\n");
    printf("  /c - char                             \r\n");
    printf("  /b - byte (8-bit)                     \r\n");
    printf("  /h - halfword (16-bit)                \r\n");
    printf("  /w - word (32-bit)                    \r\n");
    printf("  /g - giant word (64-bit)              \r\n");
}

static int mem_read(int argc, char *argv[])
{
    char buff[10], cstr[20] = {}, fmt = 'x';
    unsigned int count = 0;
    int num, byte = 4;
    virtual_addr_t addr;
    physical_addr_t phys;

    if (argc == 2 && *argv[0] == '/' && isdigit(*argv[1])) {
        char *para = argv[0] + 1;

        if (!*para)
            goto usage;

        for (; isdigit(*para); ++count) {
            if (count >= sizeof(cstr))
                goto usage;
            cstr[count] = *para++;
        }

        for (; *para; para++) {
            switch (*para) {
                case 'b':
                    byte = 1;
                    break;

                case 'h':
                    byte = 2;
                    break;

                case 'w':
                    byte = 4;
                    break;

                case 'g':
                    byte = 8;
                    break;

                case 'c':
                    byte = 1;
                    strcpy(buff, "'\\%d' '%c'");
                    goto exit;

                case 'a':
                    fmt = 'x';
                    break;

                case 'x': case 'd':
                case 'u': case 'o':
                    fmt = *para;
                    break;

                default:
                    goto usage;
            }
        }
    }

    else if (argc == 1 && isdigit(*argv[0]))
        goto pass;

    else
        goto usage;

pass:
    if (fmt == 'x')
        sprintf(buff, "0x%%0%d%c", byte * 2, fmt);
    else
        sprintf(buff, "%%-%d%c", align_table[byte / 2], fmt);

exit:
    if (count) {
        num = atoi(cstr);
        if (!num)
            goto usage;
    } else
        num = 1;

    phys = strtoul(argv[argc - 1], NULL, 0);
    addr = phys_to_virt(phys);

    for (; (count = min(num, number_table[byte / 2])); num -= count) {
        unsigned int tmp;
        printf("0x%08lx: ", phys);
        for (tmp = 0; tmp < count; ++tmp) {
            switch (byte) {
                case 1:
                    printf(buff, read8(addr), read8(addr));
                    break;
                case 2:
                    printf(buff, read16(addr));
                    break;
                case 4:
                    printf(buff, read32(addr));
                    break;
                case 8: default:
                    printf(buff, read64(addr));
                    break;
            }
            phys += byte;
            addr += byte;
            printf("  ");
        }
        printf("\r\n");
    }

    return -ENOERR;

usage:
    mem_read_usage();
    return -EINVAL;
}

static void mem_write_usage(void)
{
    printf("usage: mem -w /nu <addr> <operator> <value>     \r\n");
    printf("  /n - length                                   \r\n");
    printf("  /b - byte (8-bit)                             \r\n");
    printf("  /h - halfword (16-bit)                        \r\n");
    printf("  /w - word (32-bit)                            \r\n");
    printf("  /g - giant word (64-bit)                      \r\n");
}

static int mem_write(int argc, char *argv[])
{
    char cstr[20] = {};
    unsigned int count = 0;
    unsigned long long value, tmp;
    int num, byte = 4;
    enum write_operator oper;
    virtual_addr_t addr;
    physical_addr_t phys;

    if (argc == 4 && *argv[0] == '/' && isdigit(*argv[1]) && legal_val(argv[3])) {
        char *para = argv[0] + 1;

        if (!*para)
            goto usage;

        for (; isdigit(*para); ++count) {
            if (count >= sizeof(cstr))
                goto usage;
            cstr[count] = *para++;
        }

        for (; *para; para++) {
            switch (*para) {
                case 'b':
                    byte = 1;
                    break;

                case 'h':
                    byte = 2;
                    break;

                case 'w':
                    byte = 4;
                    break;

                case 'g':
                    byte = 8;
                    break;

                case 'c': case 'a':
                case 'x': case 'd':
                case 'u': case 'o':
                    break;

                default:
                    goto usage;
            }
        }
    }

    else if (argc == 3 && isdigit(*argv[0]) && legal_val(argv[2]))
        goto pass;

    else
        goto usage;

pass:
    oper = get_operator(argv[argc - 2]);
    if (!oper)
        goto usage;

    if (count) {
        num = atoi(cstr);
        if (!num)
            goto usage;
    } else
        num = 1;

    phys = strtoul(argv[argc - 3], NULL, 0);
    value = convert_val(argv[argc - 1]);
    addr = phys_to_virt(phys);

    while (num--) {
        switch (byte) {
            case 1:
                tmp = read8(addr);
                tmp = do_operator(oper, tmp, (uint8_t)value);
                write8(addr, tmp);
                break;

            case 2:
                tmp = read16(addr);
                tmp = do_operator(oper, tmp, (uint16_t)value);
                write16(addr, tmp);
                break;

            case 4:
                tmp = read32(addr);
                tmp = do_operator(oper, tmp, (uint32_t)value);
                write32(addr, tmp);
                break;

            case 8: default:
                tmp = read64(addr);
                tmp = do_operator(oper, tmp, (uint64_t)value);
                write64(addr, tmp);
                break;
        }
        addr += byte;
    }

    return -ENOERR;

usage:
    mem_write_usage();
    return -EINVAL;
}

static void usage(void)
{
    printf("usage: mem [option] ...                 \r\n");
    printf("  -r   memory read (default)            \r\n");
    printf("  -w   memory write                     \r\n");
    printf("  -s   show after modification          \r\n");
    printf("  -h   display this message             \r\n");
}

static int mem_main(int argc, char *argv[])
{
    unsigned int act = 0;
    bool_t show = FALSE;
    int ret;

    if (argc < 2)
        goto usage;

    else if (*argv[1] == '/' || isdigit(*argv[1]))
        return mem_read(argc - 1, &argv[1]);

    else if (*argv[1] == '-') {
        char *para = argv[1] + 1;

        if (!*para)
            goto usage;

        for (; *para; para++) {
            switch (*para) {
                case 'r':
                    act = ACTION_MEM_READ;
                    break;

                case 'w':
                    act = ACTION_MEM_WRITE;
                    break;

                case 's':
                    show = TRUE;
                    break;

                case 'h': default:
                    goto usage;
            }
        }
    }

    else
        goto usage;

    switch (act) {
        case ACTION_MEM_WRITE:
            ret = mem_write(argc - 2, &argv[2]);
            if (ret || !show)
                return ret;
            show = FALSE;
            argc -= 2;

        case ACTION_MEM_READ: default:
            if (show)
                goto usage;
            return mem_read(argc - 2, &argv[2]);
    }

usage:
    usage();
    return -EINVAL;
}

static struct command_t cmd_mem = {
    .name = "mem",
    .desc = "memory toolset",
    .exec = mem_main,
    .usage = usage,
};

static __init void mem_cmd_init(void)
{
    register_command(&cmd_mem);
}

static __exit void mem_cmd_exit(void)
{
    unregister_command(&cmd_mem);
}

command_initcall(mem_cmd_init);
command_exitcall(mem_cmd_exit);
