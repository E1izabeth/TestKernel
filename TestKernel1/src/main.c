#include <arch/ia32/cpu/cpu.h>
#include <arch/ia32/cpu/tables.h>
#include <arch/ia32/multiboot/multiboot.h>
#include <arch/ia32/threads.h>

// TODO: refactor threading and displays

byte th1Stack[4096];
byte th2Stack[4096];

int sp = -1;
void thread1()
{
	thread_loop(2, "th1");
}

void thread2()
{
	thread_loop(2, "th2");
}

slock_t lock;

void thread_loop(int num, char* msg)
{
	thread_t* t = get_thread();

	for (;;)
	{
		slockCapture(&lock);

		for (int i = 0; i < 10; i++)
			puts(num, msg);
	
		puts(num, "\n");
	
		slockRelease(&lock);
	
		for (int i = 0; i < 1000000; i++);
	}
}

void kernel_main(struct multiboot_info multiboot)
{
	lock = slockInit();

	//asm("movl %%esp, %0" : "=r"(esp));

	init_cpu();
	
	create_thread(thread1, th1Stack, sizeof(th1Stack));
	create_thread(thread2, th2Stack, sizeof(th2Stack));

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

