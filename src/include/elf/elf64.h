#ifndef __ELF64_H__
#define __ELF64_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <elf/elf.h>

/* Type for a 16-bit quantity */
typedef u16_t		elf64_half;

/* Types for signed and unsigned 32-bit quantities */
typedef u32_t		elf64_word;
typedef s32_t		elf64_sword;

/* Types for signed and unsigned 64-bit quantities */
typedef u64_t		elf64_xword;
typedef s64_t		elf64_sxword;

/* Type of addresses */
typedef u64_t		elf64_addr;

/* Type of file offsets */
typedef u64_t		elf64_off;

/* Type for section indices, which are 16-bit quantities */
typedef u16_t		elf64_section;

/* Type for version symbol information */
typedef elf64_half	elf64_versym;

/*
 * The ELF file header
 */
struct elf64_ehdr {
	unsigned char	e_ident[16];	/* Magic number and other info */
	elf64_half		e_type;			/* Object file type */
	elf64_half		e_machine;		/* Architecture */
	elf64_word		e_version;		/* Object file version */
	elf64_addr		e_entry;		/* Entry point virtual address */
	elf64_off		e_phoff;		/* Program header table file offset */
	elf64_off		e_shoff;		/* Section header table file offset */
	elf64_word		e_flags;		/* Processor-specific flags */
	elf64_half		e_ehsize;		/* ELF header size in bytes */
	elf64_half		e_phentsize;	/* Program header table entry size */
	elf64_half		e_phnum;		/* Program header table entry count */
	elf64_half		e_shentsize;	/* Section header table entry size */
	elf64_half		e_shnum;		/* Section header table entry count */
	elf64_half		e_shstrndx;		/* Section header string table index */
};

/*
 * Section header
 */
struct elf64_shdr {
	elf64_word		sh_name;		/* Section name (string tbl index) */
	elf64_word 		sh_type;		/* Section type */
	elf64_xword 	sh_flags;		/* Section flags */
	elf64_addr 		sh_addr;		/* Section virtual addr at execution */
	elf64_off 		sh_offset;		/* Section file offset */
	elf64_xword 	sh_size;		/* Section size in bytes */
	elf64_word 		sh_link;		/* Link to another section */
	elf64_word 		sh_info;		/* Additional section information */
	elf64_xword 	sh_addralign;	/* Section alignment */
	elf64_xword 	sh_entsize;		/* Entry size if section holds table */
};

/*
 * Symbol table entry
 */
struct Elf64_Sym {
	elf64_word		st_name;		/* Symbol name (string tbl index) */
	unsigned char	st_info;		/* Symbol type and binding */
	unsigned char	st_other;		/* Symbol visibility */
	elf64_section	st_shndx;		/* Section index */
	elf64_addr		st_value;		/* Symbol value */
	elf64_xword		st_size;		/* Symbol size */
};

/*
 * The syminfo section if available contains additional information about
 * every dynamic symbol
 */
struct elf64_syminfo {
	elf64_half		si_boundto;		/* Direct bindings, symbol bound to */
	elf64_half		si_flags;		/* Per symbol flags */
};

/*
 * The following, at least, is used on Sparc v9, MIPS, and Alpha
 */
struct Elf64_Rel {
	elf64_addr		r_offset;		/* Address */
	elf64_xword		r_info;			/* Relocation type and symbol index */
};

/*
 * Relocation table entry with addend (in section of type SHT_RELA)
 */
struct elf64_rela {
	elf64_addr		r_offset;		/* Address */
	elf64_xword		r_info;			/* Relocation type and symbol index */
	elf64_sxword	r_addend;		/* Addend */
};

/*
 * Dynamic section entry
 */
struct elf64_dyn {
	elf64_sxword	d_tag;			/* Dynamic entry type */
	union {
		elf64_xword	d_val;			/* Integer value */
		elf64_addr	d_ptr;			/* Address value */
	} d_un;
};

/*
 * Program segment header
 */
struct elf64_phdr {
	elf64_word		p_type;			/* Segment type */
	elf64_word		p_flags;		/* Segment flags */
	elf64_off		p_offset;		/* Segment file offset */
	elf64_addr		p_vaddr;		/* Segment virtual address */
	elf64_addr		p_paddr;		/* Segment physical address */
	elf64_xword		p_filesz;		/* Segment size in file */
	elf64_xword		p_memsz;		/* Segment size in memory */
	elf64_xword		p_align;		/* Segment alignment */
};

#ifdef __cplusplus
}
#endif

#endif /* __ELF64_H__ */
