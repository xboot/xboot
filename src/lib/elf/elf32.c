/*
 * elf/elf.c
 */

#include <elf/elf32.h>

bool_t elf32_is_valid(void * elf)
{
	struct elf32_ehdr * e = (struct elf32_ehdr *)elf;

	if(!e)
		return FALSE;

	if(memcmp(e->e_ident, ELFMAG, SELFMAG) != 0)
		return FALSE;

	if(e->e_ident[EI_CLASS] != ELFCLASS32)
		return FALSE;

	return TRUE;
}

static char * elf32_get_section_name_by_id(void * elf, u32_t id)
{
	struct elf32_ehdr * e = (struct elf32_ehdr *)elf;
	struct elf32_shdr * s = (struct elf32_shdr *)(elf + e->e_shoff);

	if(id >= e->e_shnum)
		return NULL;

	return (char *)(elf + s[e->e_shstrndx].sh_offset + s[id].sh_name);
}

struct elf32_shdr * elf_get_section_by_id(void * elf, u32_t id)
{
	struct elf32_ehdr * e = (struct elf32_ehdr *)elf;
	struct elf32_shdr * s = (struct elf32_shdr *)(elf + e->e_shoff);

	if(id >= e->e_shnum)
		return NULL;

	return &s[id];
}

struct elf32_shdr * elf_get_section_by_name(void * elf, char * name)
{
	struct elf32_ehdr * e = (struct elf32_ehdr *)elf;
	u32_t id;

	for(id = 0; id < e->e_shnum; id++)
	{
		if(strcmp(elf32_get_section_name_by_id(elf, id), name) == 0)
			return elf_get_section_by_id(elf, id);
	}

	return NULL;
}

struct elf32_sym * elf_get_symbol_by_id(void * elf, u32_t id)
{
	struct elf32_shdr * sect = elf_get_section_by_name(elf, ".symtab");
	struct elf32_sym * symtab = (struct elf32_sym *)(elf + sect->sh_offset);

	if(id * sizeof(struct elf32_sym) > sect->sh_size)
		return NULL;

	return &symtab[id];
}

struct elf32_sym * elf_get_symbol_by_name(void * elf, char * name)
{
	struct elf32_shdr * sect = elf_get_section_by_name(elf, ".symtab");
	struct elf32_shdr * strtab = elf_get_section_by_name(elf, ".strtab");
	struct elf32_sym * symtab = elf + sect->sh_offset;
	u32_t size = sect->sh_size / sizeof(struct elf32_sym);
	u32_t i;

	for(i = 0; i < size; i++)
	{
		if(symtab[i].st_name == 0)
			continue;

		if(strcmp((char *)(elf + strtab->sh_offset + symtab[i].st_name), name) == 0)
			return &symtab[i];
	}

	return NULL;
}
