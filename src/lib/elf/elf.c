/*
 * elf/elf.c
 */

#include <xboot.h>
//#include <fs/fileio.h>
#include <xboot/module.h>
#include <elf/elf.h>

struct elf_image_t {
	char * name;
	void * data;
	size_t size;
};

struct elf_image_t * elf_image_alloc(char * path)
{
	struct stat st;
	struct elf_image_t * image;
	char * name;
	void * data;
	size_t n = 0, size = 0;
	int fd;

	if(stat(path, &st) != 0)
		return NULL;

	if(S_ISDIR(st.st_mode))
		return NULL;

	if(st.st_size <= 0)
		return NULL;

	fd = open(path, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return NULL;

	image = malloc(sizeof(struct elf_image_t));
	name = strdup(path);
	data = malloc(st.st_size);

	if(!image || !name || !data)
	{
		if(image)
			free(image);
		if(name)
			free(name);
		if(data)
			free(data);
	}

    for(;;)
    {
        n = read(fd, (void *)(data + size), SZ_512K);
        if(n <= 0)
        	break;
		size += n;
    }

	close(fd);

	image->data = data;
	image->size = size;

	return image;
}

void elf_image_free(struct elf_image_t * image)
{
	if(image)
	{
		if(image->data)
			free(image->data);

		if(image->name)
			free(image->name);

		if(image)
			free(image);
	}
}

static struct module_t * _load_shared_object(const char *name, void *module_ptr);

struct module_t * module_load(void * elf)
{
	if(!elf32_is_valid(elf))
		return NULL;

	struct elf32_ehdr * e = (struct elf32_ehdr *) (elf);
	if(e->e_type == ET_DYN)
	{
		printk("et_dyn\r\n");
	}

	return _load_shared_object("abc", elf);
}




#define elf_module          ((struct elf32_ehdr *)module_ptr)
#define shdr                ((struct elf32_shdr *)((u8_t *)module_ptr + elf_module->e_shoff))
#define phdr                ((struct elf32_phdr *)((u8_t *)module_ptr + elf_module->e_phoff))

#define	ELF_ST_BIND(info)           ((info) >> 4)
#define	ELF_ST_TYPE(info)           ((info) & 0xf)
#define	ELF_ST_INFO(bind, type)     (((bind)<<4)+((type)&0xf))

#define	STB_LOCAL   0       /* BIND */
#define	STB_GLOBAL  1
#define	STB_WEAK    2
#define	STB_NUM     3

#define ELF_BSS         ".bss"         /* uninitialized data */
#define ELF_DATA        ".data"        /* initialized data */
#define ELF_DEBUG       ".debug"       /* debug */
#define ELF_DYNAMIC     ".dynamic"     /* dynamic linking information */
#define ELF_DYNSTR      ".dynstr"      /* dynamic string table */
#define ELF_DYNSYM      ".dynsym"      /* dynamic symbol table */
#define ELF_FINI        ".fini"        /* termination code */
#define ELF_GOT         ".got"         /* global offset table */
#define ELF_HASH        ".hash"        /* symbol hash table */
#define ELF_INIT        ".init"        /* initialization code */
#define ELF_REL_DATA    ".rel.data"    /* relocation data */
#define ELF_REL_FINI    ".rel.fini"    /* relocation termination code */
#define ELF_REL_INIT    ".rel.init"    /* relocation initialization code */
#define ELF_REL_DYN     ".rel.dyn"     /* relocaltion dynamic link info */
#define ELF_REL_RODATA  ".rel.rodata"  /* relocation read-only data */
#define ELF_REL_TEXT    ".rel.text"    /* relocation code */
#define ELF_RODATA      ".rodata"      /* read-only data */
#define ELF_SHSTRTAB    ".shstrtab"    /* section header string table */
#define ELF_STRTAB      ".strtab"      /* string table */
#define ELF_SYMTAB      ".symtab"      /* symbol table */
#define ELF_TEXT        ".text"        /* code */

#define	STT_NOTYPE  0       /* symbol type is unspecified */
#define	STT_OBJECT  1       /* data object */
#define	STT_FUNC    2       /* code object */
#define	STT_SECTION 3       /* symbol identifies an ELF section */
#define	STT_FILE    4       /* symbol's name is file name */
#define	STT_COMMON  5       /* common data object */
#define	STT_TLS     6       /* thread-local data object */
#define	STT_NUM     7       /* # defined types in generic range */
#define	STT_LOOS    10      /* OS specific range */
#define	STT_HIOS    12
#define	STT_LOPROC  13      /* processor specific range */
#define	STT_HIPROC  15

#define IS_REL(s)           (s.sh_type == SHT_REL)











/*
 * Relocation type for arm
 */
#define	R_ARM_NONE			0
#define	R_ARM_PC24			1
#define	R_ARM_ABS32			2
#define	R_ARM_REL32			3
#define R_ARM_THM_CALL		10
#define	R_ARM_GLOB_DAT		21
#define	R_ARM_JUMP_SLOT		22
#define	R_ARM_RELATIVE		23
#define	R_ARM_GOT_BREL		26
#define	R_ARM_PLT32			27
#define	R_ARM_CALL			28
#define	R_ARM_JUMP24		29
#define R_ARM_THM_JUMP24	30
#define	R_ARM_V4BX			40


static int rt_module_arm_relocate(struct module_t *module, struct elf32_rel *rel, elf32_addr sym_val)
{
	elf32_addr *where, tmp;
	elf32_sword addend, offset;
	u32_t upper, lower, sign, j1, j2;

	where = (elf32_addr *)((u8_t *)module->space + rel->r_offset);
	switch (ELF32_R_TYPE(rel->r_info))
	{
	case R_ARM_NONE:
		break;
	case R_ARM_ABS32:
		*where += (elf32_addr)sym_val;
		printk("R_ARM_ABS32: %x -> %x\n", where, *where);
		break;
	case R_ARM_PC24:
	case R_ARM_PLT32:
	case R_ARM_CALL:
	case R_ARM_JUMP24:
		addend = *where & 0x00ffffff;
		if (addend & 0x00800000)
			addend |= 0xff000000;
		tmp = sym_val - (elf32_addr)where + (addend << 2);
		tmp >>= 2;
		*where = (*where & 0xff000000) | (tmp & 0x00ffffff);
		printk( "R_ARM_PC24: %x -> %x\n", where, *where);
		break;
	case R_ARM_REL32:
		*where += sym_val - (elf32_addr)where;
		printk("R_ARM_REL32: %x -> %x, sym %x, offset %x\n", where, *where, sym_val, rel->r_offset);
		break;
	case R_ARM_V4BX:
		*where &= 0xf000000f;
		*where |= 0x01a0f000;
		break;
	case R_ARM_GLOB_DAT:
	case R_ARM_JUMP_SLOT:
		*where = (elf32_addr)sym_val;
		printk(	"R_ARM_JUMP_SLOT: 0x%x -> 0x%x 0x%x\n", where, *where, sym_val);
		break;
#if 0		/* To do */
	case R_ARM_GOT_BREL:
		temp = (elf32_addr)sym_val;
		*where = (elf32_addr)&temp;
		RT_DEBUG_LOG(RT_DEBUG_MODULE,
			("R_ARM_GOT_BREL: 0x%x -> 0x%x 0x%x\n", where, *where, sym_val));
		break;
#endif
	case R_ARM_RELATIVE:
		*where += (elf32_addr)sym_val;
		//RT_DEBUG_LOG(RT_DEBUG_MODULE,
			//("R_ARM_RELATIVE: 0x%x -> 0x%x 0x%x\n", where, *where, sym_val));
		break;
	case R_ARM_THM_CALL:
	case R_ARM_THM_JUMP24:
		upper = *(u16_t *)where;
		lower = *(u16_t *)((elf32_addr)where + 2);

		sign = (upper >> 10) & 1;
		j1 = (lower >> 13) & 1;
		j2 = (lower >> 11) & 1;
		offset = (sign << 24) | ((~(j1 ^ sign) & 1) << 23) |
			 				 ((~(j2 ^ sign) & 1) << 22) |
			 				 ((upper & 0x03ff) << 12) |
			 				 ((lower & 0x07ff) << 1);
		if (offset & 0x01000000)
			 				 offset -= 0x02000000;
		offset += sym_val - (elf32_addr)where;

		if (!(offset & 1) || offset <= (s32_t)0xff000000 ||
			 	offset >= (s32_t)0x01000000)
		{
			printk("only Thumb addresses allowed\n");

			return -1;
		}

		sign = (offset >> 24) & 1;
		j1 = sign ^ (~(offset >> 23) & 1);
		j2 = sign ^ (~(offset >> 22) & 1);
		*(u16_t *)where = (u16_t)((upper & 0xf800) | (sign << 10) |
			 						 ((offset >> 12) & 0x03ff));
		*(u16_t *)(where + 2) = (u16_t)((lower & 0xd000) |
			 						   (j1 << 13) | (j2 << 11) |
			 						   ((offset >> 1) & 0x07ff));
		upper = *(u16_t *)where;
		lower = *(u16_t *)((elf32_addr)where + 2);
		break;
	default:
		return -1;
	}

	return 0;
}


static struct module_t * _load_shared_object(const char *name, void *module_ptr)
{
	u8_t *ptr = NULL;
	struct module_t * module = NULL;
	bool_t linked = FALSE;
	u32_t index, module_size = 0;

	if(memcmp(elf_module->e_ident, "\177RTM", SELFMAG) == 0)
	{
		/* rtmlinker finished */
		linked = TRUE;
	}

	/* get the ELF image size */
	for (index = 0; index < elf_module->e_phnum; index++)
	{
		if(phdr[index].p_type == PT_LOAD)
			module_size += phdr[index].p_memsz;
	}

	if (module_size == 0)
	{
		printk(" module size error\n");
		return NULL;
	}

	/* allocate module */
	module = (struct module_t *) malloc(sizeof(struct module_t));
	if (!module)
		return NULL ;
/*
	module->nref = 0;*/


	/* allocate module space */
	module->space = malloc(module_size);
	if (module->space == NULL)
	{
		free(module);

		return NULL;
	}


	module->name = strdup(name);

	/* zero all space */
	ptr = module->space;
	memset(ptr, 0, module_size);

	printk(" load address at 0x%x\n", ptr);

	for (index = 0; index < elf_module->e_phnum; index++)
	{
		if (phdr[index].p_type == PT_LOAD)
		{
			memcpy(ptr, (u8_t *)elf_module + phdr[index].p_offset, phdr[index].p_filesz);
			ptr += phdr[index].p_memsz;
		}
	}

	/* set module entry */
	module->module_entry = module->space + elf_module->e_entry;

	/* handle relocation section */
	for (index = 0; index < elf_module->e_shnum; index ++)
	{
		if (IS_REL(shdr[index]))
		{
			u32_t i, nr_reloc;
			struct elf32_sym *symtab;
			struct elf32_rel *rel;
			u8_t *strtab;
			static bool_t unsolved = FALSE;

			/* get relocate item */
			rel = (struct elf32_rel *)((u8_t *)module_ptr + shdr[index].sh_offset);

			/* locate .rel.plt and .rel.dyn section */
			symtab =(struct elf32_sym *) ((u8_t*)module_ptr + shdr[shdr[index].sh_link].sh_offset);
			strtab = (u8_t*) module_ptr + shdr[shdr[shdr[index].sh_link].sh_link].sh_offset;
			nr_reloc = (u32_t) (shdr[index].sh_size / sizeof(struct elf32_rel));

			/* relocate every items */
			for (i = 0; i < nr_reloc; i ++)
			{
				struct elf32_sym *sym = &symtab[ELF32_R_SYM(rel->r_info)];

				printk("relocate symbol %s shndx %d\n", strtab + sym->st_name, sym->st_shndx);

				if((sym->st_shndx != SHT_NULL) || (ELF_ST_BIND(sym->st_info) == STB_LOCAL))
				{
					rt_module_arm_relocate(module, rel,  (elf32_addr)(module->space + sym->st_value));
				}
				else if(!linked)
				{
					elf32_addr addr;

					printk("relocate symbol: %s\n", strtab + sym->st_name);

					/* need to resolve symbol in kernel symbol table */
					addr = (u32_t)(__symbol_get((const char *)(strtab + sym->st_name)));
					if (addr == 0)
					{
						printk("can't find %s in kernel symbol table\n", strtab + sym->st_name);
						unsolved = TRUE;
					}
					else
						rt_module_arm_relocate(module, rel, addr);
				}
				rel ++;
			}

			if (unsolved)
			{
				//rt_object_delete(&(module->parent));
				free(module);

				return NULL;
			}
		}
	}

	/* construct module symbol table */
	for (index = 0; index < elf_module->e_shnum; index ++)
	{
		/* find .dynsym section */
		u8_t *shstrab = (u8_t *)module_ptr + shdr[elf_module->e_shstrndx].sh_offset;
		if (strcmp((const char *)(shstrab + shdr[index].sh_name), ELF_DYNSYM) == 0)
			break;
	}

	/* found .dynsym section */
	if (index != elf_module->e_shnum)
	{
		int i, count = 0;
		struct elf32_sym *symtab = NULL;
		u8_t *strtab = NULL;

		symtab =(struct elf32_sym *)((u8_t *)module_ptr + shdr[index].sh_offset);
		strtab = (u8_t *)module_ptr + shdr[shdr[index].sh_link].sh_offset;

		for (i=0; i<shdr[index].sh_size/sizeof(struct elf32_sym); i++)
		{
			if ((ELF_ST_BIND(symtab[i].st_info) == STB_GLOBAL) && (ELF_ST_TYPE(symtab[i].st_info) == STT_FUNC))
				count ++;
		}

		module->symtab = (struct symbol_t *)malloc(count * sizeof(struct symbol_t));
		module->nsym = count;
		for (i=0, count=0; i<shdr[index].sh_size/sizeof(struct elf32_sym); i++)
		{
			if ((ELF_ST_BIND(symtab[i].st_info) == STB_GLOBAL) && (ELF_ST_TYPE(symtab[i].st_info) == STT_FUNC))
			{
				size_t length = strlen((const char *)(strtab + symtab[i].st_name)) + 1;

				module->symtab[count].addr = (void *)(module->space + symtab[i].st_value);
				module->symtab[count].name = malloc(length);
				memset((void *)module->symtab[count].name, 0, length);
				memcpy((void *)module->symtab[count].name, strtab + symtab[i].st_name, length);
				count ++;
			}
		}
	}

	return module;
}









void test_elf(char * path)
{
	struct elf_image_t * img;
	struct module_t * module;

	img = elf_image_alloc(path);
	if(!img)
	{
		printk("load elf fail\r\n");
		return;
	}

	printk("load elf ok\r\n");

	module = module_load(img->data);
	if(module)
	{
		if(add_module(module))
			printk(" add module ok\r\n");
		else
			printk(" add module false\r\n");
	}
	else
		printk("load fail\r\n");

	/*

	struct elf32_ehdr * e = (struct elf32_ehdr *) (img->data);
	struct elf32_sym * sym;
	struct elf32_shdr * s;
	int i;
	char * p;

	for (i = 0; i < e->e_shnum; i++)
	{
		sym = elf_get_symbol_by_name(img->data, "aa");
	}
*/


	elf_image_free(img);
}



