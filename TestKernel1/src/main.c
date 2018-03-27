#include <arch/ia32/cpu/cpu.h>
#include <arch/ia32/cpu/tables.h>
#include <arch/ia32/multiboot/multiboot.h>

// TODO: refactor threading and displays

void kernel_main(struct multiboot_info multiboot)
{
	//asm("movl %%esp, %0" : "=r"(esp));

	init_cpu();
	
	puts(1, "term 1\n");
	puts(2, "term 2\n");
	puts(3, "term 3\n");

	// int x = sizeof(gdt_ptr_t);
	// puts(display.tmp_terminal_num, itoa(x));
	
	asm("int3");
	
	for (;;)
		;
	
	asm("hlt");
}

