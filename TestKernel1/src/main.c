#include <arch/ia32/cpu/cpu.h>
#include <arch/ia32/cpu/tables.h>
#include <arch/ia32/multiboot/multiboot.h>
#include <arch/ia32/threads.h>

// TODO: refactor threading and displays

byte th1Stack[4096];
byte th2Stack[4096];

void thread1()
{
	thread_loop(2, "th1\n");
}

void thread2()
{
	thread_loop(3, "th2\n");
}

void thread_loop(int num, char* msg)
{
	for (;;)
	{
		puts(num, msg);
	}
}


void kernel_main(struct multiboot_info multiboot)
{
	//asm("movl %%esp, %0" : "=r"(esp));

	init_cpu();
	
	create_thread(thread1, th1Stack);
	// create_thread(thread2, th2Stack);

	threading_start();

	puts(1, "term 1\n");
	puts(2, "term 2\n");
	puts(3, "term 3\n");
	puts(4, "term 4\n");
	puts(5, "term 5\n");
	
	//asm("int3");
	
	for (;;)
		puts(0, "main\n");
	
	asm("hlt");
}

