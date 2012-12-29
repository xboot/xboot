#ifndef __ELF_H__
#define __ELF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <elf/elf32.h>
#include <elf/elf64.h>


#define	ELFMAG			"\177ELF"
#define	SELFMAG			(4)

#define EI_CLASS		(4)				/* File class byte index */
enum {
	ELFCLASSNONE		= 0,			/* Invalid class */
	ELFCLASS32			= 1,			/* 32-bit objects */
	ELFCLASS64			= 2,			/* 64-bit objects */
};

#define EI_DATA			(5)				/* Data encoding byte index */
enum {
	ELFDATANONE			= 0,			/* Invalid data encoding */
	ELFDATA2LSB			= 1,			/* 2's complement, little endian */
	ELFDATA2MSB			= 2,			/* 2's complement, big endian */
};

/*
 * Legal values for e_type (object file type)
 */
enum {
	ET_NONE 			= 0,			/* No file type */
	ET_REL 				= 1,			/* Relocatable file */
	ET_EXEC 			= 2,			/* Executable file */
	ET_DYN 				= 3,			/* Shared object file */
	ET_CORE 			= 4,			/* Core file */
	ET_NUM 				= 5,			/* Number of defined types */
	ET_LOOS 			= 0xfe00,		/* OS-specific range start */
	ET_HIOS 			= 0xfeff,		/* OS-specific range end */
	ET_LOPROC 			= 0xff00,		/* Processor-specific range start */
	ET_HIPROC 			= 0xffff,		/* Processor-specific range end */
};

/*
 * Legal values for e_machine (architecture)
 */
enum {
	EM_NONE 			= 0, 			/* No machine */
	EM_M32 				= 1, 			/* AT&T WE 32100 */
	EM_SPARC			= 2, 			/* SUN SPARC */
	EM_386				= 3, 			/* Intel 80386 */
	EM_68K 				= 4, 			/* Motorola m68k family */
	EM_88K 				= 5, 			/* Motorola m88k family */
	EM_860 				= 7, 			/* Intel 80860 */
	EM_MIPS 			= 8, 			/* MIPS R3000 big-endian */
	EM_S370 			= 9, 			/* IBM System/370 */
	EM_MIPS_RS3_LE		= 10, 			/* MIPS R3000 little-endian */

	EM_PARISC			= 15, 			/* HPPA */
	EM_VPP500			= 17,			/* Fujitsu VPP500 */
	EM_SPARC32PLUS		= 18, 			/* Sun's "v8plus" */
	EM_960				= 19,			/* Intel 80960 */
	EM_PPC				= 20, 			/* PowerPC */
	EM_PPC64			= 21, 			/* PowerPC 64-bit */
	EM_S390				= 22,			/* IBM S390 */

	EM_V800				= 36, 			/* NEC V800 series */
	EM_FR20				= 37, 			/* Fujitsu FR20 */
	EM_RH32 			= 38, 			/* TRW RH-32 */
	EM_RCE 				= 39, 			/* Motorola RCE */
	EM_ARM 				= 40,			/* ARM */
	EM_FAKE_ALPHA 		= 41, 			/* Digital Alpha */
	EM_SH 				= 42, 			/* Hitachi SH */
	EM_SPARCV9 			= 43,			/* SPARC v9 64-bit */
	EM_TRICORE 			= 44, 			/* Siemens Tricore */
	EM_ARC 				= 45,			/* Argonaut RISC Core */
	EM_H8_300 			= 46,			/* Hitachi H8/300 */
	EM_H8_300H 			= 47,			/* Hitachi H8/300H */
	EM_H8S 				= 48,			/* Hitachi H8S */
	EM_H8_500 			= 49,			/* Hitachi H8/500 */
	EM_IA_64 			= 50,			/* Intel Merced */
	EM_MIPS_X 			= 51,			/* Stanford MIPS-X */
	EM_COLDFIRE 		= 52, 			/* Motorola Coldfire */
	EM_68HC12 			= 53,			/* Motorola M68HC12 */
	EM_MMA 				= 54, 			/* Fujitsu MMA Multimedia Accelerator*/
	EM_PCP 				= 55,			/* Siemens PCP */
	EM_NCPU 			= 56, 			/* Sony nCPU embeeded RISC */
	EM_NDR1 			= 57, 			/* Denso NDR1 microprocessor */
	EM_STARCORE 		= 58, 			/* Motorola Start*Core processor */
	EM_ME16 			= 59, 			/* Toyota ME16 processor */
	EM_ST100 			= 60,			/* STMicroelectronic ST100 processor */
	EM_TINYJ 			= 61,			/* Advanced Logic Corp. Tinyj emb.fam*/
	EM_X86_64 			= 62,			/* AMD x86-64 architecture */
	EM_PDSP 			= 63, 			/* Sony DSP Processor */

	EM_FX66 			= 66, 			/* Siemens FX66 microcontroller */
	EM_ST9PLUS 			= 67,			/* STMicroelectronics ST9+ 8/16 mc */
	EM_ST7 				= 68, 			/* STmicroelectronics ST7 8 bit mc */
	EM_68HC16 			= 69,			/* Motorola MC68HC16 microcontroller */
	EM_68HC11 			= 70, 			/* Motorola MC68HC11 microcontroller */
	EM_68HC08 			= 71,			/* Motorola MC68HC08 microcontroller */
	EM_68HC05 			= 72, 			/* Motorola MC68HC05 microcontroller */
	EM_SVX 				= 73,			/* Silicon Graphics SVx */
	EM_ST19 			= 74, 			/* STMicroelectronics ST19 8 bit mc */
	EM_VAX 				= 75,			/* Digital VAX */
	EM_CRIS 			= 76, 			/* Axis Communications 32-bit embedded processor */
	EM_JAVELIN 			= 77,			/* Infineon Technologies 32-bit embedded processor */
	EM_FIREPATH 		= 78, 			/* Element 14 64-bit DSP Processor */
	EM_ZSP 				= 79,			/* LSI Logic 16-bit DSP Processor */
	EM_MMIX 			= 80, 			/* Donald Knuth's educational 64-bit processor */
	EM_HUANY 			= 81,			/* Harvard University machine-independent object files */
	EM_PRISM 			= 82,			/* SiTera Prism */
	EM_AVR 				= 83,			/* Atmel AVR 8-bit microcontroller */
	EM_FR30 			= 84, 			/* Fujitsu FR30 */
	EM_D10V 			= 85, 			/* Mitsubishi D10V */
	EM_D30V 			= 86, 			/* Mitsubishi D30V */
	EM_V850 			= 87, 			/* NEC v850 */
	EM_M32R 			= 88, 			/* Mitsubishi M32R */
	EM_MN10300 			= 89,			/* Matsushita MN10300 */
	EM_MN10200 			= 90,			/* Matsushita MN10200 */
	EM_PJ 				= 91,			/* picoJava */
	EM_OPENRISC 		= 92, 			/* OpenRISC 32-bit embedded processor */
	EM_ARC_A5 			= 93,			/* ARC Cores Tangent-A5 */
	EM_XTENSA 			= 94,			/* Tensilica Xtensa Architecture */
	EM_NUM 				= 95,
};

/*
 * Legal values for sh_type (section type)
 */
enum {
	SHT_NULL 			= 0, 			/* Section header table entry unused */
	SHT_PROGBITS 		= 1, 			/* Program data */
	SHT_SYMTAB 			= 2, 			/* Symbol table */
	SHT_STRTAB 			= 3, 			/* String table */
	SHT_RELA 			= 4, 			/* Relocation entries with addends */
	SHT_HASH 			= 5, 			/* Symbol hash table */
	SHT_DYNAMIC 		= 6, 			/* Dynamic linking information */
	SHT_NOTE 			= 7, 			/* Notes */
	SHT_NOBITS 			= 8, 			/* Program space with no data (bss) */
	SHT_REL 			= 9, 			/* Relocation entries, no addends */
	SHT_SHLIB 			= 10, 			/* Reserved */
	SHT_DYNSYM 			= 11, 			/* Dynamic linker symbol table */
	SHT_INIT_ARRAY 		= 14, 			/* Array of constructors */
	SHT_FINI_ARRAY 		= 15, 			/* Array of destructors */
	SHT_PREINIT_ARRAY	= 16,			/* Array of pre-constructors */
	SHT_GROUP 			= 17, 			/* Section group */
	SHT_SYMTAB_SHNDX 	= 18, 			/* Extended section indeces */
	SHT_NUM 			= 19, 			/* Number of defined types.  */
	SHT_LOOS 			= 0x60000000,	/* Start OS-specific.  */
	SHT_GNU_ATTRIBUTES 	= 0x6ffffff5,	/* Object attributes.  */
	SHT_GNU_HASH 		= 0x6ffffff6, 	/* GNU-style hash table.  */
	SHT_GNU_LIBLIST 	= 0x6ffffff7,	/* Prelink library list */
	SHT_CHECKSUM 		= 0x6ffffff8, 	/* Checksum for DSO content.  */
	SHT_LOSUNW 			= 0x6ffffffa,	/* Sun-specific low bound.  */
	SHT_SUNW_move 		= 0x6ffffffa,
	SHT_SUNW_COMDAT 	= 0x6ffffffb,
	SHT_SUNW_syminfo 	= 0x6ffffffc,
	SHT_GNU_verdef 		= 0x6ffffffd,	/* Version definition section.  */
	SHT_GNU_verneed 	= 0x6ffffffe,	/* Version needs section.  */
	SHT_GNU_versym 		= 0x6fffffff,	/* Version symbol table.  */
	SHT_HISUNW 			= 0x6fffffff,	/* Sun-specific high bound.  */
	SHT_HIOS 			= 0x6fffffff, 	/* End OS-specific type */
	SHT_LOPROC 			= 0x70000000,	/* Start of processor-specific */
	SHT_HIPROC 			= 0x7fffffff, 	/* End of processor-specific */
	SHT_LOUSER			= 0x80000000,	/* Start of application-specific */
	SHT_HIUSER 			= 0x8fffffff, 	/* End of application-specific */
};

/*
 * Legal values for sh_flags (section flags)
 */
enum {
	SHF_WRITE 			= (1 << 0), 	/* Writable */
	SHF_ALLOC 			= (1 << 1), 	/* Occupies memory during execution */
	SHF_EXECINSTR 		= (1 << 2), 	/* Executable */
	SHF_MERGE 			= (1 << 4),		/* Might be merged */
	SHF_STRINGS 		= (1 << 5), 	/* Contains nul-terminated strings */
	SHF_INFO_LINK 		= (1 << 6),		/* 'sh_info' contains SHT index */
	SHF_LINK_ORDER 		= (1 << 7),		/* Preserve order after combining */
	SHF_OS_NONCONFORMING= (1 << 8), 	/* Non-standard OS specific handling required */
	SHF_GROUP 			= (1 << 9), 	/* Section is member of a group.  */
	SHF_TLS 			= (1 << 10),	/* Section hold thread-local data.  */
	SHF_MASKOS 			= 0x0ff00000,	/* OS-specific.  */
	SHF_MASKPROC 		= 0xf0000000, 	/* Processor-specific */
	SHF_ORDERED 		= (1 << 30),	/* Special ordering requirement(Solaris) */
	SHF_EXCLUDE 		= (1 << 31), 	/* Section is excluded unless referenced or allocated(Solaris) */
};

/*
 * Legal values for p_type (segment type)
 */
enum {
	PT_NULL 			= 0,			/* Program header table entry unused */
	PT_LOAD 			= 1, 			/* Loadable program segment */
	PT_DYNAMIC 			= 2, 			/* Dynamic linking information */
	PT_INTERP 			= 3, 			/* Program interpreter */
	PT_NOTE 			= 4, 			/* Auxiliary information */
	PT_SHLIB 			= 5, 			/* Reserved */
	PT_PHDR 			= 6, 			/* Entry for header table itself */
	PT_TLS 				= 7, 			/* Thread-local storage segment */
	PT_NUM 				= 8, 			/* Number of defined types */
	PT_LOOS 			= 0x60000000,	/* Start of OS-specific */
	PT_GNU_EH_FRAME 	= 0x6474e550,	/* GCC .eh_frame_hdr segment */
	PT_GNU_STACK 		= 0x6474e551,	/* Indicates stack executability */
	PT_GNU_RELRO 		= 0x6474e552,	/* Read-only after relocation */
	PT_LOSUNW 			= 0x6ffffffa,
	PT_SUNWBSS 			= 0x6ffffffa,	/* Sun Specific segment */
	PT_SUNWSTACK 		= 0x6ffffffb,	/* Stack segment */
	PT_HISUNW 			= 0x6fffffff,
	PT_HIOS 			= 0x6fffffff,	/* End of OS-specific */
	PT_LOPROC 			= 0x70000000,	/* Start of processor-specific */
	PT_HIPROC 			= 0x7fffffff,	/* End of processor-specific */
};

/*
 * Legal values for d_tag (dynamic entry type)
 */
enum {
	DT_NULL 			= 0, 			/* Marks end of dynamic section */
	DT_NEEDED 			= 1, 			/* Name of needed library */
	DT_PLTRELSZ 		= 2, 			/* Size in bytes of PLT relocs */
	DT_PLTGOT 			= 3, 			/* Processor defined value */
	DT_HASH 			= 4, 			/* Address of symbol hash table */
	DT_STRTAB 			= 5, 			/* Address of string table */
	DT_SYMTAB 			= 6, 			/* Address of symbol table */
	DT_RELA 			= 7, 			/* Address of Rela relocs */
	DT_RELASZ 			= 8, 			/* Total size of Rela relocs */
	DT_RELAENT 			= 9, 			/* Size of one Rela reloc */
	DT_STRSZ 			= 10, 			/* Size of string table */
	DT_SYMENT 			= 11, 			/* Size of one symbol table entry */
	DT_INIT 			= 12, 			/* Address of init function */
	DT_FINI 			= 13, 			/* Address of termination function */
	DT_SONAME 			= 14, 			/* Name of shared object */
	DT_RPATH 			= 15, 			/* Library search path (deprecated) */
	DT_SYMBOLIC 		= 16, 			/* Start symbol search here */
	DT_REL 				= 17, 			/* Address of Rel relocs */
	DT_RELSZ 			= 18,			/* Total size of Rel relocs */
	DT_RELENT 			= 19, 			/* Size of one Rel reloc */
	DT_PLTREL 			= 20, 			/* Type of reloc in PLT */
	DT_DEBUG 			= 21, 			/* For debugging; unspecified */
	DT_TEXTREL 			= 22, 			/* Reloc might modify .text */
	DT_JMPREL 			= 23, 			/* Address of PLT relocs */
	DT_BIND_NOW 		= 24, 			/* Process relocations of object */
	DT_INIT_ARRAY 		= 25, 			/* Array with addresses of init fct */
	DT_FINI_ARRAY 		= 26, 			/* Array with addresses of fini fct */
	DT_INIT_ARRAYSZ 	= 27, 			/* Size in bytes of DT_INIT_ARRAY */
	DT_FINI_ARRAYSZ 	= 28, 			/* Size in bytes of DT_FINI_ARRAY */
	DT_RUNPATH 			= 29, 			/* Library search path */
	DT_FLAGS 			= 30, 			/* Flags for the object being loaded */
	DT_ENCODING 		= 32, 			/* Start of encoded range */
	DT_PREINIT_ARRAY 	= 32, 			/* Array with addresses of preinit fct*/
	DT_PREINIT_ARRAYSZ	= 33,			/* size in bytes of DT_PREINIT_ARRAY */
	DT_NUM 				= 34,			/* Number used */
	DT_LOOS 			= 0x6000000d,	/* Start of OS-specific */
	DT_HIOS 			= 0x6ffff000,	/* End of OS-specific */
	DT_LOPROC 			= 0x70000000,	/* Start of processor-specific */
	DT_HIPROC 			= 0x7fffffff,	/* End of processor-specific */
	DT_PROCNUM 			= 0x35,			/* Most used by any processor */
};

/*
 * Legal values for p_flags (segment flags)
 */
enum {
	PF_X 				= (1 << 0),		/* Segment is executable */
	PF_W 				= (1 << 1),		/* Segment is writable */
	PF_R 				= (1 << 2),		/* Segment is readable */
	PF_MASKOS 			= 0x0ff00000,	/* OS-specific */
	PF_MASKPROC 		= 0xf0000000, 	/* Processor-specific */
};

/*
 * How to extract and insert information held in the r_info field
 */
#define ELF32_R_SYM(val)				((val) >> 8)
#define ELF32_R_TYPE(val)				((val) & 0xff)
#define ELF32_R_INFO(sym, type)			(((sym) << 8) + ((type) & 0xff))

#define ELF64_R_SYM(i)					((i) >> 32)
#define ELF64_R_TYPE(i)					((i) & 0xffffffff)
#define ELF64_R_INFO(sym,type)			((((elf64_xword) (sym)) << 32) + (type))

#ifdef __cplusplus
}
#endif

#endif /* __ELF_H__ */
