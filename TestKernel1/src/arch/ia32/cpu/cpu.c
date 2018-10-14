#include <arch/ia32/cpu/cpu.h>

extern display_t display;

terminal_t terminals[NUM_TERMINALS];

void irq_handler(registers_t* regs)
{
	char buff[12];
	if (regs->int_no == 32)
	{
		change_thread(&regs);
	}
	else if (regs->int_no != 32 && regs->int_no != 33)
	{
		puts(display.tmp_terminal_num, "recieved interrupt: ");
		puts(display.tmp_terminal_num, itoa(regs->int_no, buff));
		puts(display.tmp_terminal_num, "\n");
	}
	

	if (regs->int_no >= 40)
	{
		// Send reset signal to slave.
		outportb(0xA0, 0x20);
	}
	outportb(0x20, 0x20);

	if (regs->int_no == 33)
	{
		byte x = inportb(0x60);
		if (x == 65)//F7
		{
			change_terminal((display.tmp_terminal_num + 1) % NUM_TERMINALS);
		}
		else if (x < 128)
		{
			puts(display.tmp_terminal_num, "recieved interrupt: ");
			puts(display.tmp_terminal_num, itoa(regs->int_no, buff));
			puts(display.tmp_terminal_num, "  You pressed key with scancode ");
			puts(display.tmp_terminal_num, utoa(x, buff));
			puts(display.tmp_terminal_num, "\n");
		}
	}
}

extern gdt_ptr_t init_gdt();
extern idt_ptr_t init_idt();

idt_ptr_t idt;
gdt_ptr_t gdt;


void init_cpu()
{
	init_display();

	clear(display.tmp_terminal_num);

	gdt = init_gdt();
	idt = init_idt();

	init_directory();

	puts(display.tmp_terminal_num, "Hello world from my custom kernel! ;)\n");
}

