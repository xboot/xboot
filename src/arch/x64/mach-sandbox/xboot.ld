SECTIONS
{
	. = ALIGN(8);
	__init : { KEEP(*(.init.text)) }

	. = ALIGN(8);
	__exit : { KEEP(*(.exit.text)) }

    . = ALIGN(8);
    PROVIDE (__ksymtab_start = .);
	__ksymtab : { KEEP(*(.ksymtab.text)) }
	PROVIDE (__ksymtab_end = .);

    . = ALIGN(8);	
	PROVIDE (__initcall_start = .);
	__initcall :
	{
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
	}
	PROVIDE (__initcall_end = .);

	. = ALIGN(8);
	PROVIDE (__exitcall_start = .);
	__exitcall :
	{
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
	}
	PROVIDE (__exitcall_end = .);

	. = ALIGN(8);
	PROVIDE (__romdisk_start = .);
	__romdisk : { KEEP(*(.romdisk)) }
	PROVIDE (__romdisk_end = .);
}
INSERT BEFORE .rodata;
