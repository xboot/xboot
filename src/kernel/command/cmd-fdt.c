/*
 * kernel/command/cmd-fdt.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <libfdt.h>
#include <command/command.h>

#define FDT_MAX_LEVEL		(32)
#define FDT_MAX_DUMP		(64)
#define FDT_SCRATCHPAD		(4096)

static struct fdt_header * fdt = NULL;

static void usage(void)
{
	printf("usage:\r\n");
	printf("    fdt addr <addr> [<length>]     - Set the fdt location to <addr>\r\n");
	printf("    fdt resize [<extrasize>]       - Resize fdt and add optional [<extrasize>] and padding\r\n");
	printf("    fdt print <path> [<prop>]      - Recursive print starting at <path>\r\n");
	printf("    fdt list <path> [<prop>]       - Print one level starting at <path>\r\n");
	printf("    fdt header                     - Display header info\r\n");
	printf("    fdt bootcpu <id>               - Set boot cpuid\r\n");
	printf("    fdt set <path> <prop> [<val>]  - Set <property> [to <val>]\r\n");
	printf("    fdt mk <path> <node>           - Create a new node after <path>\r\n");
	printf("    fdt rm <path> [<prop>]         - Delete the node or <property>\r\n");
	printf("    fdt memory <addr> <size> [...] - Add or update memory node\r\n");
	printf("    fdt chosen <bootargs>          - Add or update the /chosen branch in the tree\r\n");
	printf("    fdt initrd [<start> <end>]     - Set initrd start and end address\r\n");
	printf("    fdt rsvmem print               - Show current mem reserves\r\n");
	printf("    fdt rsvmem add <addr> <size>   - Add a mem reserve\r\n");
	printf("    fdt rsvmem delete <index>      - Delete a mem reserves\r\n");
}

static int fdt_shrink_to_minimum(void * blob, int extrasize)
{
	uint64_t addr, size;
	int total, ret, i;
	int actualsize;

	if(!blob)
		return 0;

	total = fdt_num_mem_rsv(blob);
	for(i = 0; i < total; i++)
	{
		fdt_get_mem_rsv(blob, i, &addr, &size);
		if(addr == (uintptr_t)blob)
		{
			fdt_del_mem_rsv(blob, i);
			break;
		}
	}
	actualsize = fdt_off_dt_strings(blob) + fdt_size_dt_strings(blob) + 5 * sizeof(struct fdt_reserve_entry);
	actualsize += extrasize;
	actualsize = (actualsize + ((uintptr_t)blob & 0xfff) + 0x1000) & ~(0x1000 - 1);
	actualsize = actualsize - ((uintptr_t)blob & 0xfff);
	fdt_set_totalsize(blob, actualsize);
	ret = fdt_add_mem_rsv(blob, (uintptr_t)blob, actualsize);
	if(ret < 0)
		return ret;
	return actualsize;
}

static int is_printable_string(const void * data, int len)
{
	const char * s = data;

	if(len == 0)
		return 0;

	if(s[len - 1] != '\0' && s[len - 1] != '\n')
		return 0;

	while(((*s == '\0') || isprint(*s) || isspace(*s)) && (len > 0))
	{
		if(s[0] == '\0')
		{
			if(len == 1)
				return 1;
			if(s[1] == '\0')
				return 0;
		}
		s++;
		len--;
	}

	if(*s != '\0' || (len != 0))
		return 0;
	return 1;
}

static void print_data(const void * data, int len)
{
	int i;

	if(len == 0)
		return;

	if(is_printable_string(data, len))
	{
		printf("\"");
		i = 0;
		while(i < len)
		{
			if(i > 0)
				printf("\", \"");
			printf("%s", data);
			i += strlen(data) + 1;
			data += strlen(data) + 1;
		}
		printf("\"");
		return;
	}

	if((len % 4) == 0)
	{
		if(len > FDT_MAX_DUMP)
			printf("* 0x%p [0x%08x]", data, len);
		else
		{
			const uint32_t * p;
			printf("<");
			for(i = 0, p = data; i < len / 4; i++)
				printf("0x%08x%s", fdt32_to_cpu(p[i]), i < (len/4 - 1) ? " " : "");
			printf(">");
		}
	}
	else
	{
		if(len > FDT_MAX_DUMP)
			printf("* 0x%p [0x%08x]", data, len);
		else
		{
			const unsigned char * s;
			printf("[");
			for(i = 0, s = data; i < len; i++)
				printf("%02x%s", s[i], i < len - 1 ? " " : "");
			printf("]");
		}
	}
}

static int fdt_print(const char * pathp, char * prop, int depth)
{
	static const char tabs[FDT_MAX_LEVEL + 1] =
		"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
		"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	const void * nodep;
	int o;
	int no;
	uint32_t tag;
	int len;
	int level = 0;
	const struct fdt_property * fdt_prop;

	o = fdt_path_offset(fdt, pathp);
	if(o < 0)
	{
		printf("libfdt fdt_path_offset() returned %s\r\n", fdt_strerror(o));
		return 1;
	}

	if(prop)
	{
		nodep = fdt_getprop(fdt, o, prop, &len);
		if(len == 0)
		{
			printf("%s %s\r\n", pathp, prop);
			return 0;
		}
		else if(nodep && len > 0)
		{
			printf("%s = ", prop);
			print_data(nodep, len);
			printf("\r\n");
			return 0;
		}
		else
		{
			printf("libfdt fdt_getprop(): %s\r\n", fdt_strerror(len));
			return 1;
		}
	}

	while(level >= 0)
	{
		tag = fdt_next_tag(fdt, o, &no);
		switch(tag)
		{
		case FDT_BEGIN_NODE:
			pathp = fdt_get_name(fdt, o, NULL);
			if(level <= depth)
			{
				if(pathp == NULL)
					pathp = "/* NULL pointer error */";
				if(*pathp == '\0')
					pathp = "/";
				printf("%s%s {\r\n", &tabs[FDT_MAX_LEVEL - level], pathp);
			}
			level++;
			if(level >= FDT_MAX_LEVEL)
			{
				printf("Nested too deep, aborting.\r\n");
				return 1;
			}
			break;
		case FDT_END_NODE:
			level--;
			if(level <= depth)
				printf("%s};\r\n", &tabs[FDT_MAX_LEVEL - level]);
			if(level == 0)
			{
				level = -1;
			}
			break;
		case FDT_PROP:
			fdt_prop = fdt_offset_ptr(fdt, o, sizeof(*fdt_prop));
			pathp  = fdt_string(fdt, fdt32_to_cpu(fdt_prop->nameoff));
			len = fdt32_to_cpu(fdt_prop->len);
			nodep = fdt_prop->data;
			if(len < 0)
			{
				printf("libfdt fdt_getprop(): %s\r\n", fdt_strerror(len));
				return 1;
			}
			else if(len == 0)
			{
				if(level <= depth)
					printf("%s%s;\r\n", &tabs[FDT_MAX_LEVEL - level], pathp);
			}
			else
			{
				if(level <= depth)
				{
					printf("%s%s = ", &tabs[FDT_MAX_LEVEL - level], pathp);
					print_data(nodep, len);
					printf(";\r\n");
				}
			}
			break;
		case FDT_NOP:
			printf("%s/* NOP */\r\n", &tabs[FDT_MAX_LEVEL - level]);
			break;
		case FDT_END:
			return 1;
		default:
			if(level <= depth)
				printf("Unknown tag 0x%08X\r\n", tag);
			return 1;
		}
		o = no;
	}
	return 0;
}

static int fdt_parse_prop(char * const * newval, int count, char * data, int * len)
{
	char * cp;
	char * newp;
	unsigned long tmp;
	int stridx = 0;

	*len = 0;
	newp = newval[0];

	if(*newp == '<')
	{
		newp++;
		while((*newp != '>') && (stridx < count))
		{
			if(*newp == '\0')
			{
				newp = newval[++stridx];
				continue;
			}

			cp = newp;
			tmp = strtoul(cp, &newp, 0);
			if(*cp != '?')
				*(fdt32_t *)data = cpu_to_fdt32(tmp);
			else
				newp++;

			data += 4;
			*len += 4;

			if((newp - cp) <= 0)
			{
				printf("Sorry, I could not convert \"%s\"\r\n", cp);
				return 1;
			}
			while(*newp == ' ')
				newp++;
		}

		if(*newp != '>')
		{
			printf("Unexpected character '%c'\r\n", *newp);
			return -1;
		}
	}
	else if(*newp == '[')
	{
		newp++;
		while((stridx < count) && (*newp != ']'))
		{
			while(*newp == ' ')
				newp++;
			if(*newp == '\0')
			{
				newp = newval[++stridx];
				continue;
			}
			if(!isxdigit(*newp))
				break;
			tmp = strtoul(newp, &newp, 16);
			*data++ = tmp & 0xFF;
			*len = *len + 1;
		}
		if(*newp != ']')
		{
			printf("Unexpected character '%c'\r\n", *newp);
			return -1;
		}
	}
	else
	{
		while(stridx < count)
		{
			size_t length = strlen(newp) + 1;
			strcpy(data, newp);
			data += length;
			*len += length;
			newp = newval[++stridx];
		}
	}
	return 0;
}

static int fdt_find_or_add_subnode(void * fdt, int parent, const char * name)
{
	int o;

	o = fdt_subnode_offset(fdt, parent, name);
	if(o == -FDT_ERR_NOTFOUND)
		o = fdt_add_subnode(fdt, parent, name);
	return o;
}

static int fdt_pack_reg(const void * fdt, void * buf, uint64_t * address, uint64_t * size, int n)
{
	int i;
	int address_cells = fdt_address_cells(fdt, 0);
	int size_cells = fdt_size_cells(fdt, 0);
	char * p = buf;

	for(i = 0; i < n; i++)
	{
		if(address_cells == 2)
			*(fdt64_t *)p = cpu_to_fdt64(address[i]);
		else
			*(fdt32_t *)p = cpu_to_fdt32(address[i]);
		p += 4 * address_cells;

		if(size_cells == 2)
			*(fdt64_t *)p = cpu_to_fdt64(size[i]);
		else
			*(fdt32_t *)p = cpu_to_fdt32(size[i]);
		p += 4 * size_cells;
	}
	return p - (char *)buf;
}

static int fdt_memory_banks(void * blob, uint64_t * start, uint64_t * size, int banks)
{
	int e, o;
	int len, i;
	uint8_t tmp[256 * 16];

	o = fdt_find_or_add_subnode(blob, 0, "memory");
	if(o < 0)
		return o;

	e = fdt_setprop(blob, o, "device_type", "memory", sizeof("memory"));
	if(e < 0)
	{
		printf("Could not set %s %s.\r\n", "device_type", fdt_strerror(e));
		return -1;
	}

	for(i = 0; i < banks; i++)
	{
		if(start[i] == 0 && size[i] == 0)
			break;
	}
	banks = i;
	if(!banks)
		return 0;

	for(i = 0; i < banks; i++)
	{
		if(start[i] == 0 && size[i] == 0)
			break;
	}
	banks = i;

	len = fdt_pack_reg(blob, tmp, start, size, banks);
	e = fdt_setprop(blob, o, "reg", tmp, len);
	if(e < 0)
	{
		printf("Could not set %s %s.\r\n", "reg", fdt_strerror(e));
		return -1;
	}
	return 0;
}

static int fdt_setprop_uxx(void *fdt, int o, const char * name, uint64_t val, int is_u64)
{
	if(is_u64)
		return fdt_setprop_u64(fdt, o, name, val);
	else
		return fdt_setprop_u32(fdt, o, name, (uint32_t)val);
}

static int do_fdt(int argc, char ** argv)
{
	if(argc < 2)
	{
		usage();
		return -1;
	}

	if(!strcmp(argv[1], "addr"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 0)
		{
			if(!fdt || fdt_check_header(fdt) != 0)
			{
				usage();
				return -1;
			}
			printf("The current fdt address is 0x%llx, length is %ld\r\n", fdt, fdt_totalsize(fdt));
		}
		else
		{
			void * addr = (void *)strtoul(argv[0], NULL, 0);
			int len = 0;
			int e = 0;
			if(!addr || (e = fdt_check_header(addr)) != 0)
			{
				printf("Check fdt failed at the address 0x%llx: %s\r\n", addr, fdt_strerror(e));
				return -1;
			}
			else
			{
				fdt = (struct fdt_header *)addr;
			}
			if(argc >= 2)
			{
				len = strtoul(argv[1], NULL, 0);
				if(len < fdt_totalsize(fdt))
				{
					printf ("New length %ld < existing length %ld, ignoring.\r\n", len, fdt_totalsize(fdt));
				}
				else
				{
					if((e = fdt_open_into(fdt, fdt, len)) != 0)
					{
						printf("libfdt fdt_open_into(): %s\r\n", fdt_strerror(e));
						return -1;
					}
				}
			}
		}
		return 0;
	}
	if(!fdt)
	{
		printf("No fdt memory address configured, Please config via 'fdt addr <address> [<length>]' command.\r\n");
		return -1;
	}
	if(!strcmp(argv[1], "resize"))
	{
		int extrasize;
		if(argc >= 3)
			extrasize = strtoul(argv[2], NULL, 0);
		else
			extrasize = 0;
		fdt_shrink_to_minimum(fdt, extrasize);
	}
	else if(!strcmp(argv[1], "print"))
	{
		int depth = FDT_MAX_LEVEL;
		char root[2] = "/";
		char * pathp;
		char * prop;
		int ret;

		if(argc == 2)
			pathp = root;
		else
			pathp = argv[2];
		if(argc > 3)
			prop = argv[3];
		else
			prop = NULL;
		ret = fdt_print(pathp, prop, depth);
		if(ret != 0)
			return ret;
	}
	else if(!strcmp(argv[1], "list"))
	{
		int depth = 1;
		char root[2] = "/";
		char * pathp;
		char * prop;
		int ret;

		if(argc == 2)
			pathp = root;
		else
			pathp = argv[2];
		if(argc > 3)
			prop = argv[3];
		else
			prop = NULL;
		ret = fdt_print(pathp, prop, depth);
		if(ret != 0)
			return ret;
	}
	else if(!strcmp(argv[1], "header"))
	{
		uint32_t version = fdt_version(fdt);
		printf("magic:\t\t\t0x%x\r\n", fdt_magic(fdt));
		printf("totalsize:\t\t0x%x (%d)\r\n", fdt_totalsize(fdt), fdt_totalsize(fdt));
		printf("off_dt_struct:\t\t0x%x\r\n", fdt_off_dt_struct(fdt));
		printf("off_dt_strings:\t\t0x%x\r\n", fdt_off_dt_strings(fdt));
		printf("off_mem_rsvmap:\t\t0x%x\r\n", fdt_off_mem_rsvmap(fdt));
		printf("version:\t\t%d\r\n", version);
		printf("last_comp_version:\t%d\r\n", fdt_last_comp_version(fdt));
		if(version >= 2)
			printf("boot_cpuid_phys:\t0x%x\r\n", fdt_boot_cpuid_phys(fdt));
		if(version >= 3)
			printf("size_dt_strings:\t0x%x\r\n", fdt_size_dt_strings(fdt));
		if(version >= 17)
			printf("size_dt_struct:\t\t0x%x\r\n", fdt_size_dt_struct(fdt));
		printf("number mem_rsv:\t\t0x%x\r\n", fdt_num_mem_rsv(fdt));
		printf("\r\n");
	}
	else if(!strcmp(argv[1], "bootcpu"))
	{
		if(argc >= 3)
		{
			int id = strtoul(argv[2], NULL, 0);
			fdt_set_boot_cpuid_phys(fdt, id);
		}
		else
		{
			usage();
			return -1;
		}
	}
	else if(!strcmp(argv[1], "set"))
	{
		if(argc >= 4)
		{
			char data[FDT_SCRATCHPAD] __attribute__((aligned(16)));
			const void * ptmp;
			char * pathp;
			char * prop;
			int o;
			int len;
			int e;

			pathp = argv[2];
			prop = argv[3];

			o = fdt_path_offset(fdt, pathp);
			if(o < 0)
			{
				printf("libfdt fdt_path_offset() returned %s\r\n", fdt_strerror(o));
				return -1;
			}
			if(argc == 4)
			{
				len = 0;
			}
			else
			{
				ptmp = fdt_getprop(fdt, o, prop, &len);
				if(len > FDT_SCRATCHPAD)
				{
					printf("prop (%ld) doesn't fit in scratchpad!\r\n", len);
					return -1;
				}
				if(ptmp != NULL)
					memcpy(data, ptmp, len);
				e = fdt_parse_prop(&argv[4], argc - 4, data, &len);
				if(e != 0)
					return -1;
			}
			e = fdt_setprop(fdt, o, prop, data, len);
			if(e < 0)
			{
				printf("libfdt fdt_setprop(): %s\r\n", fdt_strerror(e));
				return -1;
			}
		}
		else
		{
			usage();
			return -1;
		}
	}
	else if(!strcmp(argv[1], "mk"))
	{
		if(argc >= 4)
		{
			char * pathp;
			char * nodep;
			int o;
			int e;

			pathp = argv[2];
			nodep = argv[3];
			o = fdt_path_offset (fdt, pathp);
			if(o < 0)
			{
				printf("libfdt fdt_path_offset() returned %s\r\n", fdt_strerror(o));
				return -1;
			}
			e = fdt_add_subnode(fdt, o, nodep);
			if(e < 0)
			{
				printf("libfdt fdt_add_subnode(): %s\r\n", fdt_strerror(e));
				return -1;
			}
		}
		else
		{
			usage();
			return -1;
		}
	}
	else if(!strcmp(argv[1], "rm"))
	{
		if(argc >= 3)
		{
			int o;
			int e;

			o = fdt_path_offset(fdt, argv[2]);
			if(o < 0)
			{
				printf("libfdt fdt_path_offset() returned %s\r\n", fdt_strerror(o));
				return -1;
			}
			if(argc >= 4)
			{
				e = fdt_delprop(fdt, o, argv[3]);
				if(e < 0)
				{
					printf("libfdt fdt_delprop(): %s\r\n", fdt_strerror(e));
					return -1;
				}
			}
			else
			{
				e = fdt_del_node(fdt, o);
				if(e < 0)
				{
					printf("libfdt fdt_del_node(): %s\r\n", fdt_strerror(e));
					return -1;
				}
			}
		}
		else
		{
			usage();
			return -1;
		}
	}
	else if(!strcmp(argv[1], "memory"))
	{
		if(argc >= 4)
		{
			uint64_t addr[32], size[32];
			int e, banks;

			argc -= 2;
			argv += 2;
			for(banks = 0; argc > 0; banks++)
			{
				addr[banks] = strtoull(argv[0], NULL, 0);
				size[banks] = strtoull(argv[1], NULL, 0);
				argc -= 2;
				argv += 2;
			}
			e = fdt_memory_banks(fdt, addr, size, banks);
			if(e < 0)
				return -1;
		}
		else
		{
			usage();
			return -1;
		}
	}
	else if(!strcmp(argv[1], "chosen"))
	{
		if(argc >= 3)
		{
			char * s = argv[2];
			int o;
			int e;

			o = fdt_find_or_add_subnode(fdt, 0, "chosen");
			if(o < 0)
				return -1;
			if(s)
			{
				if((e = fdt_setprop(fdt, o, "bootargs", s, strlen(s) + 1)) < 0)
				{
					printf("Could not set bootargs %s.\r\n", fdt_strerror(e));
					return -1;
				}
			}
		}
		else
		{
			usage();
			return -1;
		}
	}
	else if(!strcmp(argv[1], "initrd"))
	{
		uint64_t initrd_start, initrd_end;
		uint64_t addr, size;
		int e, i, total;
		int is_u64;
		int o;

		if(argc == 2)
		{
			initrd_start = 0;
			initrd_end = 0;
		}
		else if(argc == 4)
		{
			initrd_start = strtoull(argv[2], NULL, 0);
			initrd_end = strtoull(argv[3], NULL, 0);
		}
		else
		{
			usage();
			return -1;
		}

		if(initrd_start == initrd_end)
			return 0;
		o = fdt_find_or_add_subnode(fdt, 0, "chosen");
		if(o < 0)
			return -1;
		total = fdt_num_mem_rsv(fdt);
		for(i = 0; i < total; i++)
		{
			e = fdt_get_mem_rsv(fdt, i, &addr, &size);
			if(addr == initrd_start)
			{
				fdt_del_mem_rsv(fdt, i);
				break;
			}
		}
		e = fdt_add_mem_rsv(fdt, initrd_start, initrd_end - initrd_start);
		if(e < 0)
		{
			printf("fdt_initrd: %s\r\n", fdt_strerror(e));
			return -1;
		}
		is_u64 = (fdt_address_cells(fdt, 0) == 2) ? 1 : 0;
		if((e = fdt_setprop_uxx(fdt, o, "linux,initrd-start", (uint64_t)initrd_start, is_u64)) < 0)
		{
			printf("Could not set linux,initrd-start %s.\r\n", fdt_strerror(e));
			return -1;
		}
		if((e = fdt_setprop_uxx(fdt, o, "linux,initrd-end", (uint64_t)initrd_end, is_u64)) < 0)
		{
			printf("Could not set linux,initrd-end %s.\r\n", fdt_strerror(e));
			return -1;
		}
	}
	else if(!strcmp(argv[1], "rsvmem"))
	{
		if(argc >= 3)
		{
			if(!strcmp(argv[2], "print"))
			{
				uint64_t addr, size;
				int total = fdt_num_mem_rsv(fdt);
				int i, e;
				printf("index\t\t   start\t\t    size\r\n");
				printf("------------------------------------------------\r\n");
				for(i = 0; i < total; i++)
				{
					e = fdt_get_mem_rsv(fdt, i, &addr, &size);
					if(e < 0)
					{
						printf("libfdt fdt_get_mem_rsv():  %s\r\n", fdt_strerror(e));
						return -1;
					}
					printf("    %x\t%08x%08x\t%08x%08x\r\n", i, (uint32_t)(addr >> 32), (uint32_t)(addr & 0xffffffff), (uint32_t)(size >> 32), (uint32_t)(size & 0xffffffff));
				}
			}
			else if(!strcmp(argv[2], "add"))
			{
				if(argc >= 5)
				{
					uint64_t addr, size;
					int e;

					addr = strtoull(argv[3], NULL, 0);
					size = strtoull(argv[4], NULL, 0);
					e = fdt_add_mem_rsv(fdt, addr, size);
					if(e < 0)
					{
						printf("libfdt fdt_add_mem_rsv():  %s\r\n", fdt_strerror(e));
						return -1;
					}
				}
				else
				{
					usage();
					return -1;
				}
			}
			else if(!strcmp(argv[2], "delete"))
			{
				if(argc >= 4)
				{
					int idx = strtoul(argv[3], NULL, 0);
					int e = fdt_del_mem_rsv(fdt, idx);
					if(e < 0)
					{
						printf("libfdt fdt_del_mem_rsv():  %s\r\n", fdt_strerror(e));
						return -1;
					}
				}
				else
				{
					usage();
					return -1;
				}
			}
			else
			{
				usage();
				return -1;
			}
		}
		else
		{
			usage();
			return -1;
		}
	}
	else
	{
		usage();
		return -1;
	}

	return 0;
}

static struct command_t cmd_fdt = {
	.name	= "fdt",
	.desc	= "flattened device tree utility commands",
	.usage	= usage,
	.exec	= do_fdt,
};

static __init void fdt_cmd_init(void)
{
	register_command(&cmd_fdt);
}

static __exit void fdt_cmd_exit(void)
{
	unregister_command(&cmd_fdt);
}

command_initcall(fdt_cmd_init);
command_exitcall(fdt_cmd_exit);
