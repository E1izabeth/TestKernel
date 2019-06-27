#include <arch/ia32/cpu/cpu.h>
#include <arch/ia32/cpu/tables.h>
#include <arch/ia32/multiboot/multiboot.h>
#include <arch/ia32/threads.h>
#include <arch/ia32/mutex.h>
#include <arch/ia32/autoResetEvent.h>
#include <arch/ia32/display.h>
#include <arch/ia32/monitor.h>
#include <arch/ia32/multiboot/multiboot.h>
#include <arch/ia32/cpu/tables.h>
#include <mem.h>

// TODO: refactor threading and displays

byte th1Stack[4096];
byte th2Stack[4096];

int sp = -1;
int k = 0;

// slock_t lock;
mutex_t _sectionMutex;
extern uint placement_address;
uint initial_esp;

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

extern byte* _end;
extern byte* _multiboot_header;

void printMultibootInfo(struct multiboot_info *info)
{
	char buff[20];
	int l = 20;

	print("flags: "); print(utox(info->flags, buff, l)); print("\n");
	if (info->flags >> 0 & 0x01)
	{
		print("mem lower: "); print(utox(info->mem_lower * 1024, buff, l)); print("\n");
		print("mem upper: "); print(utox(info->mem_upper * 1024, buff, l)); print("\n");
	}
	if (info->flags >> 6 & 0x01)
	{
		for (multiboot_memory_map_t *entry = (multiboot_memory_map_t*)(void*)info->mmap_addr;
			(int)entry < (info->mmap_addr + info->mmap_length);
			entry = (multiboot_memory_map_t*)((void*)entry + entry->size + 4))
		{
			print("addr: "); print(ultox(entry->addr, buff, l)); print("\n");
			print("  length: "); print(ultox(entry->len, buff, l)); print("\n");
			print("  type: "); print(utox(entry->type, buff, l)); print("\n");
		}
	}
	print("cmdline: "); print((char*)info->cmdline); print("\n");


	print("start: "); print(ultox((ulong)&_multiboot_header, buff, l)); print("\n");
	print("end: "); print(ultox((ulong)&_end, buff, l)); print("\n");

}

int align_to(int size, int alignment)
{
	return size + (alignment - ((size - 1) % alignment)) - 1;
}

void* get_free_page_address(struct multiboot_info *info)
{
	long s = 4096;

	for (multiboot_memory_map_t *entry = (multiboot_memory_map_t*)(void*)info->mmap_addr;
		(int)entry < (info->mmap_addr + info->mmap_length);
		entry = (multiboot_memory_map_t*)((void*)entry + entry->size + 4))
	{
		int a_rn = align_to(entry->addr, s),
			b_rn = entry->addr + entry->len,
			a_m = &_multiboot_header,
			b_m = align_to(&_end, s);
				
		if (entry->type == 1 && a_rn >= 1024*1024)
		{
			
			if (a_m - a_rn > s)
				return a_rn;
			else if (b_rn - b_m > s)
				return b_m;
		}
	}

	return 0;
}

void* get_page_address(void* source, struct multiboot_info *info)
{
	long s = 4096;

	for (multiboot_memory_map_t *entry = (multiboot_memory_map_t*)(void*)info->mmap_addr;
		(int)entry < (info->mmap_addr + info->mmap_length);
		entry = (multiboot_memory_map_t*)((void*)entry + entry->size + 4))
	{
		int a_rn = align_to(entry->addr, s),
			b_rn = entry->addr + entry->len,
			a_m = &_multiboot_header,
			b_m = align_to(&_end, s);

		if (entry->type == 1 && a_rn >= 1024 * 1024)
		{

			if (a_m - a_rn > s)
				return a_rn;
			else if (b_rn - b_m > s)
				return b_m;
		}
	}

	return 0;
}


int x = 10;

void test(int *p)
{
	if (x == 10)
	{
		*p = 1;
	}
	else
	{
		*p = 0;
	}
}

extern page_directory current_directory;

void test_paging(struct multiboot_info *multiboot)
{
	char buff[20];
	void* oldPageAddress = ((int)&x) / 4096 * 4096;
	void* newPageAddress = get_free_page_address(multiboot);
	memcpy(newPageAddress, oldPageAddress, 4096);

	int x_mod = (int)&x - (int)oldPageAddress;
	int* new_addr_x = newPageAddress + x_mod;
	*new_addr_x = 5;

	int catalog_entry = ((int)&x) / 4096 / 1024;
	int page_entry = ((int)&x) / 4096 % 1204;

	page_table_entry_t *entry = &current_directory.tables[catalog_entry].pages[page_entry];
	page_table_entry_info_t info = decode_page_table_entry(*entry);

	info.physicalAddress = newPageAddress;

	int y;
	test(&y);
	puts(1, "test 1: ");
	puts(1, itoa(y, buff, 20));
	puts(1, ".\n");

	*entry = encode_page_table_entry(info);
	asm volatile("invlpg (%0)" : : "r"(&x));

	test(&y);
	puts(1, "test 2: ");
	puts(1, itoa(y, buff, 20));
	puts(1, ".\n");
}

u32 initial_esp;

void move_stack(void *new_stack_start, u32 size)
{
	u32 i;
	// Выделяем немного места для нового стека.
	for (i = (u32)new_stack_start;
		i >= ((u32)new_stack_start - size);
		i -= 0x1000)
	{
		// Стек общего назначения используется в пользовательском режиме.
		
		//alloc_frame(get_page(i, 1, current_directory), 0 /* User mode */, 1 /* Is writable */);
	}

	// Обновление  TLB выполняется с помощью чтения и повторной записи адреса директория страниц.
	u32 pd_addr;
	asm volatile("mov %%cr3, %0" : "=r" (pd_addr));
	asm volatile("mov %0, %%cr3" : : "r" (pd_addr));

	// Старые ESP и EBP, читаем из регистров.
	u32 old_stack_pointer; asm volatile("mov %%esp, %0" : "=r" (old_stack_pointer));
	u32 old_base_pointer;  asm volatile("mov %%ebp, %0" : "=r" (old_base_pointer));

	u32 offset = (u32)new_stack_start - initial_esp;

	u32 new_stack_pointer = old_stack_pointer + offset;
	u32 new_base_pointer = old_base_pointer + offset;

		// Копирование стека.
	memcpy((void*)new_stack_pointer, (void*)old_stack_pointer, initial_esp - old_stack_pointer);

	// Проходим по исходному стеку и копируем новые значения в новый стек.
	for (i = (u32)new_stack_start; i > (u32)new_stack_start - size; i -= 4)
	{
		u32 tmp = *(u32*)i;
		// Если значение tmp попадает в диапазон адресов старого стека, мы полагаем, что это указатель базы
		// и переопределяем его. В результате, к сожалению, будет переопределено ЛЮБОЕ значение в этом 
		// диапазоне независимо от того, является ли оно указателем базы или нет.
		if ((old_stack_pointer < tmp) && (tmp < initial_esp))
		{
			tmp = tmp + offset;
			u32 *tmp2 = (u32*)i;
			*tmp2 = tmp;
		}
	}

	// Замена стеков.
	asm volatile("mov %0, %%esp" : : "r" (new_stack_pointer));
	asm volatile("mov %0, %%ebp" : : "r" (new_base_pointer));
}


void kernel_main(struct multiboot_info *multiboot, uint initial_stack)
{
	initial_esp = initial_stack;
	init_main_thread();
	init_cpu();

	threading_start();

	printMultibootInfo(multiboot);

	puts(1, "term 1\n");
	puts(2, "term 2\n");
	puts(3, "term 3\n");
	puts(4, "term 4\n");
	puts(5, "term 5\n");
	puts(6, "term 6\n");
	puts(7, "term 7\n");
	puts(8, "term 8\n");
	puts(9, "term 9\n");

	test_paging(multiboot);

	puts(0, "main\n");

	for (;;)
		asm("hlt");
}

