#include <cpu/tables.h>
#include <cpu/multiboot.h>
#include <display.h>



extern display_t display;

void irq_handler(registers_t regs)
{
	if (regs.int_no != 32 && regs.int_no != 33)
	{
		puts(display.tmp_terminal_num, "recieved interrupt: ");
		puts(display.tmp_terminal_num, itoa(regs.int_no));
		puts(display.tmp_terminal_num, "\n");
	}


	if (regs.int_no >= 40)
	{
		// Send reset signal to slave.
		outportb(0xA0, 0x20);
	}
	outportb(0x20, 0x20);

	if (regs.int_no == 33)
	{
		byte x = inportb(0x60);
		if (x == 65)//F7
		{
			change_terminal((display.tmp_terminal_num + 1) % NUM_TERMINALS);
			puts(display.tmp_terminal_num, "Other term");
			puts(display.tmp_terminal_num, "\n");
		}
		else if (x < 128)
		{
			puts(display.tmp_terminal_num, "recieved interrupt: ");
			puts(display.tmp_terminal_num, itoa(regs.int_no));
			puts(display.tmp_terminal_num, "  You pressed key with scancode ");
			puts(display.tmp_terminal_num, utoa(x));
			puts(display.tmp_terminal_num, "\n");
		}
	}
}

extern gdt_ptr_t init_gdt();
extern idt_ptr_t init_idt();

idt_ptr_t idt;
gdt_ptr_t gdt;

terminal_t terminals[NUM_TERMINALS];

void kernel_main(struct multiboot_info multiboot)
{
	init_display();
	int x = sizeof(gdt_ptr_t);
	puts(display.tmp_terminal_num, itoa(x));

	
	clear(display.tmp_terminal_num);
	gdt = init_gdt();
	idt = init_idt();
	
	
	puts(display.tmp_terminal_num, "Hello world from my custom kernel! ;)\n");
	asm("int3");

	/*puts(1, "msg on term 1");
	puts(2, "msg on term 2");
	puts(3, "msg on term 3");
	puts(4, "msg on term 4");*/
	
	for (;;)
		;
	
	asm("hlt");
}





