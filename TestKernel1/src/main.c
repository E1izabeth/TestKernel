#include <arch/ia32/cpu/cpu.h>
#include <arch/ia32/cpu/tables.h>
#include <arch/ia32/multiboot/multiboot.h>
#include <arch/ia32/threads.h>
#include <arch/ia32/mutex.h>
#include <arch/ia32/autoResetEvent.h>
#include <arch/ia32/display.h>
#include <arch/ia32/monitor.h>
#include <mem.h>

// TODO: refactor threading and displays

byte th1Stack[4096];
byte th2Stack[4096];

int sp = -1;
int k=0;

// slock_t lock;
mutex_t _sectionMutex;

void thread_loop(int num, char* msg, bool isLooped)
{
	thread_t* t = get_thread();

	for (;;)
	{
		// slockCapture(&lock);
		_sectionMutex._->wait(&_sectionMutex);
		
		for (int i = 0; i < 10; i++)
			puts(num, msg);


		puts(num, "\n");

		_sectionMutex._->release(&_sectionMutex);

		for (int i = 0; i < 100000000; i++);
		
		k = 1;
		if (!isLooped)
			break;
	}
}

void thread1()
{
	thread_loop(2, "th1", false);
}

void thread2()
{
	for (int i = 0; i < 10; i++)
	{
		thread_loop(2, "th2", false);
	}
}

void print(char* str)
{
	puts(2, str);
}

void printMultibootInfo(struct multiboot_info *info)
{
	char buff[20];
	print("flags: "); print(utox(info->flags, buff)); print("\n");
	if (info->flags >> 0 & 0x01)
	{
		print("mem lower: "); print(utox(info->mem_lower * 1024, buff)); print("\n");
		print("mem upper: "); print(utox(info->mem_upper * 1024, buff)); print("\n");
	}
	if (info->flags >> 6 & 0x01)
	{
		for (multiboot_memory_map_t *entry = (multiboot_memory_map_t*)(void*)info->mmap_addr;
			 (int)entry < (info->mmap_addr + info->mmap_length);
			 entry = (multiboot_memory_map_t*)((void*)entry + entry->size + 4))
		{
			print("addr: "); print(ultox(entry->addr, buff)); print("\n");
			print("  length: "); print(ultox(entry->len, buff)); print("\n");
			print("  type: "); print(utox(entry->type, buff)); print("\n");
		}
	}
	print("cmdline: "); print((char*)info->cmdline); print("\n");
}

void kernel_main(struct multiboot_info *multiboot)
{
	init_main_thread();
	// lock = slockInit();

	//_sectionMutex = newMutex(1, 0);

	//asm("movl %%esp, %0" : "=r"(esp));
	
	init_cpu();
	
	threading_start();
	
	autoResetEventExample();
	//create_thread(thread1, th1Stack, sizeof(th1Stack));
	//create_thread(thread2, th2Stack, sizeof(th2Stack));

	printMultibootInfo(multiboot);

	puts(1, "term 1\n");
	puts(2, "term 2\n");
	puts(3, "term 3\n");
	puts(4, "term 4\n");
	puts(5, "term 5\n");


	//asm("int3");
	for (int i = 0; i < 1000000; i++)
	{
	}

	for (;;)
		puts(0, "main\n");
	
	asm("hlt");
}

