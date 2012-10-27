OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)


STACK_FIQ_SIZE 	= 0x0400;
STACK_IRQ_SIZE 	= 0x0400;
STACK_ABT_SIZE 	= 0x0400;
STACK_UND_SIZE 	= 0x0400;
STACK_SRV_SIZE 	= 0x8000;


MEMORY
{
	rom (rx)	: org = 0x34000000, len = 0x02000000	/* 32 MB */
	ram (rwx)	: org = 0x36000000, len = 0x1a000000	/* 416 MB */
}

SECTIONS
{   
	.text :
	{
		. = ALIGN(8);
		PROVIDE (__text_start = .);
		.obj/arch/arm32/mach-smdkv210/start.o (.text)
		.obj/arch/arm32/mach-smdkv210/s5pv210-irom.o (.text)
		*(.text)
		*(.text.*)
		*(.init.text)
    	*(.exit.text)

    	. = ALIGN(8);
    	PROVIDE (__ksymtab_start = .);
		KEEP(*(.ksymtab.text))
		PROVIDE (__ksymtab_end = .);

    	. = ALIGN(8);	
		PROVIDE (__initcall_start = .);
		KEEP(*(.initcall_0.text))
		KEEP(*(.initcall_0s.text))
		KEEP(*(.initcall_1.text))
		KEEP(*(.initcall_1s.text))
		KEEP(*(.initcall_2.text))
		KEEP(*(.initcall_2s.text))
		KEEP(*(.initcall_3.text))
		KEEP(*(.initcall_3s.text))
		KEEP(*(.initcall_4.text))
		KEEP(*(.initcall_4s.text))
		KEEP(*(.initcall_5.text))
		KEEP(*(.initcall_5s.text))
		KEEP(*(.initcall_6.text))
		KEEP(*(.initcall_6s.text))
		KEEP(*(.initcall_7.text))
		KEEP(*(.initcall_7s.text))
		KEEP(*(.initcall_8.text))
		KEEP(*(.initcall_8s.text))
		PROVIDE (__initcall_end = .);
		
		. = ALIGN(8);
		PROVIDE (__exitcall_start = .);
		KEEP(*(.exitcall_8s.text))
		KEEP(*(.exitcall_8.text))
		KEEP(*(.exitcall_7s.text))
		KEEP(*(.exitcall_7.text))
		KEEP(*(.exitcall_6s.text))
		KEEP(*(.exitcall_6.text))
		KEEP(*(.exitcall_5s.text))
		KEEP(*(.exitcall_5.text))
		KEEP(*(.exitcall_4s.text))
		KEEP(*(.exitcall_4.text))
		KEEP(*(.exitcall_3s.text))
		KEEP(*(.exitcall_3.text))
		KEEP(*(.exitcall_2s.text))
		KEEP(*(.exitcall_2.text))
		KEEP(*(.exitcall_1s.text))
		KEEP(*(.exitcall_1.text))
		KEEP(*(.exitcall_0s.text))
		KEEP(*(.exitcall_0.text))
		PROVIDE (__exitcall_end = .);

		. = ALIGN(8);
		PROVIDE (__romdisk_start = .);
		KEEP(*(.romdisk))
		PROVIDE (__romdisk_end = .);

		. = ALIGN(8);
		*(.rodata);
		*(.rodata.*);

		. = ALIGN(8);
		*(.glue_7);
		*(.glue_7t);

		. = ALIGN(8);
		PROVIDE (__text_end = .);
	} > rom

	.data_shadow ALIGN(8) :
	{
		PROVIDE (__data_shadow_start = .);
		PROVIDE (__data_shadow_end = (. + SIZEOF (.data)) );
	} > rom

	.data : AT ( ADDR (.data_shadow) )
	{
		PROVIDE (__data_start = .);	
		*(.data)
		. = ALIGN(8);
  		PROVIDE (__data_end = .);		
	} > ram

	.bss ALIGN(8) (NOLOAD) :
	{
		PROVIDE (__bss_start = .);
		*(.bss)
		*(.bss.*)
		*(.sbss)
		*(COMMON)
		PROVIDE (__bss_end = .);
		
		. = ALIGN(8);
		PROVIDE (__heap_start = .);
		*(.heap)
		. = ALIGN(8);
		PROVIDE (__heap_end = .);
		
		. = ALIGN(8);
		PROVIDE (__stack_start = .);
		PROVIDE (__stack_fiq_start = .);
		. += STACK_FIQ_SIZE;
		PROVIDE (__stack_fiq_end = .);
		. = ALIGN(8);
		PROVIDE (__stack_irq_start = .);
		. += STACK_IRQ_SIZE;
		PROVIDE (__stack_irq_end = .);
		. = ALIGN(8);
		PROVIDE (__stack_abt_start = .);
		. += STACK_ABT_SIZE;
		PROVIDE (__stack_abt_end = .);
		. = ALIGN(8);
		PROVIDE (__stack_und_start = .);
		. += STACK_UND_SIZE;
		PROVIDE (__stack_und_end = .);
		. = ALIGN(8);
		PROVIDE (__stack_srv_start = .);
		. += STACK_SRV_SIZE;
		PROVIDE (__stack_srv_end = .);
		. = ALIGN(8);
		PROVIDE (__stack_end = .);
	} > ram

	/*
	 * Stabs debugging sections.
	 */
	.stab 0 : { *(.stab) }
	.stabstr 0 : { *(.stabstr) }
	.stab.excl 0 : { *(.stab.excl) }
	.stab.exclstr 0 : { *(.stab.exclstr) }
	.stab.index 0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment 0 : { *(.comment) }
	.debug_abbrev 0 : { *(.debug_abbrev) }
	.debug_info 0 : { *(.debug_info) }
	.debug_line 0 : { *(.debug_line) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	.debug_aranges 0 : { *(.debug_aranges) }
}
