#include <xboot.h>
#include <virt/reg-uart.h>

extern u8_t	__text_start[];
extern u8_t __text_end[];
extern u8_t __data_shadow_start[];
extern u8_t __data_shadow_end[];
extern u8_t __data_start[];
extern u8_t __data_end[];
extern u8_t __bss_start[];
extern u8_t __bss_end[];
extern u8_t __stack_start[];
extern u8_t __stack_end[];

void debug(char c)
{
	while( (read8(phys_to_virt(VIRT_UART0_BASE + UART_FR)) & UART_FR_TXFF) );
	write8(phys_to_virt(VIRT_UART0_BASE + UART_DATA), c);
}

void copy_shadow(void)
{
	u64_t size;

	size = __data_shadow_end - __data_shadow_start;
	memcpy(__data_start, __data_shadow_start, size);
}

/*void clear_bss(void)
{
	u64_t size;

	size = __bss_end - __bss_start;
	memset(__bss_start, 0, size);
}*/
