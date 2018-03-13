#include <cpu/tables.h>
#include <cpu/multiboot.h>
#include <display.h>
#include <cpu/tables.h>
#include <threads.h>

extern display_t display;
thread_t threads[NUM_THREADS];
int tmp_thread;

void irq_handler(registers_t regs)
{
	if (regs.int_no == 32)
	{
		tmp_thread = change_thread(&threads, regs);
	}
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

void thread1(int num)
{
	threads[num].processed = true;
	for (;;)
	{
		thread2(1);
		puts(display.tmp_terminal_num, "th1\n");
	}
	threads[num].processed = false;
	return;
}

void thread2(int num)
{
	threads[num].processed = true;
	for (;;) 
	{
		puts(display.tmp_terminal_num, "th2\n");
	}
	threads[num].processed = false;
	return;
}

void kernel_main(struct multiboot_info multiboot)
{
	//asm("movl %%esp, %0" : "=r"(esp));
	init_threads(threads);
	init_display();

	clear(display.tmp_terminal_num);
	gdt = init_gdt();
	idt = init_idt();
	
	thread1(0);
	
	int x = sizeof(gdt_ptr_t);
	puts(1, "term 1\n");
	puts(2, "term 2\n");
	puts(3, "term 3\n");
	puts(display.tmp_terminal_num, itoa(x));
	
	puts(display.tmp_terminal_num, "Hello world from my custom kernel! ;)\n");
	asm("int3");
	
	for (;;)
		;
	
	asm("hlt");
}

