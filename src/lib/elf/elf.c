/*
 * elf/elf.c
 */

#include <xboot.h>
//#include <fs/fileio.h>
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



void test_elf(void)
{
	struct elf_image_t * i;

	i = elf_image_alloc("/romdisk/hello.so");
	if(!i)
	{
		printk("load elf fail\r\n");
		return;
	}

	printk("load elf ok\r\n");

	elf_check_header(i);


	elf_image_free(i);
}


char *elf_section_name(void *image, int section)
{
	struct elf32_ehdr *header = image;
	struct elf32_shdr *sections = image + header->e_shoff;
	return image + sections[header->e_shstrndx].sh_offset + sections[section].sh_name;
}

elf_check_header (struct elf_image_t * image)
{
	struct elf32_ehdr * e = (struct elf32_ehdr *)(image->data);
	int i;
  if (e->e_ident[EI_MAG0] != ELFMAG0
      || e->e_ident[EI_MAG1] != ELFMAG1
      || e->e_ident[EI_MAG2] != ELFMAG2
      || e->e_ident[EI_MAG3] != ELFMAG3
      || e->e_ident[EI_VERSION] != EV_CURRENT
      || e->e_version != EV_CURRENT)
  {

	  for(i=0;i<16;i++)
	  {
		  printk("%02x ", e->e_ident[i]);
	  }

	  printk("error elf magic\r\n");
	  return;
  }
  else
	  printk("ok elf magic\r\n");

  char * p;
	for (i = 0; i < 10; i++) {
		p = elf_section_name(image->data, i);
		printk(p);
		printk("\r\n");
	}













}












///////////////////
#define DT_NEEDED 1
/////////////////////



struct elf32_shdr *elf_get_section_by_id(void *image, int section)
{
	struct elf32_ehdr *header = image;
	struct elf32_shdr *sections = image + header->e_shoff;

	if(section >= header->e_shnum)
		return 0;

	return &sections[section];
}

struct elf32_shdr *elf_get_section_by_name(void *image, char *name)
{
	struct elf32_ehdr * header = image;
	int i;
	for(i=0;i<header->e_shnum;i++)
		if(!strcmp(elf_section_name(image, i), name))
			return elf_get_section_by_id(image, i);
	return 0;
}

struct elf32_sym *elf_get_symbol_by_id(void *image, int id)
{
	struct elf32_shdr *sect = elf_get_section_by_name(image, ".symtab");
	struct elf32_sym *symtab = image + sect->sh_offset;
	if(id * sizeof(struct elf32_sym) > sect->sh_size)
		return 0;
	return &symtab[id];
}

struct elf32_sym *elf_get_symbol_by_name(void *image, char *name)
{
	struct elf32_shdr *sect = elf_get_section_by_name(image, ".symtab");
	struct elf32_shdr *strtab = elf_get_section_by_name(image, ".strtab");
	struct elf32_sym *symtab = image + sect->sh_offset;
	int i;
	int size = sect->sh_size / sizeof(struct elf32_sym);

	for(i=0;i<size;i++)
	{
		if (symtab[i].st_name == 0)
			continue;

		if(strcmp((char *)((unsigned int)image + strtab->sh_offset + symtab[i].st_name), name) == 0)
			return &symtab[i];
	}
	return 0;
}

int elf_get_sym(void *image, int index, unsigned int *val, unsigned int symtab_sect, void *(*import_func)(char *name))
{
	struct elf32_shdr *sect;
	struct elf32_sym *sym;
	struct elf32_ehdr *elf = image;
	char *sym_name;
	void *s;

	if(symtab_sect >= elf->e_shnum)	//Bad symbol table
	{
		return -1;
	}

	sect = elf_get_section_by_id(image, symtab_sect);
	if(sect == 0)
	{
		printk("elf_get_sym1 - SECT UGUALE A 0!!!\n");
		return -1;
	}

	if(index >= sect->sh_size) //Offset into symbol table exceeds symbol table size
	{
		return -1;
	}

	sym = (struct elf32_sym*)(image + sect->sh_offset) + index;
	if(sym == 0)
	{
		printk("elf_get_sym1 - sym UGUALE A 0!!!\n");
		return -1;
	}

	if(sym->st_shndx == 0) // Symbol from import
	{
		//FORSE QUI DOBBIAMO METTERE IL CODICE PER PRENDERE IL SIMBOLO DALLE SHARED LIBRARIES

		if(!import_func)
			return -1;

		sect = elf_get_section_by_id(image, sect->sh_link);
		if(sect == 0)
		{
			printk("elf_get_sym2 - SECT UGUALE A 0!!!\n");
			return -1;
		}

		sym_name = (char*)(image + sect->sh_offset + sym->st_name);

	if(sym_name == 0)
	{
		printk("elf_get_sym1 - sym_name UGUALE A 0!!!\n");
		return -1;
	}

		s = import_func(sym_name);
		if(!s)
		{
			printk("elf_get_sym(): symbol %s not exist!\n", sym_name);
			return -1;
		}
		*val = (unsigned int)s;
	}
	else
	{
		sect = elf_get_section_by_id(image, sym->st_shndx);
		if(sect == 0)
		{
			printk("elf_get_sym3 - SECT UGUALE A 0!!!\n");
			return -1;
		}

		*val = (unsigned int)(image + sym->st_value + sect->sh_offset);
	}
	return 0;
}

int elf_get_deps(void *image)
{
	struct elf32_shdr *dynamic_header;
	struct elf32_shdr *string_header;
	struct elf32_dyn *dyn_array;
	int numLibraries = 0;
	unsigned char *string;
	int count;

	dynamic_header = elf_get_section_by_name(image,".dynamic");
	if(dynamic_header == 0)
	{
		printk("elf_get_deps: image has no dynamic linking section");
		return -1;
	}

	string_header = elf_get_section_by_id(image, dynamic_header->sh_link);
	if(string_header == 0)
	{
		printk("elf_get_deps: cannot find ELF image dynamic string header");
		return -1;
	}

	dyn_array = (struct elf32_dyn*)(image + dynamic_header->sh_offset);

	for(count=0; (dyn_array[count].d_tag != 0); count++)
	{
		if(dyn_array[count].d_tag == DT_NEEDED)
		{
			string = (unsigned char*)(image + string_header->sh_offset+dyn_array[count].d_un.d_val);
			//for now, only shows dependencies on stdout
			printk("elf_get_deps: %s\n",string);
			numLibraries += 1;
		}
	}
	return 0;
}

int elf_do_reloc(void *image, struct elf32_rela *reloc, struct elf32_shdr *sect, void *(*import_func)(char *name))
{
	struct elf32_shdr *t_sect;
	unsigned int sym_val;
	unsigned int *where;
	int err;

	t_sect = elf_get_section_by_id(image, sect->sh_info);
	if(t_sect == 0)
	{
		printk("elf_do_reloc - T_SECT UGUALE A 0!!!\n");
		return -1;
	}

	where = (unsigned int*)(image + t_sect->sh_offset + reloc->r_offset);
	if(where == 0)
	{
		printk("elf_do_reloc - where UGUALE A 0!!!\n");
		return -1;
	}

//	err = elf_get_sym(image, ELF32_R_SYM(reloc->r_info), &sym_val, sect->sh_link, import_func);

	if(err != 0)
		return err;

#if 0 //xxx
	switch(ELF32_R_TYPE(reloc->r_info))
	{
		case R_386_NONE:
			break;

		case R_386_32: // (S + A)
		{
			*where = sym_val + *where;
			break;
		}
		case R_386_PC32: // (S + A - P)
		{
			*where = sym_val + *where - (unsigned int)where;
			break;
		}

		case R_386_GLOB_DAT: // S
		case R_386_JMP_SLOT:
			*where = (unsigned int)sym_val;
		break;

		case R_386_RELATIVE:
			*where += (unsigned int)image;
		break;

		default:
		{
			return -1;
		}
	}
#endif
	return 0;
}

#if 0
int elf_load_reloc(void *image, void *(*import_func)(char *name))
{
	// First check if module is elf file
	struct elf32_ehdr *elf = image;
	struct elf32_shdr *sect;
	struct elf32_rela *reloc;
	int reloc_size;
	int err;
	int i, j;
	void *bss;

	if(!IS_ELF(elf))
		return -1;

	if(elf->e_type != ET_REL)
		return -1;

	for(i=0; i<elf->e_shnum; i++)
	{
		sect = elf_get_section_by_id(image, i);
//xxx		if(sect->sh_type != SHT_NOBITS) continue;
		bss = kmalloc(sect->sh_size);
		memset(bss, 0, sect->sh_size);
		sect->sh_offset = (unsigned int)bss - (unsigned int)image;
		sect->sh_addr = (unsigned int)bss;
	}

	printk("image: 0x%x\n", image);

	for(i=0; i<elf->e_shnum; i++)
	{
		sect = elf_get_section_by_id(image, i);

		printk("SECT NUMBER: %d, sect: 0x%x\n", i, sect);

		/*if(sect->sh_type == SHT_NOBITS)
		{
			bss = kmalloc(sect->sh_size);
			memset(bss, 0, sect->sh_size);
			sect->sh_offset = (unsigned int)bss - (unsigned int)image;

			printk("size: 0x%x\n", sect->sh_size);

			continue;
		}
		else*/
/*
		if(sect->sh_type == SHT_RELA)
			reloc_size = sizeof(struct elf32_rela);
		else if(sect->sh_type == SHT_REL)
			reloc_size = sizeof(Elf32_Rel);
		else
			continue;
*/

		int off = sect->sh_offset;

		int fin = sect->sh_size/reloc_size;

		for(j=0; j<fin; j++)
		{
			reloc = (struct elf32_rela*)(image + off + reloc_size * j);
			if(reloc == 0)
			{
				printk("elf_load_reloc - reloc UGUALE A 0\n");
				kfree(bss);
				return -1;
			}

			err = elf_do_reloc(image, reloc, sect, import_func);

			if(err != 0)
			{
				kfree(bss);
				return err;
			}
		}
	}

	return 0;
}
#endif

#if 0
#define ROUNDUP(a, b) (((a) + ((b)-1)) & ~((b)-1))
#define ROUNDOWN(a, b) (((a) / (b)) * (b))

#define ROUND_DOWN_TO_PAGE(a) ROUNDOWN(a, PAGE_SIZE)

//FORSE POTREI MAPPARE LE SEZIONI IN MEMORIA COME VmmMapFile
//E FORSE POTREI FARE QUESTA FUNZIONE UN TUTT'UNO CON TaskCreate, COSI' DA EVITARCI NUMEROSI CASINI
int elf_exec(char *path, int priority, int uid, int gid, int argc, char **argv)
{
	fs_node_t *elf_file;
	int result = VfsOpen(path, &elf_file, FS_EXEC, 0);

	if(result != ESUCCESS)
		return result;

	struct elf32_ehdr eheader;
	Elf32_Phdr *pheaders = 0;

	VfsRequest(elf_file, FS_READ, &eheader, 0, sizeof(struct elf32_ehdr), &result, 0, 0, 0);

	while(result==SIOPENDING)
		enable();

	if(result != ESUCCESS)
	{
		VfsClose(elf_file, 0);
		return result;
	}

	if(eheader.e_type != ET_EXEC)
		return ENOEXEC;
	if(memcmp(eheader.e_ident, ELFMAG, 4) != 0)
		return ENOEXEC;
	if(eheader.e_ident[EI_CLASS] != ELFCLASS32)
		return ENOEXEC;
	if(eheader.e_ident[EI_VERSION] != EV_CURRENT)
		return ENOEXEC;
	if(eheader.e_phoff == 0)
		return ENOEXEC;
	if(eheader.e_phentsize < sizeof(Elf32_Phdr))
		return ENOEXEC;

	pheaders = kmalloc(eheader.e_phnum * eheader.e_phentsize);
	if(pheaders == 0)
	{
		VfsClose(elf_file, 0);
		return ENOMEM;
	}

	VfsRequest(elf_file, FS_READ, pheaders, eheader.e_phoff, eheader.e_phnum * eheader.e_phentsize, &result, 0, 0, 0);

	while(result==SIOPENDING)
		enable();

	if(result != ESUCCESS)
	{
		kfree(pheaders);
		VfsClose(elf_file, 0);
		return result;
	}

	void (*entry)() = eheader.e_entry;

	task_t *task = TaskCreate(entry, elf_file->name, priority, uid, gid, argc, argv);
	if(task == 0)
		return EGENERIC;

	int i;
	for(i=0; i<eheader.e_phnum; i++)
	{
		if(pheaders[i].p_type != PT_LOAD)
			continue;

		unsigned int *region_addr;
		region_addr = (char*)PAGE_ALIGN(pheaders[i].p_vaddr);

		printk("region_addr: 0x%x\n", region_addr);

		#warning IL PROBLEMA E' NEL MAPPING!!! DEVO FARLO PIU' O MENO COME NEWOS
			//PERCHÈ PER COME LO FACCIO ORA SI MAPPANO LE PAGINE A PARTIRE DALL'OFFSET...
				//(0X20080 AL POSTO DI 0X20000)
				//DA VEDERE BENE COME NEWOS

		//RW Segment
		if(pheaders[i].p_flags & PF_W)
		{
			unsigned start_clearing;
			unsigned to_clear;
			unsigned A = pheaders[i].p_vaddr+pheaders[i].p_memsz;
			unsigned B = pheaders[i].p_vaddr+pheaders[i].p_filesz;

			A = ROUNDOWN(A, PAGE_SIZE);
			B = ROUNDOWN(B, PAGE_SIZE);

			//Crea una zona di memoria settandola a zero
			int pages = ROUNDUP(pheaders[i].p_filesz + (pheaders[i].p_vaddr % PAGE_SIZE), PAGE_SIZE) / PAGE_SIZE;

			//Inizializza l'user stack per l'utente inserendo un nuovo nodo nella memoria virtuale del task
			vm_desc_t *desc = kmalloc(sizeof(vm_desc_t));
			if(desc == 0)
			{
				#warning ELIMINA TUTTO
				return ENOMEM;
			}
			memset(desc, 0, sizeof(vm_desc_t));
			desc->pages = kmalloc(pages * sizeof(int));
			if(desc->pages==0)
			{
				#warning ELIMINA TUTTO
				return ENOMEM;
			}

			desc->num_pages = pages;
			desc->num_copies = 1;
			desc->type = VM_AREA_FILE;
			desc->spin.lock = 1;
			desc->mtx_allocate.lock = 1;
			desc->dest.file.file = elf_file;
			desc->dest.file.offset = ROUNDOWN(pheaders[i].p_offset, PAGE_SIZE);
			//desc->dest.file.size = pheaders[i].p_filesz;

			vm_node_t *node = VmmAllocNode(task, ROUND_DOWN_TO_PAGE(pheaders[i].p_vaddr), VM_MEM_ZERO | VM_MEM_USER | VM_MEM_READ | VM_MEM_WRITE, desc, 0);
			if(node == 0)
			{
				#warning ELIMINA TUTTO
				return ENOMEM;
			}

			/*
			 * check if we need extra storage for the bss
			 */
			if(A != B) {
				unsigned int bss_size;

				bss_size=
					ROUNDUP(pheaders[i].p_memsz+ (pheaders[i].p_vaddr % PAGE_SIZE), PAGE_SIZE)
					- ROUNDUP(pheaders[i].p_filesz+ (pheaders[i].p_vaddr % PAGE_SIZE), PAGE_SIZE);

				region_addr+= ROUNDUP(pheaders[i].p_filesz+ (pheaders[i].p_vaddr % PAGE_SIZE), PAGE_SIZE);

				//Crea una zona di memoria settandola a zero
				int pages = bss_size / PAGE_SIZE;

				vm_desc_t *desc = kmalloc(sizeof(vm_desc_t));
				if(desc == 0)
				{
					#warning ELIMINA TUTTO
					return ENOMEM;
				}
				memset(desc, 0, sizeof(vm_desc_t));
				desc->pages = kmalloc(pages * sizeof(int));
				if(desc->pages==0)
				{
					#warning ELIMINA TUTTO
					return ENOMEM;
				}

				desc->num_pages = pages;
				desc->num_copies = 1;
				desc->type = VM_AREA_NORMAL;
				desc->spin.lock = 1;
				desc->mtx_allocate.lock = 1;

				vm_node_t *node = VmmAllocNode(task, region_addr, VM_MEM_ZERO | VM_MEM_USER | VM_MEM_READ | VM_MEM_WRITE, desc, 0);
				if(node == 0)
				{
					#warning ELIMINA TUTTO
					return ENOMEM;
				}
			}
		}
		else //RO Segment
		{
			int pages = PAGE_ALIGN_UP(pheaders[i].p_memsz) / PAGE_SIZE;

			//Inizializza l'user stack per l'utente inserendo un nuovo nodo nella memoria virtuale del task
			vm_desc_t *desc = kmalloc(sizeof(vm_desc_t));
			if(desc == 0)
			{
				#warning ELIMINA TUTTO
				return ENOMEM;
			}
			memset(desc, 0, sizeof(vm_desc_t));
			desc->pages = kmalloc(pages * sizeof(int));
			if(desc->pages==0)
			{
				#warning ELIMINA TUTTO
				return ENOMEM;
			}

			desc->num_pages = pages;
			desc->num_copies = 1;
			desc->type = VM_AREA_FILE;
			desc->spin.lock = 1;
			desc->mtx_allocate.lock = 1;
			desc->dest.file.file = elf_file;
			desc->dest.file.offset = 0;

			printk("addr: 0x%x\n", pheaders[i].p_vaddr);

			vm_node_t *node = VmmAllocNode(task, ROUND_DOWN_TO_PAGE(pheaders[i].p_vaddr), VM_MEM_ZERO | VM_MEM_USER | VM_MEM_READ, desc, 0);

			if(node == 0)
			{
				#warning ELIMINA TUTTO
				return ENOMEM;
			}
		}
	}

	kfree(pheaders);

	printk("END elf_exec\n");

	return ESUCCESS;
}
#endif
