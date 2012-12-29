#ifndef __ELF32_H__
#define __ELF32_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <elf/elf.h>

/* Type for a 16-bit quantity */
typedef u16_t		elf32_half;

/* Types for signed and unsigned 32-bit quantities */
typedef u32_t		elf32_word;
typedef s32_t		elf32_sword;

/* Types for signed and unsigned 64-bit quantities */
typedef u64_t		elf32_xword;
typedef s64_t		elf32_sxword;

/* Type of addresses */
typedef u32_t		elf32_addr;

/* Type of file offsets */
typedef u32_t		elf32_off;

/* Type for section indices, which are 16-bit quantities */
typedef u16_t		elf32_section;

/* Type for version symbol information */
typedef elf32_half	elf32_versym;

/*
 * The ELF file header
 */
struct elf32_ehdr {
	unsigned char	e_ident[16];	/* Magic number and other info */
	elf32_half		e_type;			/* Object file type */
	elf32_half		e_machine;		/* Architecture */
	elf32_word		e_version;		/* Object file version */
	elf32_addr		e_entry;		/* Entry point virtual address */
	elf32_off		e_phoff;		/* Program header table file offset */
	elf32_off		e_shoff;		/* Section header table file offset */
	elf32_word		e_flags;		/* Processor-specific flags */
	elf32_half		e_ehsize;		/* ELF header size in bytes */
	elf32_half		e_phentsize;	/* Program header table entry size */
	elf32_half		e_phnum;		/* Program header table entry count */
	elf32_half		e_shentsize;	/* Section header table entry size */
	elf32_half		e_shnum;		/* Section header table entry count */
	elf32_half		e_shstrndx;		/* Section header string table index */
};

/*
 * Section header
 */
struct elf32_shdr {
	elf32_word		sh_name;		/* Section name (string tbl index) */
	elf32_word 		sh_type; 		/* Section type */
	elf32_word 		sh_flags; 		/* Section flags */
	elf32_addr 		sh_addr; 		/* Section virtual addr at execution */
	elf32_off 		sh_offset; 		/* Section file offset */
	elf32_word 		sh_size; 		/* Section size in bytes */
	elf32_word 		sh_link; 		/* Link to another section */
	elf32_word 		sh_info; 		/* Additional section information */
	elf32_word 		sh_addralign; 	/* Section alignment */
	elf32_word 		sh_entsize; 	/* Entry size if section holds table */
};

/*
 * Symbol table entry
 */
struct elf32_sym {
	elf32_word		st_name;		/* Symbol name (string tbl index) */
	elf32_addr		st_value;		/* Symbol value */
	elf32_word		st_size;		/* Symbol size */
	unsigned char	st_info;		/* Symbol type and binding */
	unsigned char	st_other;		/* Symbol visibility */
	elf32_section	st_shndx;		/* Section index */
};

/*
 * The syminfo section if available contains additional information about
 * every dynamic symbol
 */
struct elf32_syminfo
{
	elf32_half		si_boundto;		/* Direct bindings, symbol bound to */
	elf32_half		si_flags;		/* Per symbol flags */
};

/*
 * Relocation table entry without addend (in section of type SHT_REL)
 */
struct elf32_rel {
	elf32_addr		r_offset;		/* Address */
	elf32_word		r_info;			/* Relocation type and symbol index */
};

/*
 * Relocation table entry with addend (in section of type SHT_RELA)
 */
struct elf32_rela {
	elf32_addr		r_offset;		/* Address */
	elf32_word		r_info;			/* Relocation type and symbol index */
	elf32_sword		r_addend;		/* Addend */
};

/*
 * Dynamic section entry
 */
struct elf32_dyn {
	elf32_sword		d_tag;			/* Dynamic entry type */
	union {
		elf32_word	d_val;			/* Integer value */
		elf32_addr	d_ptr;			/* Address value */
	} d_un;
};

/*
 * Program segment header
 */
struct elf32_phdr
{
	elf32_word		p_type;			/* Segment type */
	elf32_off		p_offset;		/* Segment file offset */
	elf32_addr		p_vaddr;		/* Segment virtual address */
	elf32_addr		p_paddr;		/* Segment physical address */
	elf32_word		p_filesz;		/* Segment size in file */
	elf32_word		p_memsz;		/* Segment size in memory */
	elf32_word		p_flags;		/* Segment flags */
	elf32_word		p_align;		/* Segment alignment */
};

#ifdef __cplusplus
}
#endif

#endif /* __ELF32_H__ */
