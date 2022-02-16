OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

STACK_UND_SIZE = 0x40000 * 2;
STACK_ABT_SIZE = 0x40000 * 2;
STACK_IRQ_SIZE = 0x40000 * 2;
STACK_FIQ_SIZE = 0x40000 * 2;
STACK_SRV_SIZE = 0x100000 * 2;

MEMORY
{
	ram  : org = 0x40000000, len = 16M
	dma  : org = 0x41000000, len = 16M
	heap : org = 0x42000000, len = 96M
}

SECTIONS
{
	.text :
	{
		PROVIDE(__image_start = .);
		PROVIDE(__text_start = .);
		PROVIDE(__spl_start = .);
		.obj/arch/arm32/mach-t113s3/start.o (.text*)
		.obj/arch/arm32/lib/memcpy.o (.text*)
		.obj/arch/arm32/lib/memset.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-smp.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-jtag.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-uart.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-clock.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-dram.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-mmu.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-decompress.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-hash.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-verify.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-spinor.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-spinand.o (.text*)
		.obj/arch/arm32/mach-t113s3/sys-copyself.o (.text*)
		PROVIDE(__ddr_param_start = .);
		KEEP(*(.ddr.param))
		PROVIDE(__ddr_param_end = .);
		PROVIDE(__ddr_bin_start = .);
		KEEP(*(.ddr.bin))
		PROVIDE(__ddr_bin_end = .);
		PROVIDE(__spl_end = .);
		*(.text*)
		*(.init.text)
		*(.exit.text)
		*(.glue*)
		*(.note.gnu.build-id)
		PROVIDE(__text_end = .);
	} > ram
	PROVIDE(__spl_size = __spl_end - __spl_start);

	.ARM.exidx ALIGN(8) :
	{
		PROVIDE (__exidx_start = .);
		*(.ARM.exidx*)
		PROVIDE (__exidx_end = .);
	} > ram

	.ARM.extab ALIGN(8) :
	{
		PROVIDE (__extab_start = .);
		*(.ARM.extab*)
		PROVIDE (__extab_end = .);
	} > ram

	.initcall ALIGN(8) :
	{
		PROVIDE(__initcall_start = .);
		KEEP(*(.initcall_0.text))
		KEEP(*(.initcall_1.text))
		KEEP(*(.initcall_2.text))
		KEEP(*(.initcall_3.text))
		KEEP(*(.initcall_4.text))
		KEEP(*(.initcall_5.text))
		KEEP(*(.initcall_6.text))
		KEEP(*(.initcall_7.text))
		KEEP(*(.initcall_8.text))
		KEEP(*(.initcall_9.text))
		PROVIDE(__initcall_end = .);
	} > ram

	.exitcall ALIGN(8) :
	{
		PROVIDE(__exitcall_start = .);
		KEEP(*(.exitcall_9.text))
		KEEP(*(.exitcall_8.text))
		KEEP(*(.exitcall_7.text))
		KEEP(*(.exitcall_6.text))
		KEEP(*(.exitcall_5.text))
		KEEP(*(.exitcall_4.text))
		KEEP(*(.exitcall_3.text))
		KEEP(*(.exitcall_2.text))
		KEEP(*(.exitcall_1.text))
		KEEP(*(.exitcall_0.text))
		PROVIDE(__exitcall_end = .);
	} > ram

	.ksymtab ALIGN(16) :
	{
		PROVIDE(__ksymtab_start = .);
		KEEP(*(.ksymtab.text))
		PROVIDE(__ksymtab_end = .);
	} > ram

	.romdisk ALIGN(8) :
	{
		PROVIDE(__romdisk_start = .);
		KEEP(*(.romdisk))
		PROVIDE(__romdisk_end = .);
	} > ram

	.rodata ALIGN(8) :
	{
		PROVIDE(__rodata_start = .);
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
		PROVIDE(__rodata_end = .);
	} > ram

	.data ALIGN(8) :
	{
		PROVIDE(__data_start = .);
		*(.data*)
		. = ALIGN(8);
  		PROVIDE(__data_end = .);
		PROVIDE(__image_end = .);
	} > ram

	.bss ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__bss_start = .);
		*(.bss*)
		*(.sbss*)
		*(COMMON)
		. = ALIGN(8);
		PROVIDE(__bss_end = .);
	} > ram

	.mmu ALIGN(0x4000) (NOLOAD) :
	{
		PROVIDE(__mmu_start = .);
		. += 4096;
		PROVIDE(__mmu_end = .);
	} > ram

	.stack ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__stack_start = .);
		PROVIDE(__stack_und_start = .);
		. += STACK_UND_SIZE;
		PROVIDE(__stack_und_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_abt_start = .);
		. += STACK_ABT_SIZE;
		PROVIDE(__stack_abt_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_irq_start = .);
		. += STACK_IRQ_SIZE;
		PROVIDE(__stack_irq_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_fiq_start = .);
		. += STACK_FIQ_SIZE;
		PROVIDE(__stack_fiq_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_srv_start = .);
		. += STACK_SRV_SIZE;
		PROVIDE(__stack_srv_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_end = .);
	} > ram

	.dma ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__dma_start = ORIGIN(dma));
		PROVIDE(__dma_end = ORIGIN(dma) + LENGTH(dma));
	} > dma

	.heap ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__heap_start = ORIGIN(heap));
		PROVIDE(__heap_end = ORIGIN(heap) + LENGTH(heap));
	} > heap

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
